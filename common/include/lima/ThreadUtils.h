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

#if defined(_WIN32)
#include <WinBase.h>
#define pid_t DWORD
#endif

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
	~MutexAttr();

	void setType(Type type);
	Type getType() const;

	// Non-copiable
	MutexAttr(const MutexAttr& rhs) = delete;
	MutexAttr& operator =(const MutexAttr& rhs) = delete;

 private:
	void destroy();
	friend class Mutex;

	pthread_mutexattr_t m_mutex_attr;
};


class LIMACORE_API Mutex
{
 public:
	Mutex(MutexAttr::Type type = MutexAttr::Recursive);
	~Mutex();

	void lock();
	void unlock();
	bool tryLock();

	// Non-copiable
	Mutex(const Mutex& rhs) = delete;
	Mutex& operator =(const Mutex& rhs) = delete;

 private:
	friend class Cond;

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

	// Non-copiable
	Cond(const Cond& rhs) = delete;
	Cond& operator =(const Cond& rhs) = delete;

 private:
	pthread_cond_t m_cond;
	Mutex	       m_mutex;
};


class LIMACORE_API ReadWriteLock
{
 public:
	class ReadGuard
	{
	public:
		ReadGuard(ReadWriteLock& lock) : m_lock(lock)
		{ m_lock._waitAndGet(ReadLock); }

		ReadGuard(const ReadGuard& o) = delete;
		ReadGuard& operator =(const ReadGuard& o) = delete;

		~ReadGuard()
		{ m_lock._release(); }

	private:
		ReadWriteLock& m_lock;
	};

	class WriteGuard
	{
	public:
		WriteGuard(ReadWriteLock& lock) : m_lock(lock)
		{ m_lock._waitAndGet(WriteLock); }

		WriteGuard(const WriteGuard& o) = delete;
		WriteGuard& operator =(const WriteGuard& o) = delete;

		~WriteGuard()
		{ m_lock._release(); }

	private:
		ReadWriteLock& m_lock;
	};

	ReadWriteLock() : m_usage(Free), m_readers(0)
	{}

 private:
	friend class ReadGuard;
	friend class WriteGuard;

	enum Usage { Free, ReadLock, WriteLock };

	void _waitAndGet(Usage requested_usage)
	{
		const bool req_write_lock = (requested_usage == WriteLock);
		
		AutoMutex l(m_cond.mutex());
		// While the lock is taken for writing or the lock is taken
		// for reading and we want to take it for writing
		while ((m_usage == WriteLock)
		       || ((m_usage == ReadLock) && req_write_lock))
			// Wait for availability
			m_cond.wait();
		m_usage = requested_usage;
		if (m_usage == ReadLock)
			++m_readers;
	}

	void _release()
	{
		AutoMutex l(m_cond.mutex());
		if ((m_usage == WriteLock) || (--m_readers == 0)) {
			m_usage = Free;
			m_cond.broadcast();
		}
	}

	Cond m_cond;
	Usage m_usage;
	int m_readers;
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

	// Non-copiable
	Thread(const Thread& rhs) = delete;
	Thread& operator =(const Thread& rhs) = delete;

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

	bool isJoinable();

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
#if defined(_WIN32)
#pragma warning( push )  
#pragma warning( disable : 4251 )
#endif 
	std::bitset<16> m_status_history;
	std::queue<int> m_cmd;
#if defined(_WIN32)
#pragma warning( pop ) 
#endif
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
