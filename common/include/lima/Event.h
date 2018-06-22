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
#ifndef EVENT_H
#define EVENT_H

#include "lima/LimaCompatibility.h"
#include "lima/Timestamp.h"
#include <string>
#include <ostream>

namespace lima
{

class LIMACORE_API Event
{
 public:
	enum Severity {
		Info, Warning, Error, Fatal,
	};
	enum Domain {
		Saving, Processing, Camera, Other,
	};
	enum Code {
		Default,
		SaveUnknownError, SaveOpenError, SaveCloseError,
		SaveAccessError, SaveOverwriteError, SaveDiskFull, SaveOverun,
		ProcessingOverun,
		CamOverrun, CamNoMemory, CamCommError, CamCommLost, 
		CamAlarm, CamFault,
	};

	Layer layer;
	Severity severity;
	Domain domain;
	Code code;
	std::string desc;
	Timestamp abs_timestamp;
	Timestamp rel_timestamp;

	Event();
	Event(Layer l, Severity s, Domain d, Code c,const std::string &e);
	virtual ~Event();

	virtual std::string getMsgStr();

	DebProxy getDebug(DebObj *deb) const;
};

#define DEB_EVENT(event)	(event).getDebug(DEB_PTR())

std::ostream& operator <<(std::ostream& os, const Event& event);


class EventCallbackGen;

class LIMACORE_API EventCallback
{
	DEB_CLASS(DebModCommon, "EventCallback");

 public:
	EventCallback();
	virtual ~EventCallback();

	virtual void processEvent(Event *event) = 0;

 private:
	friend class EventCallbackGen;
	void setEventCallbackGen(EventCallbackGen *cb_gen);

	EventCallbackGen *m_cb_gen;
};

class LIMACORE_API EventCallbackGen
{
	DEB_CLASS(DebModCommon, "EventCallbackGen");

 public:
	EventCallbackGen();
	virtual ~EventCallbackGen();

	virtual void   registerEventCallback(EventCallback& cb);
	virtual void unregisterEventCallback(EventCallback& cb);
	virtual bool hasRegisteredCallback();

	virtual void reportEvent(Event *event);

 private:
	EventCallback *m_cb;
};


} // namespace lima


#endif // EVENTCALLBACK_H
