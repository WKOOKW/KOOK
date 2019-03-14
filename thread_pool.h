#ifndef KOOK_THREAD_POOL_H_H
#define KOOK_THREAD_POOL_H_H

/*
简单的线程池模块
实现：不懂
*/

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <queue>
using std::queue;
#include "small_log.h"
#include "share_resource.h"

// 如果引入了time就需要定义，不然会冲突
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#ifdef _WIN32
#pragma comment(lib,"pthreadVC1.lib")
#endif

// 一个轻量级的线程池，工作方式：抢占式
// 线程池包含管理器，状态控制，工作计数，任务队列，任务运行记录，工作函数，结果队列，实际使用中只需要编写特定的工作线程即可
// 管理器：用于管理线程的创建，任务的增加，线程的暂停与停止以及线程数量的平衡，所有功能都通过管理器控制
// 状态控制：用于控制线程池的运行状态，主要有<0运行，1暂停，2停止任务，3终止线程池>
// 工作计数：包含记录创建的线程数量，处于工作的线程数量，完成工作的数量累计
// 任务队列：一个通用队列，用于保存要工作的任务数量，线程会以抢占的方式去队列中获取工作任务
// 任务运行记录：当前正在线程池中执行的任务，可以看到任务都在那些线程中执行，主要用于调试
// 工作函数：具体的任务执行函数，由线程池调用，返回false表示执行的结果无效，不需要插入到结果队列中，返回true表示结果需要插入结果队列中
// 结果队列：与任务队列一样，用于保存运行的结果，方便上层应用获取


namespace kook {

	// 全局函数
	// 获取线程ID，在运行线程中调用可以获取线程ID
	unsigned long get_current_thread_id();

	// 线程池任务运行计数，用于记录线程池累计运行的任务数量，此计数器在任务停止与线程结束状态是可以清空的
	// 注意：最好定时清空计数器，防止累加漏出
	class thread_pool_work_counter {
	public:
		thread_pool_work_counter();
		~thread_pool_work_counter();
		unsigned int add();
		void clean();
		unsigned int get();
	protected:
		unsigned int work_count;
		pthread_mutex_t thread_work_count_mutex;   // 工作累计计数锁
	};

	class thread_pool_state   // 管理线程池状态的对象
	{
	public:
		enum TPOOLSTATE {
			RUN,             // 运行
			PAUSE,           // 暂停
			STOP,            // 停止，只停止线程的工作，并不会结束线程
			EXIT             // 退出，先停止线程工作，然后再结束线程
		};
		thread_pool_state();
		~thread_pool_state();
		bool set_thread_max_size(unsigned int size); // 设置最大线程数量，最大不可超过65535
		unsigned int get_thread_max_size();        // 获取被允许的最大线程数量
		unsigned int get_thread_current_size();    // 获取当前已经运行的线程数
		unsigned int add_thread_record();          // 增加线程的计数
		unsigned int del_thread_record();          // 减少线程的计数
		unsigned int add_task_record();            // 增加线程工作任务计数
		unsigned int del_task_record();            // 减少线程工作任务计数
		bool set_thread_control_state(TPOOLSTATE state); // 设置线程池的控制状态
		unsigned int get_thread_control_state();   // 获取管理控制线程的状态
		unsigned int get_thread_run_state();       // 获取线程的运行状态
		bool reset_thread_run_state(TPOOLSTATE state);   // 重置线程池运行状态
		unsigned int get_thread_work_size();       // 获取工作中的线程数量
		thread_pool_work_counter work_counter;     // 工作累计器，这里直接由外部调用
	protected:
		unsigned int thread_max_size;        // 标识线程池中被允许的最大线程数，如果当线程池中的线程数量大于此数时，要求线程自动退出一部分，以稳定在此数量范围内
		unsigned int thread_current_size;    // 标识当前已经运行的线程数
		unsigned int thread_control_state;   // 用于控制线程运行状态的标识(0运行,1暂停,2停止(当收到状态2时，要求线程完成当前工作后停止工作，但不线束线程),3终止(当收到状态3时，要求线程在完成工作后退出))
		unsigned int thread_run_state;       // 标识线程当前运行的状态(0运行,1暂停,2停止(只是结束工作),3终止(结束工作并退出线程))
		unsigned int thread_work_size;       // 记录当前有多少线程在工作
		pthread_mutex_t thread_work_mutex;   // 工作线程工作记数锁
		pthread_mutex_t thread_count_mutex;  // 线程数量计数锁
	};


	class state_signal {         // 线程池状态信号对象，只用于传递给工作线程函数使用
	public:
		state_signal(thread_pool_state *pstate = NULL) {
			this->pstate = pstate;
			DEFERRORSTATE = 65535;
		}
		// 重置线程状态对象指针
		void reset(thread_pool_state *pstate = NULL) {
			this->pstate = pstate;
		}
		// 获取线程池的控制状态
		unsigned int get_state() {
			if (NULL == this->pstate) {
				return DEFERRORSTATE;
			}
			return this->pstate->get_thread_control_state();
		}
		// 判断控制状态是否在有效范围内
		bool is_state(unsigned int state) {
			if (state < 0 || state > 3) {
				return false;
			}
			return true;
		}
		// 处理暂停，因为任务的暂停也使用到了计数器，所以在处理暂停时需要做特殊处理
		bool pause() {
			if (NULL == this->pstate) {
				return false;
			}
			if (this->pstate->get_thread_control_state() != thread_pool_state::PAUSE) {
				return true;
			}
			// 进入暂停后开始减少任务计数
			this->pstate->del_task_record();
			while ((this->pstate->get_thread_control_state() == thread_pool_state::PAUSE)) {
				kook::share_resource::sleep(50);
			}
			// 暂停结束后还原任务计数
			this->pstate->add_task_record();
			return true;
		}
	protected:
		thread_pool_state *pstate;
		unsigned int DEFERRORSTATE;
	};

	// 特殊的任务队列兼结果队列
	template<class data_type>
	class data_queue
	{
	public:
		data_queue();
		~data_queue();
		// 向队列追加元素
		bool push(data_type element);
		// 弹出一个元素，并删除已经弹出的元素，弹出的元素从ret_data返回
		bool pop(data_type *out_data = NULL);
		// 查看元素，并不删除，元素从ret_data返回
		bool view(data_type *out_data = NULL);
		// 判断队列是否为空
		bool empty();
		// 返回队列元素个数
		size_t size();
		// 清空队列元素
		bool clear();
	protected:
		// 重新包装queue来实现，不自己造轮子，这里只给队列加锁
		// list<data_type> arietas_data_queue;
		queue<data_type> arietas_data_queue;
		// 增加一个独立的数量计数器，解决list的size是O(n)的问题
		// list的size是一个大坑，时间复杂居然是O(n)，windows已做了优化，但linux没有，所以要注意
		// unsigned int data_queue_size;
		// 使用读写锁来处理数据读取与更新
		pthread_rwlock_t queue_oper_mutex;
	};

	template<class data_type>
	data_queue<data_type>::data_queue()
	{
		pthread_rwlock_init(&queue_oper_mutex, NULL);
	}

	template<class data_type>
	data_queue<data_type>::~data_queue()
	{
		pthread_rwlock_destroy(&queue_oper_mutex);
	}

	template<class data_type>
	bool data_queue<data_type>::push(data_type element)
	{
		pthread_rwlock_wrlock(&queue_oper_mutex);
		arietas_data_queue.push(element);
		pthread_rwlock_unlock(&queue_oper_mutex);
		return true;
	}

	template<class data_type>
	bool data_queue<data_type>::pop(data_type *out_data)
	{
		if (NULL == out_data) {
			return false;
		}
		pthread_rwlock_wrlock(&queue_oper_mutex);
		if (arietas_data_queue.empty()) {
			pthread_rwlock_unlock(&queue_oper_mutex);
			return false;
		}
		*out_data = arietas_data_queue.front();
		arietas_data_queue.pop();
		pthread_rwlock_unlock(&queue_oper_mutex);
		return true;
	}

	template<class data_type>
	bool data_queue<data_type>::view(data_type *out_data)
	{
		if (NULL == out_data) {
			return false;
		}
		pthread_rwlock_rdlock(&queue_oper_mutex);
		if (arietas_data_queue.empty()) {
			pthread_rwlock_unlock(&queue_oper_mutex);
			return false;
		}
		*out_data = arietas_data_queue.front();
		pthread_rwlock_unlock(&queue_oper_mutex);
		return true;
	}

	template<class data_type>
	bool data_queue<data_type>::empty()
	{
		bool temp_empty;
		pthread_rwlock_rdlock(&queue_oper_mutex);
		temp_empty = arietas_data_queue.empty();
		pthread_rwlock_unlock(&queue_oper_mutex);
		return temp_empty;
	}

	template<class data_type>
	size_t data_queue<data_type>::size()
	{
		size_t data_size = 0;
		pthread_rwlock_rdlock(&queue_oper_mutex);
		data_size = arietas_data_queue.size();
		pthread_rwlock_unlock(&queue_oper_mutex);
		return data_size;
	}

	template<class data_type>
	bool data_queue<data_type>::clear()
	{
		pthread_rwlock_wrlock(&queue_oper_mutex);
		// 通过swap来清空队列并释放空间
		queue<data_type>().swap(arietas_data_queue);
		pthread_rwlock_unlock(&queue_oper_mutex);
		return true;
	}


	// 当前线程运行任务记录字典
	template<class thread_id, class task_data>
	class thread_current_task {
	public:
		thread_current_task() {
			pthread_rwlock_init(&map_oper_mutex, NULL);
		}
		~thread_current_task() {
			pthread_rwlock_destroy(&map_oper_mutex);
		}
		// 清空数据
		void clear() {
			// 一般这里不会保存太多数据，因为线程数量有限，所以只需要清空即可
			pthread_rwlock_wrlock(&map_oper_mutex);
			current_task.clear();
			pthread_rwlock_unlock(&map_oper_mutex);
		}
		// 更新与插入数据，pid存在就更新，不存在就插入
		bool update(thread_id pid, task_data tdata) {
			pthread_rwlock_wrlock(&map_oper_mutex);
			// 使用简单的方法实现
			current_task[pid] = tdata;
			pthread_rwlock_unlock(&map_oper_mutex);
			return true;
		}
		// 增加，key不存在就增加，如果存在就增加失败
		bool add(thread_id pid, task_data tdata) {
			// 通过判断返回值来知道插入是否成功，不成功可能是id重复，可以尝试重新插入
			// typename关键字用于指出模板声明（或定义）中的非独立名称
			std::pair<typename map<thread_id, task_data>::iterator, bool> insert_ptr;
			// 加锁写入，写入的时候不允许其它线程访问
			pthread_rwlock_wrlock(&map_oper_mutex);
			insert_ptr = current_task.insert(std::pair<thread_id, task_data>(pid, tdata));
			pthread_rwlock_unlock(&map_oper_mutex);
			return insert_ptr.second;
		}
		// 判断key是否存在
		bool is_key(thread_id pid) {
			pthread_rwlock_rdlock(&map_oper_mutex);
			// typename关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名
			typename map<thread_id, task_data>::iterator pdata = current_task.find(pid);
			if (pdata == current_task.end()) {
				pthread_rwlock_unlock(&map_oper_mutex);
				return false;
			}
			pthread_rwlock_unlock(&map_oper_mutex);
			return true;
		}
		// 删除pid任务数据
		bool erase(thread_id pid) {
			pthread_rwlock_wrlock(&map_oper_mutex);
			// typename关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名
			// 如果不指出，编译器将会产生歧义，不知道定义的是一个类型名还是一个模块名
			typename map<thread_id, task_data>::iterator pdata = current_task.find(pid);
			if (pdata == current_task.end()) {
				// 如果不存在此pid也认为删除成功
				pthread_rwlock_unlock(&map_oper_mutex);
				return true;
			}
			current_task.erase(pdata);
			pthread_rwlock_unlock(&map_oper_mutex);
			return true;
		}
		// 获取pid任务数据
		bool get(thread_id pid, task_data &out_tdata) {
			pthread_rwlock_rdlock(&map_oper_mutex);
			// typename关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名
			typename map<thread_id, task_data>::iterator pdata = current_task.find(pid);
			if (pdata == current_task.end()) {
				pthread_rwlock_unlock(&map_oper_mutex);
				return false;
			}
			out_tdata = pdata->second;
			pthread_rwlock_unlock(&map_oper_mutex);
			return true;
		}
		// 获取当前条目数量
		size_t get_size() {
			size_t temp_size = 0;
			pthread_rwlock_rdlock(&map_oper_mutex);
			temp_size = current_task.size();
			pthread_rwlock_unlock(&map_oper_mutex);
			return temp_size;
		}
		// 获取所有当前运行任务数据，只返回数据
		bool get_all(vector<task_data> &out_result);

	protected:
		// 存储当前任务字典，格式{thread_id:task_data}
		map<thread_id, task_data> current_task;
		// 操作锁，使用读写锁不防止误操作
		pthread_rwlock_t map_oper_mutex;
	};

	template<class thread_id, class task_data>
	bool thread_current_task<thread_id, task_data>::get_all(vector<task_data> &out_result) {
		//  这里不负责清空容器，需要上层应用自己清空
		pthread_rwlock_rdlock(&map_oper_mutex);
		// typename关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名
		typename map<thread_id, task_data>::iterator pdata = current_task.begin();
		for (; pdata != current_task.end(); pdata++) {
			out_result.push_back(pdata->second);
		}
		pthread_rwlock_unlock(&map_oper_mutex);
		return true;
	}



	// 工作线程的函数指针
	// 注意task_data参数的类型是模板task类型，指定具体任务的数据
	// 注意out_result参数的类型为模板result类型，指定具体结果的数据，返回的结果通过此参数返回
	// 如果返回false表示任务运行异常，并不将结果保存到结果队列中，如果返回true表示任务运行成功，并将out_result结果保存到结果队列中
	// signal为事件对象，用于给工作线程来捕获事件，slog为日志对象，用于给工作线程记录日志
	typedef bool(*work_call)(state_signal &signal, void* task_data, void* out_result, small_log &slog);

	template<class task, class result>
	class manage_gather
	{
	public:
		manage_gather() :is_del_task(true), is_del_result(true) {
			wcall = 0;
			task_queue_pt = 0;
			result_queue_pt = 0;
		}
		~manage_gather() {
			if (is_del_task && 0 != task_queue_pt) delete task_queue_pt;
			if (is_del_result && 0 != result_queue_pt) delete result_queue_pt;
		}
		data_queue<task> *create_task(bool is_del = true, data_queue<task> *T = 0);
		data_queue<result> *create_result(bool is_del = true, data_queue<result> *R = 0);
		//
		thread_pool_state tp_state;      // 线程状态控制对象
		//
		data_queue<task> *task_queue_pt;     // 任务队列
		bool is_del_task;                    // 用于对接外部队列时，是否需要在析构时delete，true表示需要delete
		data_queue<result> *result_queue_pt; // 结果队列
		bool is_del_result;
		//
		thread_current_task<unsigned long int, task> run_task;   // 记录当前正在运行的任务
		small_log slog;                  // 日志记录对象
		work_call wcall;                 // 工作回调函数地址
	};

	template<class task, class result>
	data_queue<task> *manage_gather<task, result>::create_task(bool is_del, data_queue<task> *T) {
		if (0 != T && is_del_task && 0 != task_queue_pt) {
			// 如果已存在要释放的队列，就先释放，这里没加锁，要注意使用
			delete task_queue_pt;
			task_queue_pt = 0;
		}
		if (0 != T) task_queue_pt = T;
		// 如果外部没有队列接入就创建新的
		if (0 == task_queue_pt) task_queue_pt = new data_queue<task>;
		is_del_task = is_del;
		return task_queue_pt;
	}

	template<class task, class result>
	data_queue<result> *manage_gather<task, result>::create_result(bool is_del, data_queue<result> *R) {
		if (0 != R && is_del_result && 0 != result_queue_pt) {
			// 如果已存在要释放的队列，就先释放，这里没加锁，要注意使用
			delete result_queue_pt;
			result_queue_pt = 0;
		}
		if (0 != R) result_queue_pt = R;
		// 如果外部没有队列接入就创建新的
		if (0 == result_queue_pt) result_queue_pt = new data_queue<result>;
		is_del_result = is_del;
		return result_queue_pt;
	}

	// 线程池管理对象
	template<class task, class result>
	class thread_pool_manage
	{
	public:
		thread_pool_manage(work_call wcall = 0);
		/*
		可接受外部的线程池队列，主要是为了方便多个线程池的数据流动，如将线程池A的结果流动到线程池B的任务队列，线程池B依据A的结果进行下一步工作。
		典型的示例如：网络层接收到数据，将数据放入数据处理线程池解析，数据处理线程池将解析好的数据直接流入业务处理线程池，业务处理线程池只需要处理业务，
		业务处理完直接将结果压入结果队列
		*/
		// 如果队列等于0，表示不需要外部的队列，由内部自动创建
		// 注意：如果是接受的外部队列，必须由外部队列拥有者释放，这里不会释放
		thread_pool_manage(data_queue<task> *T, data_queue<result> *R, work_call wcall = 0);
		~thread_pool_manage();
		// 初始化线程池，目前只初始化了控制状态，其它数据的初始化请手动
		// 如果队列的初始化可以手动调用清除函数来初始
		// 在运行的过程中也是禁止初始化的
		bool init_tp();
		// 设置线程池最大线程数量
		bool set_max_thread_size(unsigned int size);
		// 获取当前设置的最大线程数量
		unsigned int get_max_thread_size();
		// 获取当前已经运行的线程数量
		unsigned int get_run_thread_size();
		// 获取当前任务累计数量
		unsigned int get_work_count();
		// 清空任务累计数量，只有在停止与终止的状态下才能清空
		bool clean_work_count();
		// 从暂停恢复到运行
		bool run();
		// 从运行状态到暂停
		bool pause();
		// 从运行或暂停状态到任务停止
		bool stop();
		// 从运行或暂停状态到线程停止并结束
		bool exit();
		// 获取线程池控制状态，即线程池下一步想要变更到运行状态，如停止，结束等
		unsigned int get_thread_control_state();
		// 获取线程池运行状态，线程池当前实际工作状态
		unsigned int get_thread_run_state();
		// 获取线程池的工作线程数量
		unsigned int get_thread_work_size();
		bool start_thread_pool();   // 启动线程池
		// 重置工作回调函数
		bool reset_work_call(work_call wcall = NULL);
		// 增加任务
		bool add_task(task _task);
		// 获取任务队列中的任务数量
		size_t get_task_size();
		// 判断任务队列是否为空，为空返回true
		bool is_task();
		// 清空任务队列
		bool clear_task();
		// 获取结果，一次只获取一个结果，失败返回false
		bool get_result(result *_result);
		// 获取结果队列中结果的数量
		size_t get_result_size();
		// 判断结果队列是否为空，为空返回true
		bool is_result();
		// 清空结果队列
		bool clear_result();
		// 获取当前运行任务数量，这个数量与get_thread_work_size的数量基本一致
		unsigned long int get_run_task_size() {
			return _gather.run_task.get_size();
		}
		// 获取当前运行的任务列表
		bool get_all_run_task(vector<task> &out_result) {
			return _gather.run_task.get_all(out_result);
		}
		// 清空当前运行任务字典
		void clear_run_task() {
			_gather.run_task.clear();
		}
		// 特殊函数，返回一个日志对象，用于记录日志，主要是为了统一日志记录
		small_log *get_slog_obj();
		// 特殊函数，返回任务队列对象
		data_queue<task> *get_task_queue_obj();
		// 特殊函数，获取结果队列
		data_queue<result> *get_result_queue_obj();
	protected:
		// 监视状态线程回调函数
		static void *thread_state_monitor(void *data);
		// 具体的工作线程 
		static void *work_thread(void *data);
		// 创建线程状态监视线程，并且需要维护线程数量的平衡
		bool create_state_monitor();
		manage_gather<task, result> _gather;
		pthread_t smonitor;     // 状态监视服务线程
	};


	template<class task, class result>
	thread_pool_manage<task, result>::thread_pool_manage(work_call wcall)
	{
		_gather.wcall = wcall;
		_gather.create_task();
		_gather.create_result();
	}

	template<class task, class result>
	thread_pool_manage<task, result>::thread_pool_manage(data_queue<task> *T, data_queue<result> *R, work_call wcall) {
		_gather.wcall = wcall;
		if (0 == T) _gather.create_task();
		else _gather.create_task(false, T);
		if (0 == R) _gather.create_result();
		else _gather.create_result(false, R);
	}

	template<class task, class result>
	thread_pool_manage<task, result>::~thread_pool_manage()
	{
		// 做线程的退出与内存的清理
		if (_gather.tp_state.get_thread_run_state() == _gather.tp_state.EXIT) {
			// 如果线程池已经退出就直接返回
			return;
		}
		small_log_warning(_gather.slog,"thread pool no exit , prepare exit");
		// 退出线程
		_gather.tp_state.set_thread_control_state(thread_pool_state::EXIT);
		int exit_count = 0;
		while (exit_count < 10) {
			if (_gather.tp_state.get_thread_run_state() == _gather.tp_state.EXIT) {
				// 如果线程池已经退出就直接返回
				return;
			}
			exit_count++;
			share_resource::sleep(100);
		}
		// 如果没有退出将输出错误
		if (exit_count >= 10) {
			small_log_urgency(_gather.slog, "thread pool no exit , be abnormal end , there might be resource leaks , will lead to the instability of the program");
		}
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::init_tp() {
		// 初始化线程池状态与队列
		if (_gather.tp_state.get_thread_run_state() != thread_pool_state::EXIT) {
			// 在运行的过程禁止初始化
			small_log_debug(_gather.slog, "be being run thread pool , forbid init");
			return false;
		}
		// 暂时只初始化控制状态，不做其它操作
		_gather.tp_state.set_thread_control_state(thread_pool_state::RUN);
		return true;
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::set_max_thread_size(unsigned int size) {
		return _gather.tp_state.set_thread_max_size(size);
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_max_thread_size() {
		return _gather.tp_state.get_thread_max_size();
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_run_thread_size() {
		return _gather.tp_state.get_thread_current_size();
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_work_count() {
		return _gather.tp_state.work_counter.get();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::clean_work_count() {
		unsigned int temp_state = _gather.tp_state.get_thread_run_state();
		if (temp_state != thread_pool_state::STOP && temp_state != thread_pool_state::EXIT) {
			return false;
		}
		_gather.tp_state.work_counter.clean();
		return true;
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_thread_work_size() {
		return _gather.tp_state.get_thread_work_size();
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_thread_control_state() {
		return _gather.tp_state.get_thread_control_state();
	}

	template<class task, class result>
	unsigned int thread_pool_manage<task, result>::get_thread_run_state() {
		return _gather.tp_state.get_thread_run_state();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::reset_work_call(work_call wcall /* = NULL */) {
		_gather.wcall = wcall;
		return true;
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::start_thread_pool()
	{
		return create_state_monitor();
	}

	template<class task, class result>
	small_log *thread_pool_manage<task, result>::get_slog_obj() {
		return &_gather.slog;
	}

	template<class task, class result>
	data_queue<task> *thread_pool_manage<task, result>::get_task_queue_obj() {
		return _gather.task_queue_pt;
	}

	template<class task, class result>
	data_queue<result> *thread_pool_manage<task, result>::get_result_queue_obj() {
		return _gather.result_queue_pt;
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::run() {
		if (_gather.tp_state.get_thread_run_state() == thread_pool_state::EXIT) {
			return false;
		}
		return _gather.tp_state.set_thread_control_state(thread_pool_state::RUN);
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::pause() {
		unsigned int tp_state = _gather.tp_state.get_thread_run_state();
		if (thread_pool_state::EXIT == tp_state || thread_pool_state::STOP == tp_state) {
			return false;
		}
		return _gather.tp_state.set_thread_control_state(thread_pool_state::PAUSE);
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::stop() {
		if (_gather.tp_state.get_thread_run_state() == thread_pool_state::EXIT) {
			return false;
		}
		return _gather.tp_state.set_thread_control_state(thread_pool_state::STOP);
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::exit() {
		// 禁止重复exit操作
		if (_gather.tp_state.get_thread_run_state() == thread_pool_state::EXIT) {
			return false;
		}
		return _gather.tp_state.set_thread_control_state(thread_pool_state::EXIT);
	}

	template<class task, class result>
	size_t thread_pool_manage<task, result>::get_task_size() {
		return _gather.task_queue_pt->size();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::is_task() {
		return _gather.task_queue_pt->empty();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::clear_task() {
		return _gather.task_queue_pt->clear();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::add_task(task _task) {
		// 在任何状态下都可以增加任务，如果需要控制，可以在上层来控制
		return _gather.task_queue_pt->push(_task);
	}

	template<class task, class result>
	size_t thread_pool_manage<task, result>::get_result_size() {
		return _gather.result_queue_pt->size();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::is_result() {
		return _gather.result_queue_pt->empty();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::clear_result() {
		return _gather.result_queue_pt->clear();
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::get_result(result *_result) {
		return _gather.result_queue_pt->pop(_result);
	}

	template<class task, class result>
	bool thread_pool_manage<task, result>::create_state_monitor()
	{
		int pRet = 0;
		char error_info_buf[SLOG_BUF_SIZE_B] = "";
		// 判断线程是否存在，如果已经存在就不再创建，这里通过状态判断
		if (_gather.tp_state.get_thread_run_state() != _gather.tp_state.EXIT) {
			small_log_warning(_gather.slog, "thread pool already running");
			return false;
		}
		if (NULL == _gather.wcall) {
			small_log_warning(_gather.slog, "thread pool work call func is null , thread will be free");
		}
		pRet = pthread_create(&smonitor, NULL, thread_state_monitor, &_gather);
		if (pRet != 0) {
			STRFORMAT(error_info_buf, sizeof(error_info_buf), "create thread pool state monitor server fail, code: %d", pRet);
			small_log_error(_gather.slog, error_info_buf);
			return false;
		}
		// 分离线程，使它脱离主线程，这样线程在退出后就会回收资源
		pthread_detach(smonitor);
		return true;
	}

	template<class task, class result>
	void *thread_pool_manage<task, result>::thread_state_monitor(void *data)
	{
		unsigned int ret = 0;
		int pRet = 0;
		char error_info_buf[SLOG_BUF_SIZE_B] = "";
		pthread_t temp_pthread;
		manage_gather<task, result> *pgather = (manage_gather<task, result> *)data;
		while (true) {
			// 检查状态并更新状态
			ret = pgather->tp_state.get_thread_control_state();
			if (1 == ret && 0 == pgather->tp_state.get_thread_work_size()) {
				// 当所以线程都已经停止工作时就表示暂停成功
				pgather->tp_state.reset_thread_run_state(pgather->tp_state.PAUSE);
			}
			else if (2 == ret && 0 == pgather->tp_state.get_thread_work_size()) {
				// 当所以线程都停止工作时就表示停止任务成功
				pgather->tp_state.reset_thread_run_state(pgather->tp_state.STOP);
			}
			else if (3 == ret) {
				// 等待线程池的线程退出
				if (0 != pgather->tp_state.get_thread_current_size()) {
					share_resource::sleep(50);
					continue;
				}
				small_log_debug(pgather->slog, "thread pool has completed the mission and exit");
				// 当所有线程都退出后表示线程池结束成功
				pgather->tp_state.reset_thread_run_state(pgather->tp_state.EXIT);
				// 重新校对状态，不然无法重复使用线程池
				pgather->tp_state.set_thread_control_state(thread_pool_state::RUN);
				break;
			}
			else {
				pgather->tp_state.reset_thread_run_state(pgather->tp_state.RUN);
			}
			// 创建线程池工作线程
			ret = 0;
			while (pgather->tp_state.get_thread_current_size() < pgather->tp_state.get_thread_max_size() && ret < 10) {
				pRet = pthread_create(&temp_pthread, NULL, work_thread, data);
				if (pRet != 0) {
					STRFORMAT(error_info_buf, sizeof(error_info_buf), "create work thread fail, code: %d, current run work thread size: %d", pRet, pgather->tp_state.get_thread_current_size());
					small_log_debug(pgather->slog, error_info_buf);
					ret++;
					continue;
				}
				// 分离线程，使它脱离主线程
				pthread_detach(temp_pthread);
				// 增加工作线程计数器
				pgather->tp_state.add_thread_record();
			}
			if (ret >= 10) {
				// 如果连续10次创建线程失败就退出
				STRFORMAT(error_info_buf, sizeof(error_info_buf), "continuous 10 count create work thread fail , ready to end thread pool, count: %i", ret);
				small_log_error(pgather->slog, error_info_buf);
				pgather->tp_state.set_thread_control_state(pgather->tp_state.EXIT);
			}
			share_resource::sleep(50);
		}
		small_log_info(pgather->slog, "thread pool state monitor server exit");
		return NULL;
	}

	template<class task, class result>
	void *thread_pool_manage<task, result>::work_thread(void *data) {
		manage_gather<task, result> *pgather = (manage_gather<task, result> *)data;
		state_signal signal(&pgather->tp_state);
		unsigned int ret = 0;
		task task_data;
		result result_data;
		bool task_run_bool = false;
		//  记录线程id，用于记录当前运行任务
		char debug_info_buf[SLOG_BUF_SIZE_B] = "";
		unsigned long int pid = get_current_thread_id();
		STRFORMAT(debug_info_buf, sizeof(debug_info_buf), "new work thread id: %lu", pid);
		small_log_debug(pgather->slog, debug_info_buf);
		while (true) {
			// 首先捕获状态
			ret = pgather->tp_state.get_thread_control_state();
			if (1 == ret || 2 == ret) {
				// 暂停或停止执行任务
				share_resource::sleep(50);
				continue;
			}
			else if (3 == ret) {
				// 退出线程
				break;
			}
			// 平衡线程数量
			if (pgather->tp_state.get_thread_current_size() > pgather->tp_state.get_thread_max_size()) {
				small_log_warning(pgather->slog, "thread pool thread count overflow, based on the number of threads balance mechanism, end thread itself");
				break;
			}
			// 检查线程工作函数是否为空
			if (NULL == pgather->wcall) {
				share_resource::sleep(50);
				continue;
			}
			// 获取任务
			if (pgather->task_queue_pt->empty()) {
				share_resource::sleep(50);
				continue;
			}
			if (!pgather->task_queue_pt->pop(&task_data)) {
				share_resource::sleep(50);
				continue;
			}
			// 增加线程任务计数
			pgather->tp_state.add_task_record();
			// 记录当前任务，方便上层应用获取
			pgather->run_task.update(pid, task_data);

			// 开始执行任务，这里没有清空上次结果，所以需要使用者手动清
			try {
				task_run_bool = pgather->wcall(signal, (void *)&task_data, (void *)&result_data, pgather->slog);
			}
			catch (...) {
				small_log_warning(pgather->slog, "work function call exception , unknown error");
				task_run_bool = false;
			}
			if (task_run_bool) {
				// 将结果保存到结果队列中
				pgather->result_queue_pt->push(result_data);
			}
			else {
				small_log_debug(pgather->slog, "work call back func return false , result will not save");
			}
			// 增加任务累计
			pgather->tp_state.work_counter.add();
			// 清除已经运行完成的任务
			pgather->run_task.erase(pid);
			// 当任务执行完后，减少线程任务计数
			pgather->tp_state.del_task_record();
		}
		STRFORMAT(debug_info_buf, sizeof(debug_info_buf), "work thread exit, thread id: %lu", pid);
		small_log_info(pgather->slog, debug_info_buf);
		pgather->tp_state.del_thread_record();
		return NULL;
	}


	// 简单封装的锁对象，通过创建对象来加锁，然后通过析构来释放锁
	// 注意：这里不会对锁进行初始化和销毁
	namespace simple_pthread_mutex {

		class simple_work_rlock {
		public:
			simple_work_rlock(pthread_rwlock_t *lock);
			~simple_work_rlock();
		private:
			pthread_rwlock_t *rwlock;
		};

		class simple_work_wlock {
		public:
			simple_work_wlock(pthread_rwlock_t *lock);
			~simple_work_wlock();
		private:
			pthread_rwlock_t *rwlock;
		};

		class simple_work_mutex {
		public:
			simple_work_mutex(pthread_mutex_t *lock);
			~simple_work_mutex();
		private:
			pthread_mutex_t *mutex;
		};

	}
}

#endif
