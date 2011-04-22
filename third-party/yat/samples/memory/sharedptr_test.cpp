/*!
 * \file     
 * \brief    An example of yat::SharedPtr usage
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <yat/memory/SharedPtr.h>
#include <yat/threading/SharedObject.h>


//-----------------------------------------------------------------------------
// MySharedObject
//-----------------------------------------------------------------------------
struct MySharedObject : public yat::SharedObject
{
  MySharedObject( std::string s )
    : some_attribute(s)
  {
  	std::cout << "MySharedObject::calling ctor for " << some_attribute << std::endl;
  }

  ~MySharedObject()
  {
    std::cout << "MySharedObject::calling dtor for " << some_attribute << std::endl;
  }

  //- reimplements yat::yat::SharedObject::duplicate so that it returns a <MySharedObject*> 
  //- instead of a <yat::SharedObject*>...
  MySharedObject* duplicate()
  {
    return reinterpret_cast< MySharedObject* >(yat::SharedObject::duplicate());
  }

  std::string some_attribute;
};

//-----------------------------------------------------------------------------
// DUMP MACRO
//-----------------------------------------------------------------------------
#define DUMP( ptr ) \
  std::cout << "sharedptr " \
  					<< #ptr \
            << " -- points to sharedobject --> " \
            << (ptr ? ptr->some_attribute : "xxxxxx") \
            << " [which ref. count is " \
            << (ptr ? ptr->reference_count() : 0) \
            << "]" \
            << std::endl
            
//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  typedef yat::SharedPtr<MySharedObject> MySharedObjectPtr;

  MySharedObjectPtr foo ( new MySharedObject("foo-so") );
 
  MySharedObjectPtr bar ( new MySharedObject("bar-so") );

  MySharedObjectPtr tmp;

  std::cout << std::endl; //-----------------------------------------------------
  
  std::cout << "initial state:" << std::endl;
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = foo;
  std::cout << "after 'tmp = foo' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = bar;
  
  std::cout << "after 'tmp = bar' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp.reset();
  
  std::cout << "after 'tmp.reset()' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl; //-----------------------------------------------------
  
  tmp = foo; 
  
  foo.reset( new MySharedObject("oof-so") );
  
  std::cout << "after 'tmp = foo; foo.reset( new MySharedObject(\"oof-so\") )' :" << std::endl;
  
  DUMP( tmp );
  DUMP( foo );
  DUMP( bar );

  std::cout << std::endl;
  
  return 0;
}
