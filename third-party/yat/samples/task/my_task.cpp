/*!
 * \file     
 * \brief    An example of yat::Task (and related classes) usage. .
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "my_task.h"

// ============================================================================
// MyTask::MyTask
// ============================================================================
MyTask::MyTask (size_t _lo_wm, size_t _hi_wm)
  : yat::Task(Config(true,     //- enable timeout msg
                     1000,     //- every second (i.e. 1000 msecs)
                     false,    //- disable periodic msgs
                     0,        //- no exec period for periodic msgs (disabled)
                     false,    //- don't lock the internal mutex while handling a msg (recommended setting)
                     _lo_wm,   //- msgQ low watermark value
                     _hi_wm,   //- msgQ high watermark value
                     false,    //- do not throw exception on post msg timeout (msqQ saturated)
                     0)),      //- user data (same for all msgs) - we don't use it here
    ctrl_msg_counter (0),
    user_msg_counter (0)
#if defined (YAT_DEBUG)
    , last_msg_id (0)
    , lost_msg_counter (0)
    , wrong_order_msg_counter (0)
#endif
{
	YAT_TRACE("MyTask::MyTask");
}

// ======================================================================
// MyTask::~MyTask
// ======================================================================
MyTask::~MyTask (void)
{
	YAT_TRACE("MyTask::~MyTask");
	
#if defined (YAT_DEBUG)
  YAT_LOG("MyTask::statistics::ctrl msg:: " << this->ctrl_msg_counter);
  YAT_LOG("MyTask::statistics::user msg:: " << this->user_msg_counter);
  YAT_LOG("MyTask::statistics::lost msg:: " << this->lost_msg_counter);
  YAT_LOG("MyTask::statistics::wrong order msg:: " << this->wrong_order_msg_counter);
#endif
}

// ============================================================================
// MyTask::handle_message
// ============================================================================
void MyTask::handle_message (yat::Message& _msg)
	throw (yat::Exception)
{
	//- YAT_TRACE("MyTask::handle_message");

	//- handle msg
  switch (_msg.type())
	{
	  //- TASK_INIT ----------------------
	  case yat::TASK_INIT:
	    {
  	    //- "initialization" code goes here
  	    YAT_LOG("MyTask::handle_message::TASK_INIT::task is starting up");
        this->ctrl_msg_counter++;
      } 
		  break;
		//- TASK_EXIT ----------------------
		case yat::TASK_EXIT:
		  {
  			//- "release" code goes here
  			YAT_LOG("MyTask::handle_message::TASK_EXIT::task is quitting");
        this->ctrl_msg_counter++;
      }
			break;
		//- TASK_PERIODIC ------------------
		case yat::TASK_PERIODIC:
		  {
  		  //- code relative to the task's periodic job goes here
  		  YAT_LOG("MyTask::handle_message::handling TASK_PERIODIC msg");
      }
		  break;
		//- TASK_TIMEOUT -------------------
		case yat::TASK_TIMEOUT:
		  {
  		  //- code relative to the task's tmo handling goes here
  		  YAT_LOG("MyTask::handle_message::handling TASK_TIMEOUT msg");
      }
		  break;
		//- USER_DEFINED_MSG -----------------
		case kDUMMY_MSG:
		  {
  		  //- YAT_LOG("MyTask::handle_message::handling kDUMMY_MSG user msg");
        this->user_msg_counter++;
#if defined (YAT_DEBUG)
        if (_msg.id() < last_msg_id)
          this->wrong_order_msg_counter++;
        else
          this->lost_msg_counter += _msg.id() - (this->last_msg_id + 1);
#endif
        //- simulate some time consuming activity
        yat::ThreadingUtilities::sleep(0, 10000);
  		}
  		break;
  	default:
  		YAT_LOG("MyTask::handle_message::unhandled msg type received");
  		break;
	}
#if defined (YAT_DEBUG)
  this->last_msg_id = _msg.id();
#endif
}

