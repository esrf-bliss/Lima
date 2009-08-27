class MutexAttr
{
%TypeHeaderCode
#include <ThreadUtils.h>
using namespace lima;
%End
 public:
	enum Type {
		Normal, Recursive, ErrorCheck,
	};

	MutexAttr(Type type = Recursive);
	MutexAttr(const MutexAttr& mutex_attr);
	~MutexAttr();

	void setType(Type type);
	Type getType() const;
};


class Mutex
{
%TypeHeaderCode
#include <ThreadUtils.h>
using namespace lima;
%End
 public:
	Mutex(MutexAttr mutex_attr = MutexAttr::Recursive);
	~Mutex();

	void lock();
	void unlock();
	bool tryLock();

	MutexAttr getAttr();

	void __enter__();
%MethodCode
		sipCpp->lock();	
%End	

	void __exit__(SIP_PYOBJECT,SIP_PYOBJECT,SIP_PYOBJECT);
%MethodCode
		sipCpp->unlock();
%End
};


class Cond
{
%TypeHeaderCode
#include <ThreadUtils.h>
using namespace lima;
%End
 public:
	Cond();
	~Cond();
	
	void acquire();
	void release();
	Mutex& mutex();

	bool wait(double timeout = -1.);
	void signal();
	void broadcast();

	void __enter__();
%MethodCode
		sipCpp->acquire();	
%End	

	void __exit__(SIP_PYOBJECT,SIP_PYOBJECT,SIP_PYOBJECT);
%MethodCode
		sipCpp->release();
%End
};


