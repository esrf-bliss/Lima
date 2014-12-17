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
#include <iostream>
#include "SoftOpExternalMgr.h"
#include "SizeUtils.h"

using namespace std;
using namespace lima;

int main(int argc, char *argv[])
{
	cout << "Hello!" << endl;

	SoftOpExternalMgr op_ext_mgr;
	cout << "SoftOpExternalMgr created!" << endl;

	Roi roi = Roi(0, 0, 100, 100);
	cout << "roi=" << roi << endl;

	SoftOpInstance roi_cnt_op_inst;
	op_ext_mgr.addOp(ROICOUNTERS, "RoiCounters", 0, roi_cnt_op_inst);
	cout << "ROICOUNTERS op added!" << endl;

	SoftOpBaseClass *p = roi_cnt_op_inst.m_opt;
	SoftOpRoiCounter *roi_cnt_op = static_cast<SoftOpRoiCounter *>(p);

	list<SoftOpRoiCounter::RoiNameAndRoi> roi_list;
	roi_list.push_back(SoftOpRoiCounter::RoiNameAndRoi("roi1",roi));
	roi_cnt_op->updateRois(roi_list);
	cout << "roi list added!" << endl;

	list<SoftOpRoiCounter::RoiNameAndRoi> roi_list_check;
	roi_cnt_op->getRois(roi_list_check);
	list<SoftOpRoiCounter::RoiNameAndRoi>::const_iterator i, end = roi_list_check.end();
	for (i = roi_list_check.begin(); i != end; ++i) {
		const Roi& roi_check = i->second;
		cout << "roi_check=" << roi_check << endl;
	}

	return 0;

}
