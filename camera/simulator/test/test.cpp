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
/***************************************************************//**
 * @file   test.cpp
 * @brief  This file is to test FrameBuilder and HwBufferSave classes
 *
 * @author A.Kirov, A.Homs
 * @date   03/06/2009
 *******************************************************************/

#include <vector>
#include <exception>
#include <iostream>

#include "FrameBuilder.h"
#include "HwBufferSave.h"
#include "SizeUtils.h"
#include "Exceptions.h"
#include "AutoObj.h"

using namespace std;
using namespace lima;

int main( void )
{
  try {

	FrameBuilder fb;

	FrameDim full_fd;
	fb.getFrameDim(full_fd);
	
	Bin bin = Bin(2,2);
	fb.setBin(bin);

	Roi roi = Roi(Point(128, 128), Point(384, 384));
	fb.checkRoi(roi);
	fb.setRoi(roi);

//	FrameDim fd = full_fd/bin;
	FrameDim fd = FrameDim(roi.getSize(), full_fd.getImageType());

	HwBufferSave bs(HwBufferSave::EDF, "test");
	bs.setTotFileFrames(1);

	int size = fd.getMemSize();
	AutoPtr<unsigned char, true> buffer;
	buffer = new unsigned char[size];

	Timestamp start = Timestamp::now();

	for( int i=0; i<10; i++ ) {
		int frame_nb = fb.getFrameNr();
		fb.getNextFrame( buffer );

		Timestamp t = Timestamp::now() - start;
		int pixels = Point(fd.getSize()).getArea();
		HwFrameInfoType finfo(frame_nb, buffer, &fd, t, pixels,
				      HwFrameInfoType::Managed);
		bs.writeFrame(finfo);
	}

	return 0;

  } catch (Exception &e) {
  	cerr << e << endl;
	return -1;
  }
}
