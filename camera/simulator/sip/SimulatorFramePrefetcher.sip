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

namespace Simulator
{

class FrameLoaderPrefetched
{
%TypeHeaderCode
#include "SimulatorFramePrefetcher.h"
#include "SimulatorFrameLoader.h"

namespace lima { namespace Simulator {
typedef Simulator::FramePrefetcher<Simulator::FrameLoader> FrameLoaderPrefetched;
}}
%End

public:
    FrameLoaderPrefetched();
    ~FrameLoaderPrefetched();

    void getNbPrefetchedFrames(unsigned int& nb_prefetched_frames) const;
    void setNbPrefetchedFrames(unsigned int nb_prefetched_frames);

    void prepareAcq();
    bool getNextFrame(unsigned char *ptr);

    unsigned long getFrameNr() const;
    void resetFrameNr(unsigned long frame_nr = 0);

    void setFrameDim(const FrameDim& frame_dim);
    void getFrameDim(FrameDim& frame_dim) const;

    void getMaxImageSize(Size& max_image_size) const;

    // Inherited from FrameLoader
    void setFilePattern(const std::string& file_pattern);
    void getFilePattern(std::string& file_pattern /Out/);
};


class FrameBuilderPrefetched
{
%TypeHeaderCode
#include "SimulatorFramePrefetcher.h"
#include "SimulatorFrameBuilder.h"

namespace lima { namespace Simulator {
typedef Simulator::FramePrefetcher<Simulator::FrameBuilder> FrameBuilderPrefetched;
}}
%End

public:
    FrameBuilderPrefetched();
    ~FrameBuilderPrefetched();

    void getNbPrefetchedFrames(unsigned int& nb_prefetched_frames) const;
    void setNbPrefetchedFrames(unsigned int nb_prefetched_frames);

    void prepareAcq();
    bool getNextFrame(unsigned char *ptr);

    unsigned long getFrameNr() const;
    void resetFrameNr(unsigned long frame_nr = 0);

    void setFrameDim(const FrameDim& frame_dim);
    void getFrameDim(FrameDim& frame_dim) const;

    void getMaxImageSize(Size& max_image_size) const;

    // Inherited from FrameBuilder
	void getBin( Bin &bin /Out/ ) const;
	void setBin( const Bin &bin );
	void checkBin( Bin &bin /In,Out/ ) const;

	void getRoi( Roi &roi /Out/ ) const;
	void setRoi( const Roi &roi );
	void checkRoi( Roi &roi /In,Out/ ) const;

	void getPeaks( std::vector<struct Simulator::GaussPeak> &peaks /Out/ ) const;
	void setPeaks( const std::vector<struct Simulator::GaussPeak> &peaks );

	void getPeakAngles( std::vector<double> &angles /Out/) const;
	void setPeakAngles( const std::vector<double> &angles );

	void getFillType( Simulator::FrameBuilder::FillType &fill_type /Out/) const;
	void setFillType( Simulator::FrameBuilder::FillType fill_type );

	void getRotationAxis( Simulator::FrameBuilder::RotationAxis &rot_axis /Out/) const;
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