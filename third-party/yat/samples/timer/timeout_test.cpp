/*!
 * \file     
 * \brief    An example of yat::Timeout usage
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <math.h>
#include <yat/Timer.h>
#include <yat/threading/Thread.h> 
                
//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  //- instanciate a disabled tmo
  yat::Timeout tmo(500, yat::Timeout::TMO_UNIT_MSEC, false);
  
  //- check that time to expiration is <INFINITE> for a disabled tmo
  yat::Timeout::TimeoutValue v = tmo.time_to_expiration();
  std::cout << "tmo will expire in " << tmo.time_to_expiration() << " msecs" << std::endl;
  
  //- enable the tmo (do not restart the underlying timer)
  tmo.enable(false);
  
  std::cout << "tmo expired? " <<  tmo.expired() << std::endl;
  
  while (tmo.time_to_expiration() > 0.)
  {
  	std::cout << "tmo will expire in " << tmo.time_to_expiration() << " msecs" << std::endl;
    yat::Thread::sleep(100);
  }
  
  std::cout << "tmo expired? " <<  tmo.expired() << std::endl;
  
  std::cout << "tmo expired " << ::fabs(tmo.time_to_expiration()) << " msecs ago" << std::endl;
  
  yat::Thread::sleep(100);
  
  std::cout << "tmo expired " << ::fabs(tmo.time_to_expiration()) << " msecs ago" << std::endl;
   
  //- restart/(re)enable the tmo (silently restart the underlying timer)
  tmo.restart();
  
  while (! tmo.expired())
  {
  	std::cout << "tmo will expire in " << tmo.time_to_expiration() << " msecs" << std::endl;
    yat::Thread::sleep(100);
  }
  
  yat::Thread::sleep(100);
  
  std::cout << "tmo expired " << ::fabs(tmo.time_to_expiration()) << " msecs ago" << std::endl;
   
  return 0;
}
