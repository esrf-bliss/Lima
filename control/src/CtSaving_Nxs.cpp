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
            DEB_TRACE()<<"m_pars.options = "<<m_pars.options;
			std::stringstream ss(m_pars.options);
			std::string field;
			m_options.clear();
			while (getline(ss, field, '|'))
				m_options.push_back(field);
			////////////////////////////////////////////////////////////////////////	

            // ensure that all options are here
            if(m_options.size() != NEXUS_SAVING_OPTIONS_NUMBER)
            {
                DEB_TRACE() << "Bad Nexus Saving options number ! ("<<m_options.size()<<")";
                THROW_CTL_ERROR(Error) << "Bad Nexus Saving options number ! ("<<m_options.size()<<")";
            }
            
            // ask if timestamp is enabled            
            if(m_options.at(2) == "TRUE" )
            {
                //Add sensor 0D (scalar) 
                DEB_TRACE() << "SaveContainerNxs::_writeFile() - Add sensor 0D (scalar)";
                m_writer->AddDataItem0D(m_pars.prefix+"_timestamp");
            }

            //Add sensor 2D (image) // height,width
            DEB_TRACE() << "SaveContainerNxs::_writeFile() - Add sensor 2D (image)";
            m_writer->AddDataItem2D(m_pars.prefix+"_image", aData.dimensions[1], aData.dimensions[0]);

            // configure the Writer mode 
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

            // configure the Memory mode 
			//by default is COPY			
			if(m_options.at(1) == "NO_COPY" )
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Memory Mode = NO_COPY";
				m_writer->SetDataItemMemoryMode(m_pars.prefix+"_image", nxcpp::DataStreamer::MemoryMode::NO_COPY);
			}
			else
			{
				DEB_TRACE() << "SaveContainerNxs::_writeFile() - Memory Mode = COPY";
				m_writer->SetDataItemMemoryMode(m_pars.prefix+"_image", nxcpp::DataStreamer::MemoryMode::COPY);
			}

            DEB_TRACE() << "Create/Initialize DataStreamer : " << t.elapsed_msec() << " ms\n";
        }

        ////////////////////////////////////////////////////////////////////////
        // push (timestamp & Data))into File 
        ////////////////////////////////////////////////////////////////////////

        // ask if timestamp is enabled
        if(m_options.at(2) == "TRUE" )
        {
            //push timestamp in Nexus file
            DEB_TRACE() << "SaveContainerNxs::_writeFile() - PushData(timestamp)" ;
            m_writer->PushData( m_pars.prefix+"_timestamp", (double*) (&aData.timestamp));
		}

        DEB_TRACE() << "SaveContainerNxs::_writeFile() - PushData(Data [type = "<< aData.type <<"])" ;
        t.restart();        
		//write data in Nexus file
        switch(aData.type)
		{
            case Data::UINT8:
				m_writer->PushData( m_pars.prefix+"_image", (unsigned char*) (aData.data()));
				break;
            case Data::INT8:
                m_writer->PushData( m_pars.prefix+"_image", (char*) (aData.data()));
                break;
            case Data::UINT16:
				m_writer->PushData( m_pars.prefix+"_image", (unsigned short*) (aData.data()));
				break;
            case Data::INT16:
                m_writer->PushData( m_pars.prefix+"_image", (short*) (aData.data()));
                break;
            case Data::UINT32: 
				m_writer->PushData( m_pars.prefix+"_image", (unsigned int*) (aData.data()));
				break;
            case Data::INT32:
                m_writer->PushData( m_pars.prefix+"_image", (int*) (aData.data()));
                break;     
            case Data::UINT64:
                m_writer->PushData( m_pars.prefix+"_image", (unsigned long long*) (aData.data()));
                break;    
            case Data::INT64:
                m_writer->PushData( m_pars.prefix+"_image", (long long*) (aData.data()));
                break;    
            case Data::FLOAT: 	
				m_writer->PushData( m_pars.prefix+"_image", (float*) (aData.data()));
				break;
            case Data::DOUBLE:
                m_writer->PushData( m_pars.prefix+"_image", (double*) (aData.data()));
				break;
            default: 
                THROW_CTL_ERROR(Error) << "SaveContainerNxs::_writeFile() - This Data [type = "<< aData.type <<"] is not managed !";
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
