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
#include "lima/CtEvent.h"

using namespace lima;

class CtEvent::_InternalEventCBK : public EventCallback
{
public:
  _InternalEventCBK(CtEvent& ct_event)
    : m_ct_event(ct_event)
  {}

  virtual ~_InternalEventCBK()
  {}

  virtual void processEvent(Event *event)
  { m_ct_event.reportEvent(event); }

private:
  CtEvent& m_ct_event;
};


CtEvent::CtEvent(CtControl& ct)
  : m_ct(ct)
{
  DEB_CONSTRUCTOR();

  HwInterface *hw = m_ct.hwInterface();
  hw->getHwCtrlObj(m_event_ctrl_obj);

  if (!m_event_ctrl_obj)
    return;

  m_cb = new _InternalEventCBK(*this);
  m_event_ctrl_obj->registerEventCallback(*m_cb);
}

CtEvent::~CtEvent()
{
  DEB_DESTRUCTOR();

  if (m_cb)
    delete m_cb;

  resetEventList();
}

bool CtEvent::hasCapability() const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(!!m_event_ctrl_obj);
  return !!m_event_ctrl_obj;
}

void CtEvent::resetEventList()
{
  DEB_MEMBER_FUNCT();

  AutoMutex l = lock();

  EventList::iterator it, end = m_event_list.end();
  for (it = m_event_list.begin(); it != end; ++it) {
    Event *event = *it;
    delete event;
  }

  m_event_list.clear();
}

void CtEvent::getEventList(EventList& event_list)
{
  DEB_MEMBER_FUNCT();

  AutoMutex l = lock();
  DEB_PARAM() << DEB_VAR2(event_list.size(), m_event_list.size());

  if (!event_list.empty())
    THROW_CTL_ERROR(InvalidValue) << "Not empty event_list (" 
				  << DEB_VAR1(event_list.size()) << "): "
				  << "Where all the events deleted?";
  else if (hasRegisteredCallback())
    DEB_WARNING() << "An EventCallback is registered, events are dispatched";

  event_list = m_event_list;
  m_event_list.clear();
}

void CtEvent::_prepareAcq()
{
  DEB_MEMBER_FUNCT();

  resetEventList();
}

void CtEvent::reportEvent(Event *event)
{
  DEB_MEMBER_FUNCT();

  HwInterface *hw_inter = m_ct.hwInterface();
  HwBufferCtrlObj *buffer_ctrl_obj;
  if (!event->rel_timestamp.isSet()) {
    if (hw_inter->getHwCtrlObj(buffer_ctrl_obj)) {
      Timestamp start_timestamp;
      buffer_ctrl_obj->getStartTimestamp(start_timestamp);
      if (start_timestamp.isSet())
	event->rel_timestamp = event->abs_timestamp - start_timestamp;
      else
	DEB_WARNING() << "HwBufferCtrlObj start timestamp not set!";
    } else
      DEB_WARNING() << "HwInterface does not have a HwBufferCtrlObj!";
  }

  DEB_EVENT(*event) << DEB_VAR1(*event);

  AutoMutex l = lock();

  if ((event->severity == Event::Fatal) ||
      (event->severity == Event::Error)) {
    CtControl::ErrorCode ct_err_code;

#define CaseCode(x)				\
    case Event::x: ct_err_code = CtControl::x; break

    if (event->domain == Event::Camera)
      ct_err_code = CtControl::CameraError;
    else switch (event->code) {
      CaseCode(SaveUnknownError);
      CaseCode(SaveOpenError);
      CaseCode(SaveCloseError);
      CaseCode(SaveAccessError);
      CaseCode(SaveOverwriteError);
      CaseCode(SaveDiskFull);
      CaseCode(SaveOverun);
      CaseCode(ProcessingOverun);
    default: ct_err_code = CtControl::EventOther;
    }

    Data *data = new Data();
    m_ct.abortAcq(AcqFault, ct_err_code, *data);
  }

  if (hasRegisteredCallback())
    EventCallbackGen::reportEvent(event);
  else
    m_event_list.push_back(event);

}

void CtEvent::registerEventCallback(EventCallback& cb)
{
  DEB_MEMBER_FUNCT();
  AutoMutex l = lock();
  EventCallbackGen::registerEventCallback(cb);
}

void CtEvent::unregisterEventCallback(EventCallback& cb)
{
  DEB_MEMBER_FUNCT();
  AutoMutex l = lock();
  EventCallbackGen::unregisterEventCallback(cb);
}
