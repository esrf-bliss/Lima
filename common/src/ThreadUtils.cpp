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
#include "lima/ThreadUtils.h"
#include "lima/Exceptions.h"
#include <errno.h>
#include <iomanip>
#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#else
#include <processlib/win/unistd.h>
#include <processlib/win/time_compat.h>
#endif

#if !defined(_WIN32)
#include <sys/syscall.h>
#endif

using namespace lima;

inline void check_error(int ret, const char *desc)
{
	if (ret == 0)
		return;
	std::ostringstream os;
	os << desc << ": " << strerror(ret) << " (" << ret << ")";
	throw LIMA_COM_EXC(Error, os.str());
}


MutexAttr::MutexAttr(Type type)
{
	int ret = pthread_mutexattr_init(&m_mutex_attr);
	check_error(ret, "Error initializing mutex attr");

	try {
		setType(type);
	} catch (...) {
		destroy();
		throw;
	}
}

MutexAttr::~MutexAttr()
{
	destroy();
}

void MutexAttr::setType(Type type)
{
	int kind;
	switch (type) {
	case Normal: 
		kind = PTHREAD_MUTEX_NORMAL;
		break;
	case Recursive:
		kind = PTHREAD_MUTEX_RECURSIVE;
		break;
	case ErrorCheck:
		kind = PTHREAD_MUTEX_ERRORCHECK;
		break;
	default:
		throw LIMA_COM_EXC(InvalidValue, "Invalid MutexAttr type");
	}

	int ret = pthread_mutexattr_settype(&m_mutex_attr, kind);
	check_error(ret, "Error setting mutex attr");
}

MutexAttr::Type MutexAttr::getType() const
{
	int kind;
	int ret = pthread_mutexattr_gettype(&m_mutex_attr, &kind);
	check_error(ret, "Error getting mutex attr");

	switch (kind) {
	case PTHREAD_MUTEX_NORMAL:
		return Normal;
	case PTHREAD_MUTEX_RECURSIVE:
		return Recursive;
	case PTHREAD_MUTEX_ERRORCHECK:
		return ErrorCheck;
	default:
		throw LIMA_COM_EXC(Error, "Invalid mutex attr kind");
	}
}

void MutexAttr::destroy()
{
	pthread_mutexattr_destroy(&m_mutex_attr);
}


Mutex::Mutex(MutexAttr::Type type)
{
	MutexAttr attr(type);
	// After a mutex attributes object has been used to initialize one or more mutexes,
	// any function affecting the attributes object (including destruction)
	// shall not affect any previously initialized mutexes.
	int ret = pthread_mutex_init(&m_mutex, &attr.m_mutex_attr);
	check_error(ret, "Error initializing mutex");
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
	int ret = pthread_mutex_lock(&m_mutex);
	check_error(ret, "Error locking mutex");
}

void Mutex::unlock()
{
	int ret = pthread_mutex_unlock(&m_mutex);
	check_error(ret, "Error unlocking mutex");
}

bool Mutex::tryLock()
{
	
	switch (pthread_mutex_trylock(&m_mutex)) {
	case EBUSY:
		return false;
	case 0:
		return true;
	default:
		throw LIMA_COM_EXC(Error, "Error trying to lock mutex");
	}
}

//MutexAttr Mutex::getAttr()
//{
//	return m_mutex_attr;
//}


Cond::Cond() : m_mutex(MutexAttr::Normal)
{
	int ret = pthread_cond_init(&m_cond, NULL);
	check_error(ret, "Error initializing condition");
}

Cond::~Cond()
{
	pthread_cond_destroy(&m_cond);
}

/** @brief wait on cond variable
 *  @return 
 *  - true if ok 
 *  - false if timeout
 */
bool Cond::wait(double timeout)
{
  int retcode = 0;
  if(timeout >= 0.)
    {
      struct timeval now;
      struct timespec waitTimeout;
      gettimeofday(&now,NULL);
      waitTimeout.tv_sec = now.tv_sec + time_t(timeout);
      waitTimeout.tv_nsec = (now.tv_usec * 1000) + 
	long((timeout - long(timeout)) * 1e9);
      if(waitTimeout.tv_nsec >= 1000000000L) // Carry
	++waitTimeout.tv_sec,waitTimeout.tv_nsec -= 1000000000L;
      retcode = pthread_cond_timedwait(&m_cond,&m_mutex.m_mutex,&waitTimeout);
    }
  else
      retcode = pthread_cond_wait(&m_cond, &m_mutex.m_mutex);
	
  if(retcode != ETIMEDOUT)
    check_error(retcode, "Error waiting for condition");
  return !retcode;
}

void Cond::signal()
{
	int ret = pthread_cond_signal(&m_cond);
	check_error(ret, "Error signaling condition");
}

void Cond::acquire()
{
	m_mutex.lock();
}

void Cond::release()
{
	m_mutex.unlock();
}

void Cond::broadcast()
{
	int ret = pthread_cond_broadcast(&m_cond);
	check_error(ret, "Error broadcast condition");
}

pid_t lima::GetThreadID() {

#if defined(_WIN32)
	return GetCurrentThreadId();
#else
	return syscall(SYS_gettid);
#endif
	
}

Thread::ExceptionCleanUp::ExceptionCleanUp(Thread& thread)
	: m_thread(thread)
{
}

Thread::ExceptionCleanUp::~ExceptionCleanUp()
{
	m_thread.m_exception_handled = true;
}

Thread::Thread()
	: m_started(false), m_finished(false),
	  m_exception_handled(false), m_tid(0)
{
	pthread_attr_init(&m_thread_attr);
}

Thread::~Thread()
{
	if (m_started)
		join();
	pthread_attr_destroy(&m_thread_attr);
}

void Thread::start()
{
	if (m_started) {
		if (!m_finished)
			throw LIMA_COM_EXC(Error, "Thread already started");
		join();
	}

	m_finished = false;
	int ret = pthread_create(&m_thread, &m_thread_attr,
				 staticThreadFunction, this);
	check_error(ret, "Error creating thread");

	m_started = true;
}

void Thread::join()
{
	if (!m_started)
		throw LIMA_COM_EXC(Error, "Thread not started or joined");

	pthread_join(m_thread, NULL);
	m_started = false;
	m_tid = 0;
}

bool Thread::hasStarted()
{
	return m_started;
}

bool Thread::hasFinished()
{
	return m_finished;
}

pid_t Thread::getThreadID()
{
	return m_tid;
}

void *Thread::staticThreadFunction(void *data)
{
	using namespace std;

	Thread *thread = (Thread *) data;
	thread->m_tid = GetThreadID();

	try {
		thread->threadFunction();
	} catch (...) {
		if (!thread->m_exception_handled) {
			ostream& os = cerr;
#ifdef __unix			
			long long thread_id = (long long) pthread_self();
#else
			pthread_t tid =  pthread_self();
			long long thread_id = (long long)tid.p;
#endif
			std::streamsize w = os.width();
			os << "***** Thread " 
			   << setw(8) << hex << thread_id << setw(w) << dec
			   << " function exited due to an exception "
			   << "without clean-up! *****" << endl;
		}
	}
	
	thread->m_finished = true;
	return NULL;
}

CmdThread::AuxThread::AuxThread(CmdThread& master)
	: m_master(&master)
{
}

CmdThread::AuxThread::~AuxThread()
{
}

void CmdThread::AuxThread::threadFunction()
{
	m_master->cmdLoop();
}

CmdThread::CmdThread()
	: m_thread(*this)
{
	m_status = InInit;
	m_status_history.set(InInit);
}

CmdThread::~CmdThread()
{
	using namespace std;

	if (!m_thread.hasStarted())
		return;

	if (getStatus() != Finished) {
		cerr << "***** Error: CmdThread did not call abort "
		     << "in the derived class destructor! *****";
		abort();
	}
}

AutoMutex CmdThread::lock() const
{
	return AutoMutex(m_cond.mutex(), AutoMutex::Locked);
}

AutoMutex CmdThread::tryLock() const
{
	return AutoMutex(m_cond.mutex(), AutoMutex::TryLocked);
}

int CmdThread::getStatus() const
{
	AutoMutex l = lock();
	return m_status;
}

int CmdThread::getNextCmd() const
{
	AutoMutex l = lock();
	return m_cmd.empty() ? None : m_cmd.front();
}

void CmdThread::setStatus(int status)
{
	AutoMutex l = lock();
	m_status = status;
	m_status_history.set(status);
	m_cond.signal();
}

void CmdThread::waitStatus(int status)
{
	AutoMutex l = lock();
	while (!m_status_history.test(status))
		m_cond.wait();
}

int CmdThread::waitNotStatus(int status)
{
	std::bitset<16> mask(0xffff);
	mask.reset(status);

	AutoMutex l = lock();
	while ((m_status_history & mask).none())
		m_cond.wait();
	return m_status;
}

void CmdThread::sendCmd(int cmd)
{
	AutoMutex l = lock();
	doSendCmd(cmd);
}

/** @brief send a command only if the return of if_test is true.
 *  
 *  function if_test get as argument the command and status
 */
void CmdThread::sendCmdIf(int cmd, bool (*if_test)(int,int))
{
	AutoMutex l = lock();

	if (if_test && if_test(cmd, m_status))
		doSendCmd(cmd);
}

void CmdThread::doSendCmd(int cmd)
{
	if (m_status == Finished)
		throw LIMA_COM_EXC(Error, "Thread has Finished");

	// Assume that we will have a call to waitStatus somewhere after the new command
	m_status_history.reset();

	m_cmd.push(cmd);
	m_cond.signal();
}

void CmdThread::start()
{
	if (m_thread.hasStarted())
		throw LIMA_COM_EXC(InvalidValue, "Thread already started");

	m_cmd.push(Init);
	m_thread.start();
}

void CmdThread::abort()
{
	if (getStatus() == Finished)
		return;

	sendCmd(Abort);
	waitStatus(Finished);
}

int CmdThread::waitNextCmd()
{
	AutoMutex l = lock();

	while (m_cmd.empty())
		m_cond.wait();

	int cmd = m_cmd.front();
	m_cmd.pop();
	return cmd;
}

void CmdThread::cmdLoop()
{
	while (getStatus() != Finished) {
		int cmd = waitNextCmd();
		switch (cmd) {
		case None:
			throw LIMA_COM_EXC(InvalidValue, "Invalid None cmd");
		case Init:
			init();
			break;
		case Abort:
			setStatus(Finished);
			break;
		default:
			execCmd(cmd);
		}
	}
}
