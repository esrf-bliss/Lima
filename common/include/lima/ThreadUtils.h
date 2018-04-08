//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef THREADUTILS_H
#define THREADUTILS_H

#include "lima/LimaCompatibility.h"
#include "lima/AutoObj.h"
#include <pthread.h>
#include <bitset>
#include <queue>

namespace lima
{

class Mutex;
class Cond;

class LIMACORE_API MutexAttr
{
 public:
	enum Type {
		Normal, Recursive, ErrorCheck,
	};

	MutexAttr(Type type = Recursive);
	MutexAttr(const MutexAttr& mutex_attr);
	~MutexAttr();

	void setType(Type type);
	Type getType() const;

	MutexAttr& operator =(Type type);
	MutexAttr& operator =(const MutexAttr& mutex_attr);

 private:
	void destroy();
	friend class Mutex;

	pthread_mutexattr_t m_mutex_attr;
};


class LIMACORE_API Mutex
{
 public:
	Mutex(MutexAttr mutex_attr = MutexAttr::Recursive);
	~Mutex();

	void lock();
	void unlock();
	bool tryLock();

	MutexAttr getAttr();

 private:
	friend class Cond;

	MutexAttr m_mutex_attr;
	pthread_mutex_t m_mutex;
};


typedef AutoLock<Mutex> AutoMutex;
typedef AutoUnlock<Mutex> AutoMutexUnlock;

class LIMACORE_API Cond
{
 public:
	Cond();
	~Cond();

	void acquire();
	void release();
	Mutex& mutex() {return m_mutex;}

	bool wait(double timeout = -1.);
	void signal();
	void broadcast();
 private:
	pthread_cond_t m_cond;
	Mutex	       m_mutex;
};


pid_t GetThreadID();


class LIMACORE_API Thread
{
 public:
	Thread();
	virtual ~Thread();

	virtual void start();
	void join();

	bool hasStarted();
	bool hasFinished();

	pid_t getThreadID();

 protected:
	class LIMACORE_API ExceptionCleanUp
	{
	public:
		ExceptionCleanUp(Thread& thread);
		virtual ~ExceptionCleanUp();
	protected:
		Thread& m_thread;
	};

	virtual void threadFunction() = 0;

	pthread_attr_t	m_thread_attr;
	pthread_t m_thread;
	pid_t m_tid;

 private:
	friend class ExceptionCleanUp;

	static void *staticThreadFunction(void *data);

	bool m_started;
	bool m_finished;
	bool m_exception_handled;
};


class LIMACORE_API CmdThread
{
 public:
	enum { // Status
		InInit = 0, Finished, MaxThreadStatus,
	};

	enum { // Cmd
		None, Init, Abort, MaxThreadCmd,
	};

	CmdThread();
	virtual ~CmdThread();

	virtual void start();
	virtual void abort();

	void sendCmd(int cmd);
	void sendCmdIf(int cmd, bool (*if_test)(int, int));
	int getStatus() const;
	int getNextCmd() const;
	void waitStatus(int status);
	int waitNotStatus(int status);

 protected:
	virtual void init() = 0;
	virtual void execCmd(int cmd) = 0;

	int waitNextCmd();
	void setStatus(int status);

	AutoMutex lock() const;
	AutoMutex tryLock() const;

 private:
	class LIMACORE_API AuxThread : public Thread
	{
	public:
		AuxThread(CmdThread& master);
		virtual ~AuxThread();
	protected:
		void threadFunction();
		CmdThread *m_master;
	};
	friend class AuxThread;
	void cmdLoop();
	void doSendCmd(int cmd);

	int m_status;

	//No need to have dll-interface for private variables
#pragma warning( push )  
#pragma warning( disable : 4251 ) 
	std::bitset<16> m_status_history;
	std::queue<int> m_cmd;
#pragma warning( pop ) 

	mutable Cond m_cond;
	AuxThread m_thread;
};

#define EXEC_ONCE(statement)						\
	do {								\
		class RunOnce						\
		{							\
		public:							\
			static void run()				\
			{						\
				statement;				\
			}						\
		};							\
		static pthread_once_t init_once = PTHREAD_ONCE_INIT;	\
		pthread_once(&init_once, &RunOnce::run);		\
	} while (0)


} // namespace lima

#endif // THREADUTILS_H
