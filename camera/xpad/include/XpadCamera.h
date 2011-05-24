#ifndef XPADCAMERA_H
#define XPADCAMERA_H

///////////////////////////////////////////////////////////
// YAT::TASK 
///////////////////////////////////////////////////////////
#include <yat/threading/Task.h>

#define kLO_WATER_MARK      128
#define kHI_WATER_MARK      512

#define kPOST_MSG_TMO       2

const size_t  XPAD_DLL_START_FAST_MSG		=	(yat::FIRST_USER_MSG + 100);
const size_t  XPAD_DLL_START_SLOW_B2_MSG	=	(yat::FIRST_USER_MSG + 103);
const size_t  XPAD_DLL_START_SLOW_B4_MSG	=	(yat::FIRST_USER_MSG + 104);

///////////////////////////////////////////////////////////

//- Xpix
#include <xpci_interface.h>
#include <xpci_interface_expert.h>
#include <xpci_time.h>

#include <stdlib.h>
#include <limits>

#include "HwMaxImageSizeCallback.h"
#include "HwBufferMgr.h"

using namespace std;

namespace lima
{
	/*******************************************************************
	* \class XpadCamera
	* \brief object controlling the xpad detector via xpix driver
	*******************************************************************/
	class XpadCamera : public HwMaxImageSizeCallbackGen, public yat::Task
	{
		DEB_CLASS_NAMESPC(DebModCamera, "XpadCamera", "Xpad");

	public:

		enum Status {
			Ready, Exposure, Readout,Latency,Fault
		};

		XpadCamera();
		~XpadCamera();

		void start();
		void stop();

		//- Det info
		void getImageSize(Size& size);
		void setPixelDepth(ImageType pixel_depth);
		void getPixelDepth(ImageType& pixel_depth);
		void getPixelSize(double& size);
		void getDetectorType(std::string& type);
		void getDetectorModel(std::string& model);

		//- Buffer
		BufferCtrlMgr& getBufferMgr();
		void setNbFrames(int  nb_frames);
		void getNbFrames(int& nb_frames);

		//- Sync 
		void setTrigMode(TrigMode  mode);
		void getTrigMode(TrigMode& mode);
		void setExpTime(double  exp_time);
		void getExpTime(double& exp_time);
		
		//- Status
		void getStatus(XpadCamera::Status& status);
	
		//---------------------------------------------------------------
		//- XPAD Stuff
		//- Set all the config G
		void setAllConfigG(const vector<long>& allConfigG);
		//- Set the F parameters
		void setFParameters(unsigned deadtime, unsigned init,
			unsigned shutter, unsigned ovf,    unsigned mode,
			unsigned n,       unsigned p,
			unsigned GP1,     unsigned GP2,    unsigned GP3,      unsigned GP4);
		//-	Set the Acquisition type between fast and slow
		void setAcquisitionType(short acq_type);
		//-	Load of flat config of value: flat_value (on each pixel)
		void loadFlatConfig(unsigned flat_value);
		//- Load all the config G with predefined values (on each chip)
		void loadAllConfigG();
		//- Load a wanted config G with a wanted value
		void loadConfigG(const vector<unsigned long>& reg_and_value);
		//- Load a known value to the pixel counters
		void loadAutoTest(unsigned known_value);
		//- Get the DACL values
		vector<uint16_t> getDacl();
		//- Save and load Dacl
		void saveAndloadDacl(uint16_t* all_dacls);

	protected:
		virtual void setMaxImageSizeCallbackActive(bool cb_active);	

		//- yat::Task implementation
	protected: 
		virtual void handle_message( yat::Message& msg )throw (yat::Exception);
	private:
		//- lima stuff
		SoftBufferAllocMgr 	m_buffer_alloc_mgr;
		StdBufferCbMgr 		m_buffer_cb_mgr;
		BufferCtrlMgr 		m_buffer_ctrl_mgr;
		bool 				m_mis_cb_act;

		//- img stuff
		int 			m_nb_frames;		
		Size			m_image_size;
		IMG_TYPE		m_pixel_depth;
		unsigned short	m_trigger_type;
		unsigned		m_exp_time;

		uint16_t**	pSeqImage;
		uint16_t*	pOneImage;

		//---------------------------------
		//- xpad stuff 
		short			m_acquisition_type;
		unsigned		m_modules_mask;
		int				m_module_number;
		unsigned		m_chip_number;
		int				m_full_image_size_in_bytes;
		unsigned		m_time_unit;
		vector<long>	m_all_config_g;

		//- FParameters
		unsigned		m_fparameter_deadtime;
		unsigned		m_fparameter_init;
		unsigned		m_fparameter_shutter;
		unsigned		m_fparameter_ovf;
		unsigned		m_fparameter_mode;
		unsigned		m_fparameter_n;
		unsigned		m_fparameter_p;
		unsigned		m_fparameter_GP1;
		unsigned		m_fparameter_GP2;
		unsigned		m_fparameter_GP3;
		unsigned		m_fparameter_GP4;

		XpadCamera::Status	m_status;
		bool			m_stop_asked;
		bool			m_video_mode;



	};
} // namespace lima


#endif // XPADCAMERA_H
