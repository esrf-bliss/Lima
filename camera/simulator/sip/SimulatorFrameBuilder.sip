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

%MappedType std::vector<struct Simulator::GaussPeak>
{
%TypeHeaderCode
#include <vector>
#include <SimulatorFrameBuilder.h>
using namespace lima;
typedef struct Simulator::GaussPeak PeakType;
typedef std::vector<PeakType> PeakListType;

%End

%ConvertToTypeCode
	if (sipIsErr == NULL) {
		bool ok = PyList_Check(sipPy);
		for (int i = 0; ok && (i < PyList_Size(sipPy)); ++i) {
			PyObject *p = PyList_GET_ITEM(sipPy, i);
			ok = !!sipCanConvertToType(p,
					sipType_Simulator_GaussPeak, 
					SIP_NOT_NONE);
		}
		return ok;
	}

	PeakListType *peaks = new PeakListType();

	for (int i = 0; i < PyList_Size(sipPy); ++i) {
		PyObject *p = PyList_GET_ITEM(sipPy, i);
		int state;
		void *v = sipConvertToType(p, sipType_Simulator_GaussPeak, 0,
					   SIP_NOT_NONE, &state, sipIsErr);	
		// Deal with any errors.
		if (*sipIsErr) {
			sipReleaseType(v, sipType_Simulator_GaussPeak, state);
	 
			// Tidy up.
			delete peaks;

			// There is no temporary instance.
			return 0;
		}

		PeakType *peak = reinterpret_cast<PeakType*>(v);
		peaks->push_back(*peak);
		sipReleaseType(v, sipType_Simulator_GaussPeak, state);
	}

	*sipCppPtr = peaks;
	return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
	PyObject *l;

	// Create the Python list of the correct length.
	if (!(l = PyList_New(sipCpp->size())))
		return NULL;

	sipTransferObj = NULL;
	int i = 0;
	typedef PeakListType::iterator PeakListIt;
	for (PeakListIt it = sipCpp->begin(); it != sipCpp->end(); ++it, ++i) {
		// We need a new underlying GaussPeak object for Python
		PeakType *peak = new PeakType(*it);
		PyObject *o;
		o = sipConvertFromNewType(peak, sipType_Simulator_GaussPeak,
					  sipTransferObj);
                if (!o) {
			delete peak;
			Py_DECREF(l);
			return NULL;
		}

		PyList_SET_ITEM(l, i, o);
	}

	// Return the Python list.
	return l;
%End
};

namespace Simulator
{
struct GaussPeak {
%TypeHeaderCode
#include "SimulatorFrameBuilder.h"
%End
	double x0;
	double y0;
	double fwhm;
	double max;
	GaussPeak();
	GaussPeak(const Simulator::GaussPeak& o);
	GaussPeak(double x, double y, double w, double m);
};


class FrameBuilder
{
%TypeHeaderCode
#include "SimulatorFrameBuilder.h"
#include <vector>
%End

public:
	enum FillType { 
		Gauss, Diffraction,
	};
	enum RotationAxis {
		RotationX, RotationY,
	};

	FrameBuilder();
	FrameBuilder( FrameDim &frame_dim, Bin &bin, Roi &roi,
	              const std::vector<struct Simulator::GaussPeak> &peaks, 
		      double grow_factor );
	~FrameBuilder();

	void getFrameDim( FrameDim &dim /Out/ ) const;
	void setFrameDim( const FrameDim &dim );

	unsigned long getFrameNr();
	void resetFrameNr( int frame_nr=0 );

	void getMaxImageSize(Size& max_size /Out/);

	void getBin( Bin &bin /Out/ ) const;
	void setBin( const Bin &bin );
	void checkBin( Bin &bin /In,Out/ ) const;

	void getRoi( Roi &roi /Out/ ) const;
	void setRoi( const Roi &roi );
	void checkRoi( Roi &roi /In,Out/ ) const;

	void getPeaks( std::vector<struct Simulator::GaussPeak> &peaks /Out/ ) 
									const;
	void setPeaks( const std::vector<struct Simulator::GaussPeak> &peaks );

	void getPeakAngles( std::vector<double> &angles /Out/) const;
	void setPeakAngles( const std::vector<double> &angles );

	void getFillType( Simulator::FrameBuilder::FillType &fill_type /Out/)
									 const;
	void setFillType( Simulator::FrameBuilder::FillType fill_type );

	void getRotationAxis( Simulator::FrameBuilder::RotationAxis &rot_axis /Out/)
									 const;
	void setRotationAxis( Simulator::FrameBuilder::RotationAxis rot_axis );

	void getRotationAngle( double &a /Out/ ) const;
	void setRotationAngle( const double &a );
	
	void getRotationSpeed( double &s /Out/ ) const;
	void setRotationSpeed( const double &s );
	
	void getGrowFactor( double &grow_factor /Out/ ) const;
	void setGrowFactor( const double &grow_factor );

	void getDiffractionPos( double &x /Out/, double &y /Out/ ) const;
	void setDiffractionPos( const double &x, const double &y );

	void getDiffractionSpeed( double &sx /Out/, double &sy /Out/ ) const;
	void setDiffractionSpeed( const double &sx, const double &sy );
};

};
