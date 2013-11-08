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
#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>

#ifdef HAS_INOTIFY
#include <sys/inotify.h>
#endif

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "DirectoryEventUtils.h"
#include "Event.h"

using namespace lima;
//Base class event
class DirectoryEvent::_Event
{
  DEB_CLASS_NAMESPC(DebModCommon,"DirectoryEvent::_Event","Common");
public:
  _Event(DirectoryEvent::Callback &cbk) :
    m_quit(false),
    m_wait(true),
    m_thread_running(true),
    m_thread_id(pthread_t(-1)),
    m_cbk(cbk)
  {}
  virtual ~_Event() {}
  void prepare(const DirectoryEvent::Parameters &parameters);
  void start();
  void stop();
  bool isStopped() const;
  int getNextFileNumberExpected() const;
protected:
  virtual void _signal() {};

  mutable Cond	       		m_cond;
  bool 				m_quit;
  bool 				m_wait;
  bool 				m_thread_running;
  pthread_t 			m_thread_id;
  DirectoryEvent::Parameters 	m_current_parameters;
  DirectoryEvent::Callback&	m_cbk;
  mutable char			m_scanf_format[256];
private:
  void _check_parameters(const DirectoryEvent::Parameters&) const;
};

void DirectoryEvent::_Event::prepare(const DirectoryEvent::Parameters &parameters)
{
  AutoMutex aLock(m_cond.mutex());
  m_wait = true;
  _signal();

  while(m_thread_running)
    m_cond.wait();
  aLock.unlock();

  _check_parameters(parameters);

  m_cbk.prepare(parameters);

  aLock.lock();
  m_current_parameters = parameters;
}

void DirectoryEvent::_Event::start()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  if(!m_wait || m_thread_running)
    THROW_HW_ERROR(Error) << "You should call prepare first";

  m_wait = false;
  m_cond.signal();
}

void DirectoryEvent::_Event::stop()
{
  AutoMutex aLock(m_cond.mutex());
  m_wait = true;
  _signal();

}

bool DirectoryEvent::_Event::isStopped() const
{
  AutoMutex aLock(m_cond.mutex());
  return m_wait;
}

int DirectoryEvent::_Event::getNextFileNumberExpected() const
{
  AutoMutex aLock(m_cond.mutex());
  return m_current_parameters.next_file_number_expected;
}

void DirectoryEvent::_Event::_check_parameters(const DirectoryEvent::Parameters &parameters) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(parameters);
  
  std::string output;
  if(!access(parameters.watch_path.c_str(),F_OK))
    {
      struct stat aDirectoryStat;
      if(stat(parameters.watch_path.c_str(),&aDirectoryStat))
	{
	  output = "Can stat directory : " + parameters.watch_path;
	  THROW_HW_ERROR(Error) << output;
	}
      if(!S_ISDIR(aDirectoryStat.st_mode))
	{
	  output = "Path : " + parameters.watch_path + " is not a directory";
	  THROW_HW_ERROR(Error) << output;
	}
    }
  else
    {
      output = "Path : " + parameters.watch_path + " doesn't exist";
      THROW_HW_ERROR(Error) << output;
    }
  
  //check if there is at one %d in parameters.file_pattern
  const char *patternPt = parameters.file_pattern.c_str();
  bool percentMatch = false;
  int intConvertion = 0;
  bool not_authorized = false;
  char* scanfFormat = m_scanf_format;
  for(;*patternPt != '\0' && !not_authorized;++patternPt)
    {
      switch(*patternPt)
	{
	case '%': 
	  percentMatch = !percentMatch;break;
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	case 'i':
	case 'd':
	  if(percentMatch) ++intConvertion,percentMatch = false;
	  break;
	case '.':
	  if(percentMatch) continue;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  break;
	default:
	  if(percentMatch)
	    not_authorized = true;
	  break;
	}
      *scanfFormat = *patternPt;
      ++scanfFormat;
    }
  *scanfFormat = *patternPt;
  if(intConvertion != 1 || not_authorized)
    THROW_HW_ERROR(Error) << "file_pattern should only have one int conversion like '%d'";
}

#ifdef HAS_INOTIFY
/** @brief this local file event use inotify so it can only be use an local pc or NFS server
 */
class _LocalDirectoryEvent : public DirectoryEvent::_Event
{
  DEB_CLASS_NAMESPC(DebModCommon,"_LocalDirectoryEvent","Common");
public:
  _LocalDirectoryEvent(DirectoryEvent::Callback &cbk);
  virtual ~_LocalDirectoryEvent();

  void watch_moved_to() {m_watch_mask = IN_MOVED_TO;}
  void watch_close_write() {m_watch_mask = IN_CLOSE_WRITE;}
private:
  static void* _runFunc(void *arg)
  {
    ((_LocalDirectoryEvent*)arg)->_run();
    return NULL;
  }
  void _run();
  void _clean();
  void _startWatch();
  void _stopWatch();
  virtual void _signal();

  int m_inotify_fd;
  int m_inotify_wd;
  int m_pipes[2];
  int m_watch_mask;

  static const size_t EVENT_SIZE = sizeof(struct inotify_event);
  static const size_t EVENT_BUF_LEN = (1024 * (EVENT_SIZE + 16));
};

_LocalDirectoryEvent::_LocalDirectoryEvent(DirectoryEvent::Callback &cbk) :
  DirectoryEvent::_Event(cbk),
  m_inotify_fd(-1),
  m_inotify_wd(-1),
  m_watch_mask(IN_CLOSE_WRITE)
{
  DEB_CONSTRUCTOR();
  
  if(pipe(m_pipes))
    THROW_HW_ERROR(Error) << "Can't open pipe";
  _signal();

  m_inotify_fd = inotify_init();
  if(m_inotify_fd < 0)
    {
      _clean();
      THROW_HW_ERROR(Error) << "Can't init inotify";
    }
  if(pthread_create(&m_thread_id,NULL,_LocalDirectoryEvent::_runFunc,this))
    {
      _clean();
      THROW_HW_ERROR(Error) << "Can't start watching thread";
    }
}

_LocalDirectoryEvent::~_LocalDirectoryEvent()
{
  _clean();
}

void _LocalDirectoryEvent::_clean()
{
  m_cond.acquire();
  m_quit = true;
  m_cond.signal();
  m_cond.release();

  close(m_pipes[1]);
  void *tReturn;
  if(m_thread_id != pthread_t(-1))
    pthread_join(m_thread_id,&tReturn);
  close(m_pipes[0]);
  _stopWatch();
  if(m_inotify_fd >= 0)
    close(m_inotify_fd);
}

void _LocalDirectoryEvent::_startWatch()
{
  m_inotify_wd = inotify_add_watch(m_inotify_fd,
				   m_current_parameters.watch_path.c_str(),
				   m_watch_mask);
}

void _LocalDirectoryEvent::_stopWatch()
{
  if(m_inotify_wd >= 0)
    {
      inotify_rm_watch(m_inotify_fd,m_inotify_wd);
      m_inotify_wd = -1;
    }
}

void _LocalDirectoryEvent::_signal()
{
  write(m_pipes[1],"|",1);
}

void _LocalDirectoryEvent::_run()
{
  DEB_MEMBER_FUNCT();

  struct pollfd fds[2];
  fds[0].fd = m_pipes[0];
  fds[0].events = POLLIN;
  fds[1].fd = m_inotify_fd;
  fds[1].events = POLLIN;
  while(1)
    {
      poll(fds,2,-1);
      if(fds[1].revents)
	{
	  char buffer[EVENT_BUF_LEN];
	  int length = read(m_inotify_fd,
			    buffer,sizeof(buffer));
	  char *aPt = buffer;
	  while(length > 0)
	    {
	      struct inotify_event *event = (struct inotify_event*)aPt;
	      if(event->len)
		{
		  const char* filename = event->name;
		  int imageNb;
		  if(sscanf(filename,m_scanf_format,&imageNb) == 1)
		    {
		      char aBuffer[256];
		      snprintf(aBuffer,sizeof(aBuffer),
			       m_current_parameters.file_pattern.c_str(),imageNb);

		      AutoMutex lock(m_cond.mutex());
		      if(m_wait)
			_stopWatch();
		      else if(!strcmp(aBuffer,filename))
			{
			  int nextImageExpected = m_current_parameters.next_file_number_expected;
			  std::string fullPath = m_current_parameters.watch_path + "/";
			  fullPath += filename;
			  lock.unlock();

			  bool continueFlag;
			  if(nextImageExpected == imageNb)
			    {
			      continueFlag = m_cbk.nextFileExpected(imageNb,fullPath.c_str(),
								    ++nextImageExpected);
			    }
			  else
			    continueFlag = m_cbk.newFile(imageNb,fullPath.c_str());

			  lock.lock();
			  m_current_parameters.next_file_number_expected = nextImageExpected;
			  if(!m_wait)
			    m_wait = !continueFlag;
			}
		    }
		}
	      aPt += EVENT_SIZE + event->len;
	      length -= EVENT_SIZE + event->len;
	    }
	}

      if(m_wait || fds[0].revents)
	{
	  if(fds[0].revents)
	    {
	      char buffer[1024];
	      read(m_pipes[0],buffer,sizeof(buffer));
	    }
	  AutoMutex lock(m_cond.mutex());
	  if(m_quit) break;
	  while(m_wait && !m_quit)
	    {
	      _stopWatch();
	      m_thread_running = false;
	      m_cond.signal();
	      DEB_TRACE() << "wait";
	      m_cond.wait();
	      DEB_TRACE() << "run";
	      m_thread_running = true;
	    }
	  if(!m_quit) _startWatch();
	}
    }
}
#endif
/** @brief this local file event use inotify so it can only be use an local pc or NFS server
 */
class _GenericDirectoryEvent : public DirectoryEvent::_Event
{
  DEB_CLASS_NAMESPC(DebModCommon,"_LocalDirectoryEvent","Common");
  static void* _runFunc(void *arg)
  {
    ((_GenericDirectoryEvent*)arg)->_run();
    return NULL;
  }
  void _run();
public:
  _GenericDirectoryEvent(DirectoryEvent::Callback &cbk);
  ~_GenericDirectoryEvent();
};

_GenericDirectoryEvent::_GenericDirectoryEvent(DirectoryEvent::Callback &cbk) :
  DirectoryEvent::_Event(cbk)
{
  DEB_CONSTRUCTOR();

  if(pthread_create(&m_thread_id,NULL,_GenericDirectoryEvent::_runFunc,this))
    THROW_HW_ERROR(Error) << "Can't start watching thread";
}

_GenericDirectoryEvent::~_GenericDirectoryEvent()
{
  AutoMutex lock(m_cond.mutex());
  m_quit = true;
  m_cond.signal();
  lock.unlock();
  void *tReturn;
  if(m_thread_id >= 0)
    pthread_join(m_thread_id,&tReturn);
}
void _GenericDirectoryEvent::_run()
{
  DEB_MEMBER_FUNCT();

  time_t lastDirectoryTime = -1,newDirectoryTime = -1;
  DIR *dir = NULL;
  int dirFd;
  std::string lastDirPath;
  struct stat aDirectoryStat;

  AutoMutex lock(m_cond.mutex());
  while(!m_quit)
    {
      while(m_wait && !m_quit)
	{
	  m_thread_running = false;
	  m_cond.signal();
	  m_cond.wait();
	  m_thread_running = true;
	}

      if(!m_quit)
	{
	  if(!dir || lastDirPath != m_current_parameters.watch_path)
	    {
	      if(dir) closedir(dir);
	      dir = opendir(m_current_parameters.watch_path.c_str());
	      if(!dir)
		{
		  DEB_ERROR() << "Could not open path" << 
		    DEB_VAR1(m_current_parameters.watch_path);
		  m_wait = true;
		}
	      else
		{
		  lastDirPath = m_current_parameters.watch_path,lastDirectoryTime = -1;
		  dirFd = dirfd(dir);
		}
	    }

	  newDirectoryTime = -1;	// force to refresh

	  while(!m_quit && !m_wait)
	    {

	      while(!m_wait && !m_quit &&
		    lastDirectoryTime == newDirectoryTime)
		{
		  m_cond.wait(0.1);
		  if(fstat(dirFd,&aDirectoryStat))
		    {
		      DEB_ERROR() << "Can't stat directory: " << DEB_VAR1(lastDirPath);
		      m_wait = true;
		    }
		  else
		    newDirectoryTime = aDirectoryStat.st_mtime;
		}
	    
	      int nextImageExpected = m_current_parameters.next_file_number_expected;
	      char filename[256];
	      snprintf(filename,sizeof(filename),
		       m_current_parameters.file_pattern.c_str(),nextImageExpected);
	      std::string fullPath = m_current_parameters.watch_path + "/";
	      fullPath += filename;
	      lock.unlock();

	      bool continueFlag;
	      if(!access(fullPath.c_str(),F_OK))
		{
		  continueFlag = m_cbk.nextFileExpected(m_current_parameters.next_file_number_expected,
							fullPath.c_str(),
							++nextImageExpected);
		  lock.lock();
		  m_current_parameters.next_file_number_expected = nextImageExpected;
		}
	      else		// We didn't access the next image
		{
		  rewinddir(dir);
		  do
		    {
		      struct dirent entry;
		      struct dirent *result;
		      int status = readdir_r(dir,&entry,&result);
		      if(!status && result)
			{
			  const char* filename = result->d_name;
			  int imageNb;
			  if(sscanf(filename,m_scanf_format,&imageNb) == 1)
			    {
			      char aBuffer[256];
			      snprintf(aBuffer,sizeof(aBuffer),
				       m_current_parameters.file_pattern.c_str(),imageNb);

			      if(imageNb > nextImageExpected)
				{
				  if(!strcmp(aBuffer,filename))
				    {
				      std::string fullPath = m_current_parameters.watch_path + "/";
				      fullPath += filename;
				      continueFlag = m_cbk.newFile(imageNb,fullPath.c_str());
				    }
				}
			    }
			}
		      else
			break;
		    }
		  while(1);
		  lock.lock();
		}
	      if(!m_wait)
		m_wait = !continueFlag;
	      lastDirectoryTime = newDirectoryTime;
	    }
	}
    }
  if(dir) closedir(dir);
}

/** @brief this class manage file event on a directory.
 *  
 *  This class is dedicated to follow a detector acquisition on disk,
 *  it's call DirectoryEvent::Callback class when a
 *  new file just arrived in the directory.
 *  @param local if true use a local event technique, this can't work on an NFS client.
 *  @param cbk the callback class
 */
DirectoryEvent::DirectoryEvent(bool local,Callback &cbk)
{
#ifdef HAS_INOTIFY
  if(local)
    m_event = new _LocalDirectoryEvent(cbk),m_local = true;
  else
#endif
    m_event = new _GenericDirectoryEvent(cbk),m_local = false;
}

DirectoryEvent::~DirectoryEvent()
{
  delete m_event;
}
/** @brief prepare method register new parameters. This methode should be called 
 *  before start
 */
void DirectoryEvent::prepare(const DirectoryEvent::Parameters& parameters)
{
  m_event->prepare(parameters);
}
/** @brief start the event watcher
 */
void DirectoryEvent::start()
{
  m_event->start();
}
/** @brief stop the event watcher
 */
void DirectoryEvent::stop()
{
  m_event->stop();
}
/** @brief return the event watcher state
 */ 
bool DirectoryEvent::isStopped() const
{
  return m_event->isStopped();
}
/** @brief return the next expected file number.
 *
 *  i.e: if the file_pattern == "test_%.3d.edf" and filenumber == 42
 *  then the next filename expected is test_042.edf
 */
int DirectoryEvent::getNextFileNumberExpected() const
{
  return m_event->getNextFileNumberExpected();
}
void DirectoryEvent::watch_moved_to()
{
#ifdef HAS_INOTIFY
  if(m_local)
    ((_LocalDirectoryEvent*)m_event)->watch_moved_to();
#endif  
}
void DirectoryEvent::watch_close_write()
{
#ifdef HAS_INOTIFY
  if(m_local)
    ((_LocalDirectoryEvent*)m_event)->watch_close_write();
#endif  
}
#endif
