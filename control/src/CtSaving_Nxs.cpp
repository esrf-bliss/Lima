#include <sys/time.h>
#include <cstdio>
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
bool SaveContainerNxs::_open(const std::string &filename, std::_Ios_Openmode openFlags)
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
		  cout<<"SaveContainerNxs::_writeFile() aData.frameNumber = "<<aData.frameNumber<<endl;
		  //that's mean that snap was stopped previous by user command or device was hang
		  //so me must clean the N4T object
		  if(m_writer && aData.frameNumber==0)
		  {
			cout<<"SaveContainerNxs::_writeFile() - Abort() current Nexus writer"<<endl;
			//Abort every current task && go away
			m_writer->Abort();
			
			//destroy object
			cout<<"SaveContainerNxs::_writeFile() - delete BufferedData1D()"<<endl;
			delete m_writer;
			m_writer = 0;			
		  }
		  
		  //prepare nexus object : to do once for each new sequence
		  if(!m_writer)
		  {
			//get acquisition parameters
			getParameters(m_pars);
			
			//create N4T main object needed to generate Nexus file
			cout<<"SaveContainerNxs::_writeFile() - new BufferedData1D()"<<endl;
			m_writer = new n4t::BufferedData1D(m_pars.prefix, m_pars.nbframes,m_pars.framesPerFile);	  
						
			m_writer->Initialize(m_pars.directory, "");

			
			//Add sensor 2D (image) // height,width
			m_writer->AddDataItem2D(m_pars.prefix, aData.dimensions[1],aData.dimensions[0]);

			//Set sensors node's name
			m_writer->SetDataItemNodeName(m_pars.prefix, m_pars.prefix);
		  }
		  
		  //write data in Nexys file
		  cout<<"SaveContainerNxs::_writeFile() - PushData()"<<endl;
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

//@@@@@@@@@@@@@@@@@

		  n4t::BufferedData::Statistics nxsStats;
		  nxsStats = m_writer->GetStatistics();

		  cout<<"WrittenBytes = "			<<nxsStats.ui64WrittenBytes<<endl;
		  cout<<"PendingBytes = "			<<nxsStats.ui64PendingBytes<<endl;
		  cout<<"MaxPendingBytes = "		<<nxsStats.ui64MaxPendingBytes<<endl;
		  cout<<"TotalBytes = "				<<nxsStats.ui64TotalBytes<<endl;
		  cout<<"ActiveWriters = "			<<nxsStats.ui16ActiveWriters<<endl;
		  cout<<"MaxSimultaneousWriters = "	<<nxsStats.ui16MaxSimultaneousWriters<<endl;
		  cout<<"fInstantMbPerSec = "		<<nxsStats.fInstantMbPerSec<<endl;
		  cout<<"fPeakMbPerSec = "			<<nxsStats.fPeakMbPerSec<<endl;
		  cout<<"fAverageMbPerSec = "		<<nxsStats.fAverageMbPerSec<<endl;

///////////////////

		  //destroy Nexus object : to do once for each new sequence at the last image
		  if( (aData.frameNumber+1) == (m_pars.nbframes))
		  {
			//Finalize
			cout<<"SaveContainerNxs::_writeFile() - Finalize()"<<endl;
			m_writer->Finalize();

			//destroy object
			cout<<"SaveContainerNxs::_writeFile() - delete BufferedData1D()"<<endl;
			delete m_writer;
			m_writer = 0;
		  }
	  }
	  catch(yat::Exception& ex)
	  {
		  cout<<"SaveContainerNxs::_writeFile() - catch NexusException"<<endl;
		  std::stringstream my_error;
		  my_error.str("");
		  for(unsigned i = 0; i < ex.errors.size(); i++)
		  {
			  my_error<<ex.errors[i].desc<<endl;
		  }
		  std::cout<<my_error.str()<<std::endl;
		  throw LIMA_CTL_EXC(Error,my_error.str());
	  }
	  catch(...)
	  {
		  cout<<"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception"<<endl;
		  throw LIMA_CTL_EXC(Error,"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception");
	  }
}

//--------------------------------------------------------------------------------------------------------------------
