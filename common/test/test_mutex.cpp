#include "lima/ThreadUtils.h"
#include "lima/Debug.h"
#include "lima/Exceptions.h"

using namespace lima;

DEB_GLOBAL(DebModTest);

class TestThread : public Thread
{
	DEB_CLASS(DebModTest, "TestThread");
public:
	enum Type { Tester, Locker, Releaser };
  
	TestThread(Type type) : m_type(type), m_started(false)
	{
		DEB_CONSTRUCTOR();
		DEB_PARAM() << DEB_VAR1(type);
	}

	void start()
	{
		DEB_MEMBER_FUNCT();
		Thread::start();
		AutoMutex l(m_cond.mutex());
		while (!m_started)
			m_cond.wait();
		DEB_TRACE() << "Main: started";
	}

protected:
	void threadFunction()
	{
		DEB_MEMBER_FUNCT();

		CleanUp cleanup(*this);

		if (m_type == Tester)
			THROW_COM_ERROR(Error) << "Testing cleanup";

		if (m_type == Locker) {
			AutoMutex l(m_mutex);
			l.leaveLocked();
			DEB_TRACE() << "Leave locked";
		} else {
			AutoMutex l(m_mutex, AutoMutex::PrevLocked);
			DEB_TRACE() << "Previously unlocked";
		}
	}

private:
	class CleanUp : public ExceptionCleanUp
	{
	public:
		using ExceptionCleanUp::ExceptionCleanUp;
		virtual ~CleanUp() {
			static_cast<TestThread&>(m_thread).signalStarted();
		}
	};

	void signalStarted()
	{
		DEB_MEMBER_FUNCT();
		AutoMutex l(m_cond.mutex());
		m_started = true;
		m_cond.signal();
		DEB_TRACE() << "Thread: started";
	}
	
	static Mutex m_mutex;
	Type m_type;
	Cond m_cond;
	bool m_started;
};

Mutex TestThread::m_mutex;

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	DebParams::setTypeFlags(DebParams::AllFlags);
	
	DEB_TRACE() << "Starting test";

	TestThread tester(TestThread::Tester);
	tester.start();
	tester.join();

	TestThread locker(TestThread::Locker);
	TestThread releaser(TestThread::Releaser);
	locker.start();
	releaser.start();
	locker.join();
	releaser.join();

	return 0;
}
