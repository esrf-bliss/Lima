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
class CtBuffer {
%TypeHeaderCode
#include <CtBuffer.h>
#include <Constants.h>
#include <sstream>
using namespace lima;
%End
    public:
	struct Parameters {
		BufferMode mode;
		long	nbBuffers;
		short	maxMemory;

		const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
	};

	CtBuffer(HwInterface *hw);
	~CtBuffer();

	void setMode(BufferMode mode);
	void getMode(BufferMode& mode /Out/) const;

	void setNumber(long nb_buffers);
	void getNumber(long& nb_buffers /Out/) const;

	void setMaxMemory(short max_memory);
	void getMaxMemory(short& max_memory /Out/) const;

	void registerFrameCallback(CtControl *ct);
        void unregisterFrameCallback();

	void getFrame(Data &data /Out/,int frameNumber,int readBlockLen=1);

	void reset();
	void setup(CtControl *ct);

	bool isAccumulationActive() const;

	static void getDataFromHwFrameInfo(Data& /Out/,const HwFrameInfoType&,
					   int readBlockLen=1);
};
