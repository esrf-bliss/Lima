#ifndef THREADUTILS_H
#define THREADUTILS_H

#include "Exceptions.h"
#include "AutoObj.h"
#include <pthread.h>

namespace lima
{

class Mutex;
class Cond;

class MutexAttr
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


class Mutex
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

class Cond
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


class Thread
{
 public:
	Thread();
	virtual ~Thread();

	virtual void start();
	virtual void abort();

	bool hasStarted();
	bool hasFinished();

 protected:
	virtual void threadFunction() = 0;
	
 private:
	static void *staticThreadFunction(void *data);

	pthread_t m_thread;
	bool m_started;
	bool m_finished;
};


class CmdThread
{
 public:
	enum { // Status
		InInit, Stopped, Finished, MaxThreadStatus,
	};

	enum { // Cmd
		None, Init, Stop, Abort, MaxThreadCmd,
	};

	CmdThread();
	virtual ~CmdThread();

	virtual void start();
	virtual void abort();

	void sendCmd(int cmd);
	int getStatus();
	void waitStatus(int status);
	int waitNotStatus(int status);
	
 protected:
	virtual void init() = 0;
	virtual void execCmd(int cmd) = 0;

	int waitNextCmd();
	void setStatus(int status);

	AutoMutex lock();
	AutoMutex tryLock();

 private:
	class AuxThread : public Thread
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

	AuxThread m_thread;
	Cond m_cond;
	int m_status;
	int m_cmd;
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
