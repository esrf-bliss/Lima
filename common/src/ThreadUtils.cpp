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
#include "ThreadUtils.h"
#include "Exceptions.h"
#include <errno.h>
#ifdef __unix
#include <sys/time.h>
#else
#include <time_compat.h>
#endif

using namespace lima;

MutexAttr::MutexAttr(Type type)
{
	if (pthread_mutexattr_init(&m_mutex_attr) != 0)
		throw LIMA_COM_EXC(Error, "Error initializing mutex attr");

	try {
		setType(type);
	} catch (...) {
		destroy();
		throw;
	}
}

MutexAttr::MutexAttr(const MutexAttr& mutex_attr)
	: m_mutex_attr(mutex_attr.m_mutex_attr)
{
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

	if (pthread_mutexattr_settype(&m_mutex_attr, kind) != 0)
		throw LIMA_COM_EXC(Error, "Error setting mutex attr");
}

MutexAttr::Type MutexAttr::getType() const
{
	int kind;
	if (pthread_mutexattr_gettype(&m_mutex_attr, &kind) != 0)
		throw LIMA_COM_EXC(Error, "Error getting mutex attr");

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

MutexAttr& MutexAttr::operator =(Type type)
{
	setType(type);
	return *this;
}

MutexAttr& MutexAttr::operator =(const MutexAttr& mutex_attr)
{
	m_mutex_attr = mutex_attr.m_mutex_attr;
	return *this;
}

void MutexAttr::destroy()
{
	pthread_mutexattr_destroy(&m_mutex_attr);
}


Mutex::Mutex(MutexAttr mutex_attr)
	: m_mutex_attr(mutex_attr)
{
	pthread_mutexattr_t& attr = m_mutex_attr.m_mutex_attr;
	if (pthread_mutex_init(&m_mutex, &attr) != 0)
		throw LIMA_COM_EXC(Error, "Error initializing mutex");
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
	if (pthread_mutex_lock(&m_mutex) != 0)
		throw LIMA_COM_EXC(Error, "Error locking mutex");
}

void Mutex::unlock()
{
	if (pthread_mutex_unlock(&m_mutex) != 0)
		throw LIMA_COM_EXC(Error, "Error unlocking mutex");
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

MutexAttr Mutex::getAttr()
{
	return m_mutex_attr;
}


Cond::Cond() : m_mutex(MutexAttr::Normal)
{
	if (pthread_cond_init(&m_cond, NULL) != 0)
		throw LIMA_COM_EXC(Error, "Error initializing condition");
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
      waitTimeout.tv_sec = now.tv_sec + long(timeout);
      waitTimeout.tv_nsec = (now.tv_usec * 1000) + 
	long((timeout - long(timeout)) * 1e9);
      if(waitTimeout.tv_nsec >= 1000000000L) // Carry
	++waitTimeout.tv_sec,waitTimeout.tv_nsec -= 1000000000L;
      retcode = pthread_cond_timedwait(&m_cond,&m_mutex.m_mutex,&waitTimeout);
    }
  else
      retcode = pthread_cond_wait(&m_cond, &m_mutex.m_mutex);
	
  if(retcode && retcode != ETIMEDOUT)
    throw LIMA_COM_EXC(Error, "Error waiting for condition");
  return !retcode;
}

void Cond::signal()
{
	if (pthread_cond_signal(&m_cond) != 0)
		throw LIMA_COM_EXC(Error, "Error signaling condition");
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
  if (pthread_cond_broadcast(&m_cond) != 0)
    throw LIMA_COM_EXC(Error, "Error broadcast condition");
}

Thread::Thread()
{
	m_started = m_finished = false;
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
	if (m_started)
		throw LIMA_COM_EXC(Error, "Thread already started");

	m_finished = false;
	if (pthread_create(&m_thread, &m_thread_attr, staticThreadFunction, this) != 0)
		throw LIMA_HW_EXC(Error, "Error creating thread");

	m_started = true;
}

void Thread::join()
{
	if (!m_started)
		throw LIMA_COM_EXC(Error, "Thread not started or joined");

	pthread_join(m_thread, NULL);
	m_started = false;
}

bool Thread::hasStarted()
{
	return m_started;
}

bool Thread::hasFinished()
{
	return m_finished;
}

void *Thread::staticThreadFunction(void *data)
{
	Thread *thread = (Thread *) data;

	try {
		thread->threadFunction();
	} catch (...) {

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
	m_cmd = None;
}

CmdThread::~CmdThread()
{
	if(m_thread.hasStarted())
	{
		abort();
		waitStatus(Finished);
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
  return m_cmd;
}

void CmdThread::setStatus(int status)
{
	AutoMutex l = lock();
	m_status = status;
	m_cond.signal();
}

void CmdThread::waitStatus(int status)
{
	AutoMutex l = lock();
	while (m_status != status)
		m_cond.wait();
}

int CmdThread::waitNotStatus(int status)
{
	AutoMutex l = lock();
	while (m_status == status)
		m_cond.wait();
	return m_status;
}

void CmdThread::sendCmd(int cmd)
{
	AutoMutex l = lock();
	m_cmd = cmd;
	m_cond.signal();
}
/** @brief send a command only if the return of if_test is true.
 *  
 *  function if_test get as argument the command and status
 */
void CmdThread::sendCmdIf(int cmd,bool (*if_test)(int,int))
{
  AutoMutex l = lock();
  bool sendFlag = true;
  if(if_test)
    sendFlag = if_test(m_cmd,m_status);
  if(sendFlag)
    {
      m_cmd = cmd;
      m_cond.signal();
    }
}
void CmdThread::start()
{
	if (m_thread.hasStarted())
		throw LIMA_COM_EXC(InvalidValue, "Thread already started");

	m_cmd = Init;
	m_thread.start();
}

void CmdThread::abort()
{
	if (getStatus() != Finished)
		sendCmd(Abort);
}

int CmdThread::waitNextCmd()
{
	AutoMutex l = lock();

	while (m_cmd == None)
	  m_cond.wait();

	int cmd = m_cmd;
	m_cmd = None;
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
		case Stop:
			setStatus(Stopped);
			break;
		case Abort:
			setStatus(Finished);
			break;
		default:
			execCmd(cmd);
		}
	}
}
