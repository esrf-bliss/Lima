/*!
 * \file     
 * \brief    An example of yat::Callback usage. .
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <yat/Callback.h>

/*
 * The member and the static member of this class will be registered as callbacks
 */
class CallbackTarget
{
public:
  CallbackTarget(std::string _name)
    : name(_name)
  {}

  ~CallbackTarget()
  {}

  virtual void what_s_your_name_VIRTUAL_MEMBER(std::string my_name)
  {
    //- we are in a member function : we can use the 'this' pointer.
    std::cout << "hello "
              << my_name
              << " ! it's "
              << this->name
              << " and you are in the default impl. of the virtual function"
              << std::endl;
  };

  void what_s_your_name_MEMBER(std::string my_name)
  {
    //- we are in a member function : we can use the 'this' pointer.
    std::cout << "hello "
              << my_name
              << " ! it's "
              << this->name
              << std::endl;
  }


  static void what_s_your_name_STATIC_MEMBER(std::string my_name)
  {
    //- we are in a static member function : we can NOT use the 'this' pointer.
    std::cout << "hello "
              << my_name
              << " ! "
              << "i'm just a poor static member : i don't have a name..."
              << std::endl;
  }

protected:
  std::string name;
};

/*
 * The virtual member of this class will be registered as a callback
 */
class CallbackTargetDerived : public CallbackTarget
{
public:
  CallbackTargetDerived(std::string _name)
    : CallbackTarget(_name)
  {}
  
  //- override the virtual member function 
  virtual void what_s_your_name_VIRTUAL_MEMBER(std::string my_name)
  {
    std::cout << "hello "
              << my_name
              << " ! it's "
              << this->name
              << " and you are in the derived class"
              << std::endl;
  }
};

// this C function will be registered as a callback
void what_s_your_name_FUNCTION(std::string my_name)
{
   //- we are in a simple function : we can NOT access any other data but the parameter
   std::cout << "hello "
              << my_name
              << " ! "
              << "i'm just a poor simple function : i don't have a name..."
              << std::endl;
};

/*
 * The following macro will define a class named MyCBObjectType
 * which will represent a callback object taking a 'std::string'
 * as parameter (and returning 'void', but this cannot be customized).
 *
 * Use :
 *   - 'MyCBObjectType::instantiate( function_ptr )' to create a callback
 *       associated to a function pointer taking a 'std::string' and returning void
 *       (can be either a C function or a static member function)
 *   - 'MyCBObjectType::instantiate( object_ref , member_of object )' to create 
 *       a callback associated to a member function of the class of object, which 
 *       will be called on the object_ref instance
 *       (can be a standard member or a virtual member)
 */
YAT_DEFINE_CALLBACK( MyCBObjectType, std::string );

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  //- instantiate 2 different object of the CallbackTarget class
  //- that we can differentiate
  CallbackTarget target1("pee wee ellis");
  CallbackTarget target2("fred wesley");

  //- instantiate a CallbackTargetDerived
  CallbackTargetDerived target3("larry goldings");


  //- we will use only references to 'CallbackTarget' to illustrate
  //- the polymorphism possibilities
  CallbackTarget& target1_ref = target1;
  CallbackTarget& target2_ref = target2;
  CallbackTarget& target3_ref = target3; //- the virtual function mechanism will be used here


  //- this will be the parameter passed to every callback objects
  std::string funkyman ("maceo parker");


  //- create a callback object on a member function of the 'target1' object
  MyCBObjectType callback1 = MyCBObjectType::instanciate(
                              target1_ref,
                              &CallbackTarget::what_s_your_name_MEMBER );

  //- create a callback object on a member function of the 'target2' object
  MyCBObjectType callback2 = MyCBObjectType::instanciate( 
                              target2_ref,
                              &CallbackTarget::what_s_your_name_MEMBER );

  //- create a callback object on a static member function of the 'CallbackTarget' class
  MyCBObjectType callback3 = MyCBObjectType::instanciate( 
                              &CallbackTarget::what_s_your_name_STATIC_MEMBER );

  //- create a callback object on a simple function
  MyCBObjectType callback4 = MyCBObjectType::instanciate( &what_s_your_name_FUNCTION );

  //- create a callback object on the virtual member of the 'target1' object
  //- --> we will get the default implementation
  MyCBObjectType callback5 = MyCBObjectType::instanciate( 
                              target1_ref,
                              &CallbackTarget::what_s_your_name_VIRTUAL_MEMBER );

  //- create a callback object on the virtual member of the 'target3' object
  //- --> we will get the implementation of the derived class
  MyCBObjectType callback6 = MyCBObjectType::instanciate( 
                              target3_ref,
                              &CallbackTarget::what_s_your_name_VIRTUAL_MEMBER );

  //- fire the different callbacks
  callback1(funkyman);
  std::cout << std::endl;

  callback2(funkyman);
  std::cout << std::endl;

  callback3(funkyman);
  std::cout << std::endl;

  callback4(funkyman);
  std::cout << std::endl;

  callback5(funkyman);
  std::cout << std::endl;

  callback6(funkyman);
  std::cout << std::endl;

  return 0;
}
