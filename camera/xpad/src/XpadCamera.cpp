#include "XpadCamera.h"
#include <sstream>
#include <iostream>
#include <string>
#include <math.h>

using namespace lima;
using namespace std;


//- Const.
static const int 	BOARDNUM 	= 0;


//---------------------------
//- Ctor
//---------------------------
XpadCamera::XpadCamera(): 	m_buffer_cb_mgr(m_buffer_alloc_mgr),
							m_buffer_ctrl_mgr(m_buffer_cb_mgr),
							m_modules_mask(0x00),
							m_chip_number(7),
							m_exp_time(1000),
							m_pixel_depth(B4),
							m_trigger_type(INTERN_GATE),
							m_nb_frames(1)
{
	DEB_CONSTRUCTOR();
	m_status = XpadCamera::Ready;

	//- Hardcoded temporarly
	m_time_unit				= MILLISEC_GATE; // 1=MICROSEC_GATE; 2=MILLISEC_GATE; 3=SECONDS_GATE
	m_pixel_depth = B4;

	//- FParameters
	m_fparameter_deadtime		= 0;
	m_fparameter_init		= 0;
	m_fparameter_shutter		= 0;
	m_fparameter_ovf		= 0;
	m_fparameter_mode		= 0;
	m_fparameter_n			= 0;
	m_fparameter_p			= 0;
	m_fparameter_GP1		= 0;
	m_fparameter_GP2		= 0;
	m_fparameter_GP3		= 0;
	m_fparameter_GP4		= 0;

	m_acquisition_type		= 2; // Slow B4
	m_video_mode 			= false;

	
	//-------------------------------------------------------------

	//Reset of the PICExpress
	//- Removed because not used in the example program: DAQ_FRED and xpad_simple
	/*if(xpci_resetBoard(BOARDNUM) == 0)
	{
		DEB_TRACE() << "PCIe hardware reset is OK";
	}
	else
	{
		DEB_ERROR() << "PCIe hardware reset has FAILED:" ;
		throw LIMA_HW_EXC(Error, "Error in PCIe hardware reset!");
	}*/

	//-------------------------------------------------------------
	//- Get Modules that are ready
	if (xpci_modAskReady(&m_modules_mask) == 0)
	{
		DEB_TRACE() << "Ask modules that are ready: OK (modules mask = " << std::hex << m_modules_mask << ")" ;
		m_module_number = xpci_getModNb(m_modules_mask);
		if (m_module_number !=0)
		{
			DEB_TRACE() << "--> Number of Modules 		 = " << m_module_number ;			
		}
		else
		{
			DEB_ERROR() << "No modules found: retry to Init" ;
			//- Test if PCIe is OK
			if(xpci_isPCIeOK() == 0) 
			{
				DEB_TRACE() << "PCIe hardware check is OK" ;
			}
			else
			{
				DEB_ERROR() << "PCIe hardware check has FAILED:" ;
				DEB_ERROR() << "1. Check if green led is ON (if not go to p.3)" ;
				DEB_ERROR() << "2. Reset PCIe board" ;
				DEB_ERROR() << "3. Power off and power on PC (do not reboot, power has to be cut off)\n" ;
				throw LIMA_HW_EXC(Error, "PCIe hardware check has FAILED!");
			}
			throw LIMA_HW_EXC(Error, "No modules found: retry to Init");			
		}	
	}
	else
	{
		DEB_ERROR() << "Ask modules that are ready: FAILED" ;
		throw LIMA_HW_EXC(Error, "No Modules are ready");
	}


	//ATTENTION: We consider that image size is with always 8 modules ! 
	m_image_size = Size(80 * m_chip_number ,120 * 8);
	DEB_TRACE() << "--> Number of chips 		 = " << std::dec << m_chip_number ;
	DEB_TRACE() << "--> Image width 	(pixels) = " << std::dec << m_image_size.getWidth() ;
	DEB_TRACE() << "--> Image height	(pixels) = " << std::dec << m_image_size.getHeight() ;
}

//---------------------------
//- Dtor
//---------------------------
XpadCamera::~XpadCamera()
{
	DEB_DESTRUCTOR();
}

//---------------------------
//- XpadCamera::start()
//---------------------------
void XpadCamera::start()
{
	DEB_MEMBER_FUNCT();

	//-	((80 colonnes * 7 chips) * taille du pixel + 6 word de control ) * 120 lignes * nb_modules
	if (m_pixel_depth == B2)
	{
		m_full_image_size_in_bytes = ((80 * m_chip_number) * 2 + 6*2)  * 120 * m_module_number;
	} 
	else if(m_pixel_depth == B4)
	{
		m_full_image_size_in_bytes = ((80 * m_chip_number) * 4 + 6*2)  * 120 * m_module_number;
	} 

	m_stop_asked = false;

	DEB_TRACE() << "m_acquisition_type = " << m_acquisition_type ;

	if(m_acquisition_type == 0)
		 //- Post XPAD_DLL_START_SLOW_MSG msg (aka getOneImage)
		this->post(new yat::Message(XPAD_DLL_START_SLOW_B2_MSG), kPOST_MSG_TMO);
	else if (m_acquisition_type == 1)
		//- Post XPAD_DLL_START_FAST_MSG msg (aka getImgSeq)
		this->post(new yat::Message(XPAD_DLL_START_FAST_MSG), kPOST_MSG_TMO);
	else if (m_acquisition_type == 2)
		//- Post XPAD_DLL_START_SLOW_MSG_B4 msg (aka getOneImage)
		this->post(new yat::Message(XPAD_DLL_START_SLOW_B4_MSG), kPOST_MSG_TMO);
	else
	{
		DEB_ERROR() << "Acquisition type not supported" ;
		throw LIMA_HW_EXC(Error, "Acquisition type not supported");
	}
}

//---------------------------
//- XpadCamera::stop()
//---------------------------
void XpadCamera::stop()
{
	DEB_MEMBER_FUNCT();

	m_status = XpadCamera::Ready;
	m_stop_asked = true;
}

//-----------------------------------------------------
//- XpadCamera::getImageSize(Size& size)
//-----------------------------------------------------
void XpadCamera::getImageSize(Size& size)
{
	DEB_MEMBER_FUNCT();

	size = m_image_size;
}

//-----------------------------------------------------
//- XpadCamera::getPixelSize(double& size)
//-----------------------------------------------------
void XpadCamera::getPixelSize(double& size)
{
	DEB_MEMBER_FUNCT();
	size = 130; // pixel size is 130 micron
}

//-----------------------------------------------------
//- XpadCamera::setPixelDepth(ImageType pixel_depth)
//-----------------------------------------------------
void XpadCamera::setPixelDepth(ImageType pixel_depth)
{
	DEB_MEMBER_FUNCT();
	switch( pixel_depth )
	{
		case Bpp16:
			m_pixel_depth = B2;
		break;
	
		case Bpp32:
			m_pixel_depth = B4;
		break;
		default:
			DEB_ERROR() << "Pixel Depth is unsupported: only 16 or 32 bits is supported" ;
			throw LIMA_HW_EXC(Error, "Pixel Depth is unsupported: only 16 or 32 bits is supported");
		break;
	}
}

//-----------------------------------------------------
//- XpadCamera::getPixelDepth(ImageType& pixel_depth)
//-----------------------------------------------------
void XpadCamera::getPixelDepth(ImageType& pixel_depth)
{
	DEB_MEMBER_FUNCT();
	switch( m_pixel_depth )
	{
		case B2:
			pixel_depth = Bpp16;
		break;
	
		case B4:
			pixel_depth = Bpp32;
		break;	
	}
}

//-----------------------------------------------------
//- XpadCamera::getDetectorType(string& type)
//-----------------------------------------------------
void XpadCamera::getDetectorType(string& type)
{
	DEB_MEMBER_FUNCT();
	type = "XPAD";
}

//-----------------------------------------------------
//- XpadCamera::getDetectorModel(string& type)
//-----------------------------------------------------
void XpadCamera::getDetectorModel(string& type)
{
	DEB_MEMBER_FUNCT();
	type = "PCIe-3.2";	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setMaxImageSizeCallbackActive(bool cb_active)
{  
	m_mis_cb_act = cb_active;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BufferCtrlMgr& XpadCamera::getBufferMgr()
{
	return m_buffer_ctrl_mgr;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

	switch( mode )
	{
		case IntTrig:
			m_trigger_type = INTERN_GATE;
		break;
		case ExtTrigSingle:
			m_trigger_type = 10; //- EXTERN_TRIG (eg TIMED)
		break;
		case ExtGate:
			m_trigger_type = EXTERN_GATE;
		break;
		default:
			DEB_ERROR() << "Error: Trigger mode unsupported: only INTERN_GATE or EXTERN_GATE" ;
			throw LIMA_HW_EXC(Error, "Trigger mode unsupported: only INTERN_GATE or EXTERN_GATE");
		break;
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::getTrigMode(TrigMode& mode)
{

	DEB_MEMBER_FUNCT();
	switch( m_trigger_type )
	{
		case INTERN_GATE:
			mode = IntTrig;
		break;
		case 10: //- EXTERN_TRIG (eg TIMED)
			mode = ExtTrigSingle;
		break;
		case EXTERN_GATE:
			mode = ExtGate;
		break;
		default:
			DEB_ERROR() << "Error: Trigger mode unsupported: only INTERN_GATE or EXTERN_GATE" ;
			throw LIMA_HW_EXC(Error, "Trigger mode unsupported: only INTERN_GATE or EXTERN_GATE");
		break;
	}
    	DEB_RETURN() << DEB_VAR1(mode);
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setExpTime(double exp_time_sec)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(exp_time_sec);

	//- 1=MICROSEC_GATE; 2=MILLISEC_GATE; 3=SECONDS_GATE
	switch(m_time_unit)
	{
		case MICROSEC_GATE:
		m_exp_time = (unsigned) (exp_time_sec * 1000000);
		break;

		case MILLISEC_GATE:
		m_exp_time = (unsigned) (exp_time_sec * 1000);
		break;

		case SECONDS_GATE:
		m_exp_time = (unsigned) (exp_time_sec * 1);
		break;
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::getExpTime(double& exp_time_sec)
{
	DEB_MEMBER_FUNCT();

	//- 1=MICROSEC_GATE; 2=MILLISEC_GATE; 3=SECONDS_GATE
	switch(m_time_unit)
	{
		case MICROSEC_GATE:
		exp_time_sec = m_exp_time / 1000000;
		break;

		case MILLISEC_GATE:
		exp_time_sec = m_exp_time / 1000;
		break;

		case SECONDS_GATE:
		exp_time_sec = m_exp_time / 1;
		break;
	}
	
	DEB_RETURN() << DEB_VAR1(exp_time_sec);
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);
	m_nb_frames = nb_frames;
	if(m_nb_frames == 0) //- Video mode
	{
		m_nb_frames = 1;
		m_video_mode = true;
	}
	else
	{
		m_video_mode = false;
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	nb_frames = m_nb_frames;
	DEB_RETURN() << DEB_VAR1(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::getStatus(XpadCamera::Status& status)
{
	DEB_MEMBER_FUNCT();
	status = m_status;
	DEB_RETURN() << DEB_VAR1(DEB_HEX(status));
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::handle_message( yat::Message& msg )  throw( yat::Exception )
{
	DEB_MEMBER_FUNCT();
  try
  {
    switch ( msg.type() )
    {
      //-----------------------------------------------------	
      case yat::TASK_INIT:
      {
        DEB_TRACE() <<"XpadCamera::->TASK_INIT";          
      }
      break;
      //-----------------------------------------------------    
      case yat::TASK_EXIT:
      {
        DEB_TRACE() <<"XpadCamera::->TASK_EXIT";                
      }
      break;
      //-----------------------------------------------------    
      case yat::TASK_TIMEOUT:
      {
		DEB_TRACE() <<"XpadCamera::->TASK_TIMEOUT";       
      }
      break;
	  //-----------------------------------------------------    
	  case XPAD_DLL_START_SLOW_B2_MSG:
      {
		DEB_TRACE() <<"XpadCamera::->XPAD_DLL_START_SLOW_B2_MSG";       

		m_status = XpadCamera::Exposure;
		
		for( int i=0 ; i < m_nb_frames ; i++ )
		{
			if (m_stop_asked == true)
			{
				DEB_TRACE() <<"Stop asked: exit without allocating new images..." ;
				m_status = XpadCamera::Ready;
				return;
			}
			

			pOneImage = new uint16_t[ m_full_image_size_in_bytes / 2 ]; //- Divided by 2 because of uint16
			
			if (xpci_getOneImage(	m_pixel_depth,
									m_modules_mask,
									m_chip_number,
									(uint16_t *)pOneImage,
									m_trigger_type,
									m_exp_time,
									m_time_unit,
									30000)==-1)
			{
				DEB_ERROR()<< "Error: readOneImage as returned an error..." ;
			}

			m_status = XpadCamera::Readout;

			DEB_TRACE() <<"Image# "<< i << " acquired" ;

			//- clean the image and call new frame for each frame
			StdBufferCbMgr& buffer_mgr = m_buffer_cb_mgr;
			DEB_TRACE() <<"-> clean acquired image and publish it through newFrameReady()";

			int buffer_nb, concat_frame_nb;
			buffer_mgr.setStartTimestamp(Timestamp::now());
			buffer_mgr.acqFrameNb2BufferNb(i, buffer_nb, concat_frame_nb);

			uint16_t *ptr = (uint16_t*)(buffer_mgr.getBufferPtr(buffer_nb,concat_frame_nb));
			//clean the ptr with zero memory, pixels of a not available module are set to "0" 
			memset((uint16_t *)ptr,0,m_image_size.getWidth() * m_image_size.getHeight() * 2);
				
			//iterate on all lines of all modules returned by xpix API 
			int k=0;
			for(int j = 0; j < 120 * m_module_number; j++) 
			{
				uint16_t	OneLine[6+80*m_chip_number];
				
				//copy entire line with its header and footer
				for(k = 0; k < (6+80*m_chip_number); k++)
					OneLine[k] = pOneImage[j*(6+80*m_chip_number)+k];
				
				//compute "offset line" where to copy OneLine[] in the *ptr, to ensure that the lines are copied in order of modules
				int offset = ((120*(OneLine[1]-1))+(OneLine[4]-1)); 
				
				//copy cleaned line in the lima buffer
				for(k = 0; k < (80*m_chip_number); k++)
					ptr[(offset*80*m_chip_number)+k] = OneLine[5+k];
			}
				
			DEB_TRACE() << "image# " << i <<" cleaned" ;
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = i;
			//- raise the image to lima
			buffer_mgr.newFrameReady(frame_info);

			DEB_TRACE() <<"free image pointer";
			delete[] pOneImage;
		}

		m_status = XpadCamera::Ready;
      	}
      	break;
	//-----------------------------------------------------    
	case XPAD_DLL_START_SLOW_B4_MSG:
      	{
		DEB_TRACE() <<"XpadCamera::->XPAD_DLL_START_SLOW_B4_MSG";       

		m_status = XpadCamera::Exposure;
		
		for( int i=0 ; i < m_nb_frames ;  )
		{
			if (m_video_mode == true)
			{
				//- Re init i
				i = 0;
			}
			if (m_stop_asked == true)
			{
				DEB_TRACE() <<"Stop asked: exit without allocating new images..." ;
				m_status = XpadCamera::Ready;
				return;
			}
			
			pOneImage = new uint16_t[ m_full_image_size_in_bytes/2 ];
			
			if (xpci_getOneImage(	m_pixel_depth,
									m_modules_mask,
									m_chip_number,
									(uint16_t *)pOneImage,
									m_trigger_type,
									m_exp_time,
									m_time_unit,
									30000)==-1)
			{
				DEB_ERROR()<< "Error: readOneImage as returned an error..." ;
			}

			m_status = XpadCamera::Readout;

			DEB_TRACE() <<"Image# "<< i << " acquired" ;

			//- clean the image and call new frame for each frame
			StdBufferCbMgr& buffer_mgr = m_buffer_cb_mgr;
			DEB_TRACE() <<"-> clean acquired image and publish it through newFrameReady()";

			int buffer_nb, concat_frame_nb;
			buffer_mgr.setStartTimestamp(Timestamp::now());
			buffer_mgr.acqFrameNb2BufferNb(i, buffer_nb, concat_frame_nb);

			uint32_t *ptr = (uint32_t*)(buffer_mgr.getBufferPtr(buffer_nb,concat_frame_nb));
			//clean the ptr with zero memory, pixels of a not available module are set to "0" 
			memset((uint32_t *)ptr,0,m_image_size.getWidth() * m_image_size.getHeight() * 4);

			//iterate on all lines of all modules returned by xpix API 
			int k=0;
			for(int j = 0; j < 120 * m_module_number; j++) 
			{
				uint16_t	OneLine[6+(80*m_chip_number)*2];
				uint32_t	OneLinePix[80*m_chip_number];
				
				//copy entire line with its header and footer
				for(k = 0; k < (6+(80*m_chip_number)*2); k++)
					OneLine[k] = pOneImage[j*(6+(80*m_chip_number)*2)+k];

				//copy entire line without header&footer into OneLinePix
				memcpy((uint32_t *)OneLinePix,(uint16_t *)(&OneLine[5]),80*m_chip_number*4);

				//compute "offset line" where to copy OneLine[] in the *ptr, to ensure that the lines are copied in order of modules
				int offset = ((120*(OneLine[1]-1))+(OneLine[4]-1)); 

				//copy cleaned line in the lima buffer
				for(k = 0; k < (80*m_chip_number); k++)
					ptr[(offset*80*m_chip_number)+k] = OneLinePix[k];
			}
				
			DEB_TRACE() << "Image# " << i <<" cleaned" ;
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = i;

			//- raise the image to lima
			buffer_mgr.newFrameReady(frame_info);

			DEB_TRACE() <<"free image pointer";
			delete[] pOneImage;

			i++;
		}

		m_status = XpadCamera::Ready;
      }
      break;

      //-----------------------------------------------------    
      case XPAD_DLL_START_FAST_MSG:	
      {
		DEB_TRACE() <<"XpadCamera::->XPAD_DLL_START_FAST_MSG";
	
		//- A mettre dans le prepareAcq?
		// allocate multiple buffers
		DEB_TRACE() <<"allocating images array (" << m_nb_frames << " images)";
		pSeqImage = new uint16_t* [ m_nb_frames ];

		DEB_TRACE() <<"allocating every image pointer of the images array (1 image full size = "<< m_full_image_size_in_bytes << ") ";
		for( int i=0 ; i < m_nb_frames ; i++ )
			pSeqImage[i] = new uint16_t[ m_full_image_size_in_bytes / 2 ];

		m_status = XpadCamera::Exposure;
	
		//- Start the img sequence
		DEB_TRACE() <<"start acquiring a sequence of images";	
		cout << " ============================================== " << endl;
		cout << "Parametres pour getImgSeq: "<< endl;
		cout << "m_pixel_depth 	= "<< m_pixel_depth << endl;
		cout << "m_modules_mask = "<< m_modules_mask << endl;
		cout << "m_chip_number 	= "<< m_chip_number << endl;
		cout << "m_trigger_type = "<< m_trigger_type << endl;
		cout << "m_exp_time 	= "<< m_exp_time << endl;
		cout << "m_time_unit 	= "<< m_time_unit << endl;
		cout << "m_nb_frames 	= "<< m_nb_frames << endl;

		if ( xpci_getImgSeq(	m_pixel_depth, 
								m_modules_mask,
								m_chip_number,
								m_trigger_type,
								m_exp_time,
								m_time_unit,
								m_nb_frames,
								(void**)pSeqImage,
								8000) == -1)
		{
			DEB_ERROR() << "Error: getImgSeq as returned an error..." ;
			
			DEB_TRACE() << "Freeing every image pointer of the images array";
			for(int i=0 ; i < m_nb_frames ; i++)
				delete[] pSeqImage[i];
			
			DEB_TRACE() << "Freeing images array";
			delete[] pSeqImage;			

			m_status = XpadCamera::Fault;
			throw LIMA_HW_EXC(Error, "getImgSeq as returned an error...");

			
		}

		m_status = XpadCamera::Readout;
		
		DEB_TRACE() 	<< "#######################"
				<< "\nall images are acquired"
				<< "\n#######################" ;
		
		//- ATTENTION :
		//- Xpix acquires a buffer sized according to m_module_number.
		//- Displayed/Lima image will be ALWAYS 560*960, even if some modules are not availables ! 
		//- Image zone where a module is not available will be set to "zero"
		 
		//XPIX LIB buffer						//Device requested buffer
		//--------------						//--------------
		//line 1 	mod1						//line 1 	mod1
		//line 1 	mod2						//line 2 	mod1
		//line 1 	mod3						//line 3 	mod1
		//...									//...
		//line 1	mod8						//line 120	mod1
		//--------------						//--------------
		//line 2 	mod1						//line 1 	mod2
		//line 2 	mod2						//line 2 	mod2
		//line 2 	mod3						//line 3 	mod2
		//...									//...
		//line 2	mod4						//line 120	mod2
		//--------------						//--------------
		//...									//...
		//--------------						//--------------
		//line 120 	mod1						//line 1 	mod8
		//line 120 	mod2						//line 2 	mod8
		//line 120 	mod3						//line 3 	mod8
		//...									//...
		//line 120	mod8						//line 120	mod8
		
		int	i=0, j=0, k=0;

		//- clean each image and call new frame for each frame
		StdBufferCbMgr& buffer_mgr = m_buffer_cb_mgr;
		DEB_TRACE() <<"Cleanning each acquired image and publish it through newFrameReady()";
		for(i=0; i<m_nb_frames; i++)
		{
			pOneImage = pSeqImage[i];

			int buffer_nb, concat_frame_nb;
			buffer_mgr.setStartTimestamp(Timestamp::now());
			buffer_mgr.acqFrameNb2BufferNb(i, buffer_nb, concat_frame_nb);

			uint16_t *ptr = (uint16_t*)(buffer_mgr.getBufferPtr(buffer_nb,concat_frame_nb));
			//clean the ptr with zero memory, pixels of a not available module are set to "0" 
			memset((uint16_t *)ptr,0,m_image_size.getWidth() * m_image_size.getHeight() * 2);
			
			//iterate on all lines of all modules returned by xpix API 
			for(j = 0; j < 120 * m_module_number; j++) 
			{
				uint16_t	OneLine[6+80*m_chip_number];
				
				//copy entire line with its header and footer
				for(k = 0; k < (6+80*m_chip_number); k++)
					OneLine[k] = pOneImage[j*(6+80*m_chip_number)+k];
				
				//compute "offset line" where to copy OneLine[] in the *ptr, to ensure that the lines are copied in order of modules
				int offset = ((120*(OneLine[1]-1))+(OneLine[4]-1)); 
				
				//copy cleaned line in the lima buffer
				for(k = 0; k < (80*m_chip_number); k++)
					ptr[(offset*80*m_chip_number)+k] = OneLine[5+k];
			}
			
			DEB_TRACE() << "image# " << i <<" cleaned" ;
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = i;
			//- raise the image to lima
			buffer_mgr.newFrameReady(frame_info);
		}

		DEB_TRACE() <<"Freeing every image pointer of the images array";
		for(i=0 ; i < m_nb_frames ; i++)
			delete[] pSeqImage[i];
		DEB_TRACE() <<"Freeing images array";
		delete[] pSeqImage;
		m_status = XpadCamera::Ready;
		DEB_TRACE() <<"m_status is Ready";
      }
      break;
    }
  }
  catch( yat::Exception& ex )
  {
      DEB_ERROR() << "Error : " << ex.errors[0].desc;
    throw;
  }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setAllConfigG(const vector<long>& allConfigG)
{
	m_all_config_g = allConfigG;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setFParameters(unsigned deadtime, unsigned init,
			unsigned shutter, unsigned ovf,    unsigned mode,
			unsigned n,       unsigned p,
			unsigned GP1,     unsigned GP2,    unsigned GP3,      unsigned GP4)
{

	DEB_MEMBER_FUNCT();

	DEB_TRACE() << "Setting all F Parameters ..." ;
	m_fparameter_deadtime	= deadtime; //- Temps entre chaque image
	m_fparameter_init		= init;		//- Temps initial
	m_fparameter_shutter	= shutter;	//- 
	m_fparameter_ovf		= ovf;		//- 
	m_fparameter_mode		= mode;		//- mode de synchro
	m_fparameter_n			= n;
	m_fparameter_p			= p;
	m_fparameter_GP1		= GP1;
	m_fparameter_GP2		= GP2;
	m_fparameter_GP3		= GP3;
	m_fparameter_GP4		= GP4;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::setAcquisitionType(short acq_type)
{
	DEB_MEMBER_FUNCT();
	m_acquisition_type = acq_type;

	DEB_TRACE() << "m_acquisition_type = " << m_acquisition_type  ;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::loadFlatConfig(unsigned flat_value)
{
	DEB_MEMBER_FUNCT();

	if (xpci_modLoadFlatConfig(m_modules_mask, m_chip_number, flat_value) == 0)
	{
		DEB_TRACE() << "loadFlatConfig, with value: " <<  flat_value << " -> OK" ;
	}
	else
	{
		throw LIMA_HW_EXC(Error, "Error in loadFlatConfig!");
	}
	
	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::loadAllConfigG()
{
	DEB_MEMBER_FUNCT();

	if(xpci_modLoadAllConfigG(m_modules_mask,	m_chip_number, 
												m_all_config_g[0],//- CMOS_TP
												m_all_config_g[1],//- AMP_TP, 
												m_all_config_g[2],//- ITHH, 
												m_all_config_g[3],//- VADJ, 
												m_all_config_g[4],//- VREF, 
												m_all_config_g[5],//- IMFP, 
												m_all_config_g[6],//- IOTA, 
												m_all_config_g[7],//- IPRE, 
												m_all_config_g[8],//- ITHL, 
												m_all_config_g[9],//- ITUNE, 
												m_all_config_g[10]//- IBUFFER
												) == 0)
	{
		DEB_TRACE() << "loadAllConfigG -> OK" ;
	}
	else
	{
		throw LIMA_HW_EXC(Error, "Error in loadAllConfigG!");
	}
	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::loadConfigG(const vector<unsigned long>& reg_and_value)
{
	DEB_MEMBER_FUNCT();

	if(xpci_modLoadConfigG(m_modules_mask, m_chip_number, reg_and_value[0], reg_and_value[1])==0)
	{
		DEB_TRACE() << "loadConfigG: " << reg_and_value[0] << ", with value: " << reg_and_value[1] << " -> OK" ;
	}
	else
	{
		throw LIMA_HW_EXC(Error, "Error in loadConfigG!");
	}
	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadCamera::loadAutoTest(unsigned known_value)
{
	DEB_MEMBER_FUNCT();

	if(xpci_modLoadAutoTest(m_modules_mask, known_value)==0)
	{
		DEB_TRACE() << "loadAutoTest with value: " << known_value << " -> OK" ;
	}
	else
	{
		throw LIMA_HW_EXC(Error, "Error in loadAutoTest!");
	}
	
}

//-----------------------------------------------------
//		Get the DACL values
//-----------------------------------------------------
vector<uint16_t> XpadCamera::getDacl()
{
	DEB_MEMBER_FUNCT();

	size_t image_size_in_bytes = ((80 * m_chip_number) * 2 + 6*2)  * 120 * m_module_number;
	pOneImage = new uint16_t[ image_size_in_bytes / 2 ];
	
	if(xpci_getModConfig(m_modules_mask, m_chip_number,pOneImage)==0)
	{
		DEB_TRACE() << "getDacl -> OK" ;
		DEB_TRACE() << "Cleanning DACL image and getting DACL values..." ;

		//iterate on all lines of all modules returned by xpix API 
		vector<uint16_t> clean_image;
		int k = 0;
		uint16_t temp = 0;
		for(int j = 0; j < 120 * m_module_number; j++) 
		{
			uint16_t	OneLine[6+80*m_chip_number];
			
			//copy entire line with its header and footer
			for(k = 0; k < (6+80*m_chip_number); k++)
				OneLine[k] = pOneImage[j*(6+80*m_chip_number)+k];
			
			//compute "offset line" where to copy OneLine[] in the *clean_image, to ensure that the lines are copied in order of modules
			int offset = ((120*(OneLine[1]-1))+(OneLine[4]-1)); 
			
			//copy cleaned line in the lima buffer
			for(k = 0; k < (80*m_chip_number); k++)
				clean_image[(offset*80*m_chip_number)+k] = OneLine[5+k];
			
			//get the DACL values: bits 3 to 8 (ie remove 3 first bits)
			for(k = 0; k < (80*m_chip_number); k++)
			{
				temp = clean_image[k];
				clean_image[k] = ((temp & 0x1ff)>>3);
			}
		}
		delete[] pOneImage; 
		DEB_TRACE() << "Image cleaned and DACL values getted" ;
	}
	else
	{
		throw LIMA_HW_EXC(Error, "Error in getDacl!");
	}

	return clean_image;
	
}


//-----------------------------------------------------
//		Get the DACL values
//-----------------------------------------------------
void saveAndloadDacl(uint16_t* all_dacls);
{
}



