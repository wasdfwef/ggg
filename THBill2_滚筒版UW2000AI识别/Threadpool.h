#ifndef __TOOLUTIL_THREADPOOL_H__
#define __TOOLUTIL_THREADPOOL_H__

#include "Semaphore.h"
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace TOOLUTIL
{
	class Threadpool
	{
	public:
		using Task = std::function<void(void*)>;
		struct TaskInfo
		{
			Task  task;
			void  *pArg;
		};

		explicit Threadpool(int num);
		~Threadpool();

		void start(); // 开始
		void pause(); // 暂停
		void resume();// 继续
		void stop();  // 停止
		void appendTask(const Task& task, void *pArg);
		void removeAllTask();

	private:
		void work();

	public:
		// disable copy and assign construct
		Threadpool(const Threadpool&) = delete;
		Threadpool& operator=(const Threadpool& other) = delete;

	private:
		std::atomic<bool> _is_running; // thread pool manager status
		std::atomic<bool> _is_paused;
		int _thread_num;
		std::vector<std::thread> _threads;
		std::mutex m_mutex;
		std::queue<TaskInfo> _tasks; 
		Semaphore m_sem;

		std::mutex m_mtxCv;
		std::condition_variable m_cvPause;
	};
}


#endif //__TOOLUTIL_THREADPOOL_H__
