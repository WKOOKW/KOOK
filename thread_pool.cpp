#include "thread_pool.h"

//thread_pool_state/////////////////////////////////////////////
namespace kook {

	thread_pool_work_counter::thread_pool_work_counter() :work_count(0) {
		pthread_mutex_init(&thread_work_count_mutex, NULL);
	}

	thread_pool_work_counter::~thread_pool_work_counter() {
		pthread_mutex_destroy(&thread_work_count_mutex);
	}

	unsigned int thread_pool_work_counter::add() {
		pthread_mutex_lock(&thread_work_count_mutex);
		work_count += 1;
		pthread_mutex_unlock(&thread_work_count_mutex);
		return work_count;
	}

	void thread_pool_work_counter::clean() {
		pthread_mutex_lock(&thread_work_count_mutex);
		work_count = 0;
		pthread_mutex_unlock(&thread_work_count_mutex);
	}

	unsigned int thread_pool_work_counter::get() {
		return work_count;
	}


	thread_pool_state::thread_pool_state()
	{
		thread_max_size = 5;
		thread_current_size = 0;
		thread_control_state = RUN;
		thread_run_state = EXIT;
		thread_work_size = 0;
		pthread_mutex_init(&thread_count_mutex, NULL);
		pthread_mutex_init(&thread_work_mutex, NULL);
	}

	thread_pool_state::~thread_pool_state()
	{
		pthread_mutex_destroy(&thread_count_mutex);
		pthread_mutex_destroy(&thread_work_mutex);
	}

	bool thread_pool_state::set_thread_max_size(unsigned int size) {
		if (size < 0 || size > 65535) {
			return false;
		}
		thread_max_size = size;
		return true;
	}

	unsigned int thread_pool_state::get_thread_max_size()
	{
		return thread_max_size;
	}

	unsigned int thread_pool_state::get_thread_current_size()
	{
		return thread_current_size;
	}

	bool thread_pool_state::set_thread_control_state(TPOOLSTATE state) {
		if (state < 0 || state > 3) {
			return false;
		}
		thread_control_state = state;
		return true;
	}

	unsigned int thread_pool_state::get_thread_control_state()
	{
		return thread_control_state;
	}

	unsigned int thread_pool_state::get_thread_run_state() {
		return thread_run_state;
	}

	bool thread_pool_state::reset_thread_run_state(TPOOLSTATE state) {
		if (state < 0 || state > 3) {
			return false;
		}
		thread_run_state = state;
		return true;
	}

	unsigned int thread_pool_state::get_thread_work_size() {
		return thread_work_size;
	}

	unsigned int thread_pool_state::del_thread_record()
	{//加锁修改线程数量
		pthread_mutex_lock(&thread_count_mutex);
		if (thread_current_size > 0) {
			thread_current_size -= 1;
		}
		pthread_mutex_unlock(&thread_count_mutex);
		return thread_current_size;
	}

	unsigned int thread_pool_state::add_thread_record()
	{
		//加锁修改线程数量
		pthread_mutex_lock(&thread_count_mutex);
		thread_current_size += 1;
		pthread_mutex_unlock(&thread_count_mutex);
		return thread_current_size;
	}

	unsigned int thread_pool_state::add_task_record() {
		pthread_mutex_lock(&thread_work_mutex);
		thread_work_size += 1;
		pthread_mutex_unlock(&thread_work_mutex);
		return thread_work_size;
	}

	unsigned int thread_pool_state::del_task_record() {
		pthread_mutex_lock(&thread_work_mutex);
		if (thread_work_size > 0) {
			thread_work_size -= 1;
		}
		pthread_mutex_unlock(&thread_work_mutex);
		return thread_work_size;
	}

	
	namespace simple_pthread_mutex {

		simple_work_rlock::simple_work_rlock(pthread_rwlock_t *lock) {
			rwlock = lock;
			pthread_rwlock_rdlock(rwlock);
		}

		simple_work_rlock::~simple_work_rlock() {
			pthread_rwlock_unlock(rwlock);
		}

		simple_work_wlock::simple_work_wlock(pthread_rwlock_t *lock) {
			rwlock = lock;
			pthread_rwlock_wrlock(rwlock);
		}

		simple_work_wlock::~simple_work_wlock() {
			pthread_rwlock_unlock(rwlock);
		}

		simple_work_mutex::simple_work_mutex(pthread_mutex_t *lock) {
			mutex = lock;
			pthread_mutex_lock(mutex);
		}

		simple_work_mutex::~simple_work_mutex() {
			pthread_mutex_unlock(mutex);
		}

	}

	unsigned long get_current_thread_id() {
#ifdef _WIN32
		return (unsigned long)GetCurrentThreadId();
#else
		return pthread_self();
#endif
	}

}
