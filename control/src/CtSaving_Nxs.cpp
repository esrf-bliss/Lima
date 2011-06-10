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
SaveContainerNxs::SaveContainerNxs(CtSaving &aCtSaving)	: CtSaving::SaveContainer(aCtSaving)
{
  DEB_CONSTRUCTOR();
  cout<<"SaveContainerNxs::SaveContainerNxs()"<<endl;
  m_writer = 0;
}

//--------------------------------------------------------------------------------------------------------------------
//- dtor
//--------------------------------------------------------------------------------------------------------------------
SaveContainerNxs::~SaveContainerNxs()
{
  DEB_DESTRUCTOR();
  cout<<"SaveContainerNxs::~SaveContainerNxs()"<<endl;
}

//--------------------------------------------------------------------------------------------------------------------
//- Event rising by CtSaving when frame is acquired (newFrameReady)
//--------------------------------------------------------------------------------------------------------------------
bool SaveContainerNxs::_open(const std::string &filename, std::_Ios_Openmode openFlags)
{
  DEB_MEMBER_FUNCT();
  cout<<"SaveContainerNxs::_open"<<endl;
  return true;
}

//--------------------------------------------------------------------------------------------------------------------
//- Event rising by CtSaving when frame is saved
//--------------------------------------------------------------------------------------------------------------------
void SaveContainerNxs::_close()
{
  DEB_MEMBER_FUNCT();
  cout<<"SaveContainerNxs::_close()"<<endl;
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
	cout<<"SaveContainerNxs::_writeFile()"<<endl;

	  try
	  {
		  //that's mean that snap was stopped previous by user command or device was hang
		  //so me must clean the N4T object
		  if(m_writer && aData.frameNumber==0)
		  {
			cout<<"SaveContainerNxs::_writeFile() - Abort current Nexus writer"<<endl;
			//Abort every current task && go away
			m_writer->Abort();
			
			//destroy object
			delete m_writer;
			m_writer = 0;			
		  }
		  
		  //prepare nexus object : to do once for each new sequence
		  if(!m_writer)
		  {
			//get acquisition parameters
			getParameters(m_pars);
			
			//create N4T main object needed to generate Nexus file
			m_writer = new Nexus4Tango::BufferedData1D(m_pars.prefix, m_pars.nbframes,m_pars.framesPerFile);	  
			m_writer->Initialize(m_pars.temporaryPath, m_pars.directory);

			//Add sensor 2D (image)
			m_writer->AddDataItem2D(m_pars.prefix, aData.dimensions[1],aData.dimensions[0]);

			//Set sensors' path
			m_writer->SetPath(m_pars.prefix, "scan_data");
		  
			//Set sensors node's name
			m_writer->SetDataItemNodeName(m_pars.prefix, m_pars.prefix);
		  }
		  
		  //push data into file
		  m_writer->PushData( m_pars.prefix, (unsigned short*)(aData.data()));
	  
		  //destroy Nexus object : to do once for each new sequence at the last image
		  if( (aData.frameNumber+1) == (m_pars.nbframes))
		  {
			//Finalize
			m_writer->Finalize();

			//destroy object
			delete m_writer;
			m_writer = 0;
		  }
	  }
	  catch(NexusException &n)
	  {
		  cout<<"SaveContainerNxs::_writeFile() - catch NexusException"<<endl;
		  cout<<n.Reason()<<endl;
		  throw LIMA_CTL_EXC(Error,n.Reason());
	  }
	  catch(...)
	  {
		  cout<<"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception"<<endl;
		  throw LIMA_CTL_EXC(Error,"SaveContainerNxs::_writeFile() - catch UNKNOWN Exception");
	  }
}

//--------------------------------------------------------------------------------------------------------------------
