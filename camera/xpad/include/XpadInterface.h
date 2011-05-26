#ifndef XPADINTERFACE_H
#define XPADINTERFACE_H

#include "HwInterface.h"
#include "XpadCamera.h"

namespace lima
{
class Interface;

/*******************************************************************
 * \class DetInfoCtrlObj
 * \brief Control object providing Xpad detector info interface
 *******************************************************************/
class XpadDetInfoCtrlObj : public HwDetInfoCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj", "Xpad");

 public:
	XpadDetInfoCtrlObj(XpadCamera& cam);
	virtual ~XpadDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size);
	virtual void getDetectorImageSize(Size& det_image_size);

	virtual void getDefImageType(ImageType& def_image_type);
	virtual void getCurrImageType(ImageType& curr_image_type);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size);
	virtual void getDetectorType(std::string& det_type);
	virtual void getDetectorModel(std::string& det_model);

	virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

 private:
	XpadCamera& m_cam;
};


/*******************************************************************
 * \class BufferCtrlObj
 * \brief Control object providing Xpad buffering interface
 *******************************************************************/
class XpadBufferCtrlObj : public HwBufferCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "BufferCtrlObj", "Xpad");

 public:
	XpadBufferCtrlObj(XpadCamera& simu);
	virtual ~XpadBufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFrameDim(      FrameDim& frame_dim);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames);

	virtual void getMaxNbBuffers(int& max_nb_buffers);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	virtual void registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

 private:
	BufferCtrlMgr& m_buffer_mgr;
};

/*******************************************************************
 * \class SyncCtrlObj
 * \brief Control object providing Xpad synchronization interface
 *******************************************************************/
class XpadSyncCtrlObj : public HwSyncCtrlObj
{
    DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "Xpad");

  public:
	XpadSyncCtrlObj(XpadCamera& cam, HwBufferCtrlObj& buffer_ctrl);
    virtual ~XpadSyncCtrlObj();
	
	virtual bool checkTrigMode(TrigMode trig_mode);
    virtual void setTrigMode(TrigMode  trig_mode);
    virtual void getTrigMode(TrigMode& trig_mode);

    virtual void setExpTime(double  exp_time);
    virtual void getExpTime(double& exp_time);

    virtual void setLatTime(double  lat_time){}//- Not supported by Xpad
    virtual void getLatTime(double& lat_time){}//- Not supported by Xpad

    virtual void setNbHwFrames(int  nb_frames);
    virtual void getNbHwFrames(int& nb_frames);

    virtual void getValidRanges(ValidRangesType& valid_ranges);

  private:
    XpadCamera& m_cam;
};

/*******************************************************************
 * \class Interface
 * \brief Xpad hardware interface
 *******************************************************************/
class XpadInterface : public HwInterface
{
	DEB_CLASS_NAMESPC(DebModCamera, "XpadInterface", "Xpad");

 public:
	XpadInterface(XpadCamera& cam);
	virtual ~XpadInterface();

	//- From HwInterface
	virtual void 	getCapList(CapList&) const;
	virtual void	reset(ResetLevel reset_level);
	virtual void 	prepareAcq();
	virtual void 	startAcq();
	virtual void 	stopAcq();
	virtual void 	getStatus(StatusType& status);
	virtual int 	getNbHwAcquiredFrames();

	//- Xpad specific
	//- Set all the config G
	void setAllConfigG(const vector<long>& allConfigG)
		{m_cam.setAllConfigG(allConfigG);}
	//- Set the F parameters
	void setFParameters(unsigned deadtime, unsigned init,
									unsigned shutter, unsigned ovf,    unsigned mode,
									unsigned n,       unsigned p,
									unsigned GP1,     unsigned GP2,    unsigned GP3,      unsigned GP4);
	//-	set the Acquisition type between fast and slow
	void setAcquisitionType(short acq_type);
	//-	Load of flat config of value: flat_value (on each pixel)
	void loadFlatConfig(unsigned flat_value);
	//- Load all the config G with predefined values (on each chip)
	void loadAllConfigG();
	//- Load a wanted config G with a wanted value
	void loadConfigG(const vector<unsigned long>& reg_and_value);
	//- load a known value to the pixel counters
	void loadAutoTest(unsigned known_value)
		{m_cam.loadAutoTest(known_value);}
	//- Get the DACL values
	vector<uint16_t> getDacl()
		{return m_cam.getDacl();}
	//- Save and load Dacl
	void saveAndloadDacl(uint16_t* all_dacls)
		{m_cam.saveAndloadDacl(all_dacls);}
	

 private:
	XpadCamera&			m_cam;
	CapList 		m_cap_list;
	XpadDetInfoCtrlObj	m_det_info;
	XpadBufferCtrlObj	m_buffer;
	XpadSyncCtrlObj		m_sync;

};
} // namespace lima

#endif // XPADINTERFACE_H
