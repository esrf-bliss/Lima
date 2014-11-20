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

#ifdef __unix
#include <sys/time.h>
#else
#include <sys/timeb.h>
#include <time.h>
#endif

#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

#include <yat/time/Timer.h>
#include "CtSaving_Nxs.h"

using namespace lima;

/** @brief saving container
 *
 *  This class manage Nexus file saving
 */

//--------------------------------------------------------------------------------------------------------------------
//- ctor
//--------------------------------------------------------------------------------------------------------------------
SaveContainerNxs::SaveContainerNxs(CtSaving::Stream& stream)	: CtSaving::SaveContainer(stream)
{
	DEB_CONSTRUCTOR();
	m_writer = 0;
}

//--------------------------------------------------------------------------------------------------------------------
//- dtor
//--------------------------------------------------------------------------------------------------------------------
SaveContainerNxs::~SaveContainerNxs()
{
	DEB_DESTRUCTOR();
}

//--------------------------------------------------------------------------------------------------------------------
//- Event rising by CtSaving when frame is acquired (newFrameReady)
//--------------------------------------------------------------------------------------------------------------------
bool SaveContainerNxs::_open(const std::string &filename, std::ios_base::openmode openFlags)
{
	DEB_MEMBER_FUNCT();
	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//- Event rising by CtSaving when frame is saved
//--------------------------------------------------------------------------------------------------------------------
void SaveContainerNxs::_close()
{
	DEB_MEMBER_FUNCT();
}


//--------------------------------------------------------------------------------------------------------------------
//- Event rising by CtSaving when ???
//--------------------------------------------------------------------------------------------------------------------
void SaveContainerNxs::_clear()
{
	DEB_MEMBER_FUNCT();

	nxcpp::DataStreamer::ResetBufferIndex();
}


//--------------------------------------------------------------------------------------------------------------------
//- create nexus object
//- Initialize nexus object
//- Add sensor
//- Start Acquisition
//- for each new image :
//- 	.PushData in Nexus File
//- when last image is acquired:
//- 	.End Acquisition
//- 	.Finalize
//--------------------------------------------------------------------------------------------------------------------
void SaveContainerNxs::_writeFile(Data &aData,
								  CtSaving::HeaderMap &aHeader,
								  CtSaving::FileFormat aFormat)
{
	DEB_MEMBER_FUNCT();

	try
	{
		yat::Timer t;
		DEB_TRACE() << "SaveContainerNxs::_writeFile() - frameNumber = " << aData.frameNumber;
		//that's mean that snap was stopped previous by user command or device was hang
		//so me must clean the NXS object
		if(m_writer && aData.frameNumber == 0)
		{
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - Abort() current writer";
			//Abort every current task && go away
			t.restart();
			m_writer->Abort();
			DEB_TRACE() << "Abort DataStreamer : " << t.elapsed_msec() << " ms";

			//destroy object
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - delete the writer";
			t.restart();
			delete m_writer;
			m_writer = 0;
			DEB_TRACE() << "Delete DataStreamer : " << t.elapsed_msec() << " ms";
		}

		//prepare nexus object : to do once for each new sequence
		if(!m_writer)
		{
			//get acquisition parameters
			getParameters(m_pars);

			//create N4T main object needed to generate Nexus file
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - create the writer";
			t.restart();
			m_writer = new nxcpp::DataStreamer(m_pars.prefix, (std::size_t)m_pars.nbframes, (std::size_t)m_pars.framesPerFile);

			DEB_TRACE() << "SaveContainerNxs::_writeFile() - initialize the writer";
			m_writer->Initialize(m_pars.directory, "");

			//decode options (split) ///////////////////////////////////////////////
			//get acquisition parameters
			std::stringstream ss(m_pars.options);
			std::string field;
			m_options.clear();
			while (getline(ss, field, '|'))
				m_options.push_back(field);
			////////////////////////////////////////////////////////////////////////	

			//by default is IMMEDIATE			
			if(m_options.at(0) == "SYNCHRONOUS" )
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Write Mode = SYNCHRONOUS";
				m_writer->SetWriteMode(nxcpp::NexusFileWriter::SYNCHRONOUS);
			}
			else if (m_options.at(0) == "DELAYED" )
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Write Mode = DELAYED";
				m_writer->SetWriteMode(nxcpp::NexusFileWriter::DELAYED);
			}
			else
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Write Mode = IMMEDIATE";
				m_writer->SetWriteMode(nxcpp::NexusFileWriter::IMMEDIATE);
			}

			//Add sensor 2D (image) // height,width
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - Add sensor 2D (image)";
			m_writer->AddDataItem2D(m_pars.prefix, aData.dimensions[1], aData.dimensions[0]);

			//by default is COPY			
			if(m_options.at(1) == "NO_COPY" )
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Memory Mode = NO_COPY";
				m_writer->SetDataItemMemoryMode(m_pars.prefix, nxcpp::DataStreamer::MemoryMode::NO_COPY);
			}
			else
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Memory Mode = COPY";
				m_writer->SetDataItemMemoryMode(m_pars.prefix, nxcpp::DataStreamer::MemoryMode::COPY);
			}

			//Set sensors node's name
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - Set sensors node's name";
			m_writer->SetDataItemNodeName(m_pars.prefix, m_pars.prefix);

			DEB_TRACE() << "Create/Initialize DataStreamer : " << t.elapsed_msec() << " ms\n";
		}

		//write data in Nexus file
		DEB_TRACE() << "SaveContainerNxs::_writeFile() - PushData(type = " << m_pars.imageType << " ]" ;
		t.restart();
		switch(m_pars.imageType)
		{
			case Bpp8:
				//push data into file
				m_writer->PushData( m_pars.prefix, (unsigned char*) (aData.data()));
				break;
			case Bpp16:
				//push data into file
				m_writer->PushData( m_pars.prefix, (unsigned short*) (aData.data()));
				break;
			case Bpp32:
				//push data into file
				m_writer->PushData( m_pars.prefix, (unsigned int*) (aData.data()));
				break;
			case Bpp32F:
				//push data into file
				m_writer->PushData( m_pars.prefix, (float*) (aData.data()));
				break;
			default:  //UINT16 by default
				//push data into file
				m_writer->PushData( m_pars.prefix, (unsigned short*) (aData.data()));
				break;
		}
		DEB_TRACE() << "Push Data into DataStreamer : " << t.elapsed_msec() << " ms";
		//- Display Nexus statistics
		nxcpp::DataStreamer::Statistics nxsStats;

		nxsStats = m_writer->GetStatistics();
		DEB_TRACE() << "Nexus Writer Statistics : ";
		DEB_TRACE() << "\t- WrittenBytes = "			<< nxsStats.ui64WrittenBytes;
		DEB_TRACE() << "\t- PendingBytes = "			<< nxsStats.ui64PendingBytes;
		DEB_TRACE() << "\t- MaxPendingBytes = "         << nxsStats.ui64MaxPendingBytes;
		DEB_TRACE() << "\t- TotalBytes = "				<< nxsStats.ui64TotalBytes;
		DEB_TRACE() << "\t- ActiveWriters = "			<< nxsStats.ui16ActiveWriters;
		DEB_TRACE() << "\t- MaxSimultaneousWriters = "	<< nxsStats.ui16MaxSimultaneousWriters;
		DEB_TRACE() << "\t- fInstantMbPerSec = "		<< nxsStats.fInstantMbPerSec;
		DEB_TRACE() << "\t- fPeakMbPerSec = "			<< nxsStats.fPeakMbPerSec;
		DEB_TRACE() << "\t- fAverageMbPerSec = "		<< nxsStats.fAverageMbPerSec;

		//destroy Nexus object : to do once for each new sequence at the last image
		if( (aData.frameNumber + 1) == (m_pars.nbframes))
		{
			//Finalize
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - Finalize() the writer";
			t.restart();
			m_writer->Finalize();
			DEB_TRACE() << "Finalize the DataStreamer : " << t.elapsed_msec() << " ms";
			//destroy object
			DEB_TRACE() << "SaveContainerNxs::_writeFile() - delete the writer";
			t.restart();
			delete m_writer;
			m_writer = 0;
			DEB_TRACE() << "Delete the DataStreamer : " << t.elapsed_msec() << " ms";
		}
	}
	catch (std::bad_alloc& ex)
	{
		DEB_TRACE() << "Bad alloc exception: " << ex.what();
		THROW_CTL_ERROR(Error) << "Bad alloc exception: " << ex.what();
	}
	catch(yat::Exception& ex)
	{
		DEB_TRACE() << "SaveContainerNxs::_writeFile() - catch NexusException";
		std::stringstream my_error;
		my_error.str("");
		for(unsigned i = 0; i < ex.errors.size(); i++)
		{
			my_error << ex.errors[i].desc;
		}
		DEB_TRACE() << my_error.str();
		THROW_CTL_ERROR(Error) << my_error.str();
	}
	catch (std::exception& ex)
	{
		DEB_TRACE() << "SaveContainerNxs::_writeFile() - catch exception";
		THROW_CTL_ERROR(Error) << "Standard exception: " << ex.what();
	}
	catch(...)
	{
		DEB_TRACE() << "SaveContainerNxs::_writeFile() - catch UNKNOWN Exception";
		THROW_CTL_ERROR(Error) << "SaveContainerNxs::_writeFile() - catch UNKNOWN Exception";
	}
}

//--------------------------------------------------------------------------------------------------------------------
