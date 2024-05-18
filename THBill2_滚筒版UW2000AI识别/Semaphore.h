#ifndef __TOOLUTIL_SEMAPHORE_H__
#define __TOOLUTIL_SEMAPHORE_H__

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <assert.h>

namespace TOOLUTIL
{
	class Semaphore 
	{
	private:
		long _count;							//等待线程数量
		std::mutex _mutex;                      //互斥锁
		std::condition_variable _condition;     //条件变量
		std::string _name;                      //信号量名字

	public:
		explicit Semaphore(std::string name, int value = 0);
		~Semaphore();

		/**
		* 相当于信号量机制里面的P操作.
		* _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
		*/
		void wait();

		/**
		* 相当于信号量机制里面的P操作.
		* _count大于0(有资源)时,函数会立即返回,否则会阻塞调用此函数的线程.
		* 但如果等待时间超过seconds指定的值，会唤醒所有阻塞线程.
		* @param ms 等待时间(ms)
		*/
		void wait(int ms);

		/**
		* 如果有阻塞的线程,则随机唤醒一个线程，相当于信号量机制里面的V操作.否则
		* 立即返回.
		*/
		void signal();

		/**
		* 如果有线程阻塞,唤醒阻塞的所有线程;否则立即返回.
		*/
		void signalAll();

		/**
		* 返回这个信号量的名字
		* @return 名字
		*/
		std::string name();

		/**
		* 重载<<,输出信号量的信息
		* @param out 输出流
		* @param sem 信号量
		* @return 输出流
		*/
		friend std::ostream & operator<<(std::ostream & out, const Semaphore & sem);
	};

	std::ostream & operator<<(std::ostream & out, const Semaphore & sem);

}


#endif //__TOOLUTIL_SEMAPHORE_H__
