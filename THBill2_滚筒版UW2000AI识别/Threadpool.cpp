#include "stdafx.h"
#include "Threadpool.h"

namespace TOOLUTIL
{

	Threadpool::Threadpool(int num)
		: _thread_num(num)
		, _is_running(false)
		, _is_paused(false)
		, m_sem("Threadpool semaphore")
	{

	}

	Threadpool::~Threadpool()
	{
		if (_is_running)
			stop();
	}

	void Threadpool::start()
	{
		if (_is_running)
		{
			return;
		}
		_is_running = true;

		// start threads
		for (int i = 0; i < _thread_num; i++)
		{
			_threads.emplace_back(std::thread(&Threadpool::work, this));
		}
	}

	void Threadpool::pause()
	{
		std::unique_lock<std::mutex> lck(m_mtxCv);
		_is_paused = true;
	}

	void Threadpool::resume()
	{
		std::unique_lock<std::mutex> lck(m_mtxCv);
		_is_paused = false;
		m_cvPause.notify_all();
	}

	void Threadpool::stop()
	{
		resume();
		{
			// stop thread pool, should notify all threads to wake
			_is_running = false;
			m_sem.signalAll(); // must do this to avoid thread block
		}

		removeAllTask();

		// terminate every thread job
		for (std::thread& t : _threads)
		{
			if (t.joinable())
				t.join();
		}
	}

	void Threadpool::appendTask(const Task& task, void *pArg)
	{
		//if (_is_running)
		{
			{
				std::lock_guard<std::mutex> lck(m_mutex);
				_tasks.emplace(TaskInfo{ task, pArg });
			}
			m_sem.signal(); // wake a thread to to the task
		}
	}

	void Threadpool::removeAllTask()
	{
		std::lock_guard<std::mutex> lck(m_mutex);
		_tasks.swap(std::queue<TaskInfo>());
	}

	void Threadpool::work()
	{
		printf("begin work thread: %d\n", std::this_thread::get_id());

		// every thread will compete to pick up task from the queue to do the task
		while (_is_running)
		{
			{
				std::unique_lock<std::mutex> lck(m_mtxCv);
				m_cvPause.wait(lck, [&]{return !_is_paused.load(); });
				/*while (_is_paused)
				{
					m_cvPause.wait(lck);
				};*/
			}

			TaskInfo taskInfo;
			{
				bool bTaskEmpty = true;
				{
					std::lock_guard<std::mutex> lck(m_mutex);
					if (!_tasks.empty())
					{
						// if tasks not empty, 
						// must finish the task whether thread pool is running or not
						taskInfo = _tasks.front();
						_tasks.pop(); // remove the task
						bTaskEmpty = false;
					}
					else
					{
						bTaskEmpty = _tasks.empty();
					}
				}
				if (_is_running && bTaskEmpty)
				{
					m_sem.wait();
				}
			}

			if (!_is_running)
			{
				break;
			}

			if (taskInfo.task)
				taskInfo.task(taskInfo.pArg); // do the task
		}

		printf("end work thread: %d\n", std::this_thread::get_id());
	}

}
