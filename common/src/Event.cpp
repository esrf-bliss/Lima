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
#include "lima/Event.h"

using namespace lima;
using namespace std;

//----------------------------------------------------------------
// Event
//----------------------------------------------------------------

Event::Event()
	: layer(Common), severity(Info), domain(Other), code(Default)
{
	abs_timestamp = Timestamp::now();
}

Event::Event(Layer l, Severity s, Domain d, Code c, const string &e)
	: layer(l), severity(s), domain(d), code(c), desc(e)
{
	abs_timestamp = Timestamp::now();
}

Event::~Event()
{
}

string Event::getMsgStr()
{
	ostringstream os;

#define CaseOutput(x)				\
	case x:  os << #x; break
#define DefaultOutput(x)			\
	default: os << "Unknown (" << int(x) << ")"

	os << "Event";
	if (rel_timestamp.isSet())
		os << " [" << rel_timestamp << "]";
	os << ": Severity=";
	switch (severity) {
		CaseOutput(Info);
		CaseOutput(Warning);
		CaseOutput(Error);
		CaseOutput(Fatal);
		DefaultOutput(severity);
	}
	os << ", Domain=";
	switch (domain) {
		CaseOutput(Saving);
		CaseOutput(Processing);
		CaseOutput(Camera);
		CaseOutput(Other);
		DefaultOutput(domain);
	}
	os << ", Code=";
	switch (code) {
		CaseOutput(Default);
		CaseOutput(SaveUnknownError);
		CaseOutput(SaveOpenError);
		CaseOutput(SaveCloseError);
		CaseOutput(SaveAccessError);
		CaseOutput(SaveOverwriteError);
		CaseOutput(SaveDiskFull);
		CaseOutput(SaveOverun);
		CaseOutput(ProcessingOverun);
		CaseOutput(CamOverrun);
		CaseOutput(CamNoMemory);
		CaseOutput(CamCommError);
		CaseOutput(CamCommLost);
		CaseOutput(CamAlarm);
		CaseOutput(CamFault);
		DefaultOutput(code);
	}
	if (!desc.empty())
		os << ": " << desc;

#undef CaseOutput
#undef DefaultOutput

	return os.str();
}

DebProxy Event::getDebug(DebObj *deb_obj) const
{
	DEB_FROM_PTR(deb_obj);

	switch (severity) {
	case Event::Fatal:
		return DEB_FATAL();
	case Event::Error:
		return DEB_ERROR();
	case Event::Warning:
		return DEB_WARNING();
	default:
		return DEB_TRACE();
	}
}

ostream& lima::operator <<(ostream& os, const Event& event)
{
	Event& noconst_event = const_cast<Event&>(event);
	return os << "<" << noconst_event.getMsgStr() << ">";
}


//----------------------------------------------------------------
// EventCallback
//----------------------------------------------------------------

EventCallback::EventCallback()
	: m_cb_gen(NULL)
{
	DEB_CONSTRUCTOR();
}

EventCallback::~EventCallback()
{
	DEB_DESTRUCTOR();

	if (m_cb_gen != NULL)
		m_cb_gen->unregisterEventCallback(*this);
}

void EventCallback::setEventCallbackGen(EventCallbackGen *cb_gen)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(cb_gen, m_cb_gen);

	if (cb_gen && m_cb_gen) {
		THROW_HW_ERROR(InvalidValue) <<  
			"EventCallbackGen is already set";
	} else if (!cb_gen && !m_cb_gen) {
		THROW_HW_ERROR(InvalidValue) << 
			"EventCallbackGen is not set";
	}

	m_cb_gen = cb_gen;
}

//----------------------------------------------------------------
// EventCallbackGen
//----------------------------------------------------------------

EventCallbackGen::EventCallbackGen()
	: m_cb(NULL)
{
	DEB_CONSTRUCTOR();
}

EventCallbackGen::~EventCallbackGen()
{
	DEB_DESTRUCTOR();
	
	if (m_cb != NULL)
		unregisterEventCallback(*m_cb);
}

void EventCallbackGen::registerEventCallback(EventCallback& cb)
{
	DEB_MEMBER_FUNCT();

	if (m_cb)
		THROW_HW_ERROR(InvalidValue) << 
			"An EventCallback is already registered";

	cb.setEventCallbackGen(this);
	m_cb = &cb;
}

void EventCallbackGen::unregisterEventCallback(EventCallback& cb)
{
	DEB_MEMBER_FUNCT();

	if (m_cb != &cb)
		THROW_HW_ERROR(InvalidValue) << 
			"Specified EventCallback is not registered";

	m_cb = NULL;
	cb.setEventCallbackGen(NULL);
}

bool EventCallbackGen::hasRegisteredCallback()
{
	return !!m_cb;
}

void EventCallbackGen::reportEvent(Event *event)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(*event);

	if (!m_cb) {
		DEB_EVENT(*event) << *event;
		delete event;
		return;
	}

	m_cb->processEvent(event);
}
