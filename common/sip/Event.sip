//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
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

class Event
{
%TypeHeaderCode
#include "lima/Event.h"
using namespace lima;
%End
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
	Event(Layer l, Severity s, Domain d, Code c, const std::string &e);
	virtual ~Event();

	virtual std::string getMsgStr() const;
};


class EventCallback
{
%TypeHeaderCode
#include "lima/Event.h"
using namespace lima;
%End
 public:
	EventCallback();
	virtual ~EventCallback();

	virtual void processEvent(Event *event /TransferBack/) = 0;
};


class EventCallbackGen
{
%TypeHeaderCode
#include "lima/Event.h"
using namespace lima;
%End
 public:
	EventCallbackGen();
	virtual ~EventCallbackGen();

	void registerEventCallback(EventCallback& event_cb);
	void unregisterEventCallback(EventCallback& event_cb);
	virtual bool hasRegisteredCallback();

	void reportEvent(Event *event /Transfer/);
};
