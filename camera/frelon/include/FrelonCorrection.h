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
#ifndef FRELONCORRECTION_H
#define FRELONCORRECTION_H

#include "LinkTask.h"
#include "SizeUtils.h"

namespace lima
{

namespace Frelon
{

class E2VCorrection : public LinkTask
{
 public:
	static const int FirstCol, LastCol;
	static const double CorrFactor;

	explicit E2VCorrection();
	E2VCorrection(const E2VCorrection& o);
	~E2VCorrection();

	void setHwBin(const Bin& hw_bin);
	void getHwBin(Bin& hw_bin);
	void setHwRoi(const Roi& hw_roi);
	void getHwRoi(Roi& hw_roi);

	virtual Data process(Data& data);

 private:
	Bin m_hw_bin;
	Roi m_hw_roi;
};

} // namespace Frelon

} // namespace lima

#endif // FRELONCORRECTION_H
