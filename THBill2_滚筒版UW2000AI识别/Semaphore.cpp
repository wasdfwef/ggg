#include "stdafx.h"
#include "Semaphore.h"

namespace TOOLUTIL
{
	Semaphore::Semaphore(std::string name, int value /*= 0*/)
		:_name(std::move(name))
		, _count(value)
	{

	}

	Semaphore::~Semaphore()
	{
		
	}

	void Semaphore::wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (--_count < 0) { // 资源不够
			_condition.wait(lock); // 阻塞
		}
	}

	void Semaphore::wait(int ms)
	{
		assert(ms > 0);
		std::unique_lock<std::mutex> lock(_mutex);
		if (--_count < 0) {                            // 资源不够
			std::cv_status status
				= _condition.wait_for(lock, std::chrono::milliseconds(ms));
			if (status == std::cv_status::timeout)   // 超时
			{
				_count = 0;
				_condition.notify_all();//唤醒所有阻塞线程
			}
		}
	}

	void Semaphore::signal()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (++_count <= 0) {
			_condition.notify_one();
		}
	}

	void Semaphore::signalAll()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		while (++_count <= 0) {
			_condition.notify_one();
		}
		_count = 0;
	}

	std::string Semaphore::name()
	{
		return _name;
	}

	std::ostream & operator<<(std::ostream &out, const Semaphore &sem)
	{
		out << "Semaphore:" << sem._name.c_str() << ",Waiting threads count:" << sem._count << std::endl;
		return out;
	}

}