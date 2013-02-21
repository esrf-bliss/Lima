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
# include <sys/time.h>
#else
# include <sys/timeb.h>
# include <time.h>
#endif

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
		  DEB_TRACE()<<"SaveContainerNxs::_writeFile() aData.frameNumber = "<<aData.frameNumber;
		  //that's mean that snap was stopped previous by user command or device was hang
		  //so me must clean the N4T object
		  if(m_writer && aData.frameNumber==0)
		  {
			DEB_TRACE()<<"SaveContainerNxs::_writeFile() - Abort() current Nexus writer";
			//Abort every current task && go away
			m_writer->Abort();
			
			//destroy object
			DEB_TRACE()<<"SaveContainerNxs::_writeFile() - delete the writer()";
			delete m_writer;
			m_writer = 0;			
		  }
		  
		  //prepare nexus object : to do once for each new sequence
		  if(!m_writer)
		  {
			//get acquisition parameters
			getParameters(m_pars);
			
			//create N4T main object needed to generate Nexus file
			DEB_TRACE()<<"SaveContainerNxs::_writeFile() - create the writer";
#ifdef __unix            
			m_writer = new n4t::BufferedData1D(m_pars.prefix, m_pars.nbframes,m_pars.framesPerFile);	  
#else
            m_writer = new nx::DataStreamer(m_pars.prefix, m_pars.nbframes,m_pars.framesPerFile);
#endif            
						
			m_writer->Initialize(m_pars.directory, "");

			
			//Add sensor 2D (image) // height,width
			m_writer->AddDataItem2D(m_pars.prefix, aData.dimensions[1],aData.dimensions[0]);

			//Set sensors node's name
			m_writer->SetDataItemNodeName(m_pars.prefix, m_pars.prefix);
		  }
		  
		  //write data in Nexys file
		  DEB_TRACE()<<"SaveContainerNxs::_writeFile() - PushData()";
		  switch(m_pars.imageType)
		  {
		    case Bpp8:
		    //push data into file
		    m_writer->PushData( m_pars.prefix, (unsigned char*)(aData.data()));
		    break;
		    case Bpp32:
		    //push data into file
		    m_writer->PushData( m_pars.prefix, (unsigned int*)(aData.data()));
		    break;
		    default:  //UINT16 by default
		    //push data into file
		    m_writer->PushData( m_pars.prefix, (unsigned short*)(aData.data()));
		    break;
	    }


          //- Display Nexus statistics
#ifdef __unix  
		  n4t::BufferedData::Statistics nxsStats;
#else
		  nx::DataStreamer::Statistics nxsStats;
#endif
		  nxsStats = m_writer->GetStatistics();

		  DEB_TRACE()<<"WrittenBytes = "			<<nxsStats.ui64WrittenBytes;
		  DEB_TRACE()<<"PendingBytes = "			<<nxsStats.ui64PendingBytes;
		  DEB_TRACE()<<"MaxPendingBytes = "         <<nxsStats.ui64MaxPendingBytes;
		  DEB_TRACE()<<"TotalBytes = "				<<nxsStats.ui64TotalBytes;
		  DEB_TRACE()<<"ActiveWriters = "			<<nxsStats.ui16ActiveWriters;
		  DEB_TRACE()<<"MaxSimultaneousWriters = "	<<nxsStats.ui16MaxSimultaneousWriters;
		  DEB_TRACE()<<"fInstantMbPerSec = "		<<nxsStats.fInstantMbPerSec;
		  DEB_TRACE()<<"fPeakMbPerSec = "			<<nxsStats.fPeakMbPerSec;
		  DEB_TRACE()<<"fAverageMbPerSec = "		<<nxsStats.fAverageMbPerSec;

		  //destroy Nexus object : to do once for each new sequence at the last image
		  if( (aData.frameNumber+1) == (m_pars.nbframes))
		  {
			//Finalize
			DEB_TRACE()<<"SaveContainerNxs::_writeFile() - Finalize()";
			m_writer->Finalize();

			//destroy object
			DEB_TRACE()<<"SaveContainerNxs::_writeFile() - delete the writer";
			delete m_writer;
			m_writer = 0;
		  }
	  }
	  catch(yat::Exception& ex)
	  {
		  DEB_TRACE()<<"SaveContainerNxs::_writeFile() - catch NexusException";
		  std::stringstream my_error;
		  my_error.str("");
		  for(unsigned i = 0; i < ex.errors.size(); i++)
		  {
			  my_error<<ex.errors[i].desc;
		  }
		  DEB_TRACE()<<my_error.str();
		  throw LIMA_CTL_EXC(Error,my_error.str());
	  }
	  catch(...)
	  {
		  DEB_TRACE()<<"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception";
		  throw LIMA_CTL_EXC(Error,"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception");
	  }
}

//--------------------------------------------------------------------------------------------------------------------
