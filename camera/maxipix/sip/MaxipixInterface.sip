
namespace Maxipix
{

%TypeHeaderCode

#include "HwInterface.h"
#include "EspiaBufferMgr.h"
#include "MaxipixInterface.h"
#include "MaxipixDet.h"
#include "PriamAcq.h"

using namespace lima;
%End


class DetInfoCtrlObj : HwDetInfoCtrlObj
{
  public:
    DetInfoCtrlObj(Maxipix::MaxipixDet& det);
    virtual ~DetInfoCtrlObj();

    virtual void getMaxImageSize(Size& size /Out/);
    virtual void getDetectorImageSize(Size& size /Out/);

    virtual void getDefImageType(ImageType& image_type /Out/);
    virtual void getCurrImageType(ImageType& image_type /Out/);
    virtual void setCurrImageType(ImageType image_type);

    virtual void getPixelSize(double& size /Out/);
    virtual void getDetectorType(std::string& type /Out/);
    virtual void getDetectorModel(std::string& model /Out/);

    virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
    virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

};

class BufferCtrlObj : HwBufferCtrlObj
{
  public:
    BufferCtrlObj(BufferCtrlMgr& buffer_mgr);
    virtual ~BufferCtrlObj();

    virtual void setFrameDim(const FrameDim& frame_dim);
    virtual void getFrameDim(      FrameDim& frame_dim /Out/);

    virtual void setNbBuffers(int  nb_buffers);
    virtual void getNbBuffers(int& nb_buffers /Out/);

    virtual void setNbConcatFrames(int  nb_concat_frames);
    virtual void getNbConcatFrames(int& nb_concat_frames /Out/);

    virtual void getMaxNbBuffers(int& max_nb_buffers /Out/);

    virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
    virtual void *getFramePtr(int acq_frame_nb);

    virtual void getStartTimestamp(Timestamp& start_ts /Out/);
    virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

    virtual void   registerFrameCallback(HwFrameCallback& frame_cb);
    virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

};

class SyncCtrlObj : HwSyncCtrlObj
{
  public:
    SyncCtrlObj(Espia::Acq& acq, Maxipix::PriamAcq& priam, 
		Maxipix::BufferCtrlObj& buffer_mgr);
    virtual ~SyncCtrlObj();

    virtual void setTrigMode(TrigMode  trig_mode);
    virtual void getTrigMode(TrigMode& trig_mode /Out/);

    virtual void setExpTime(double  exp_time);
    virtual void getExpTime(double& exp_time /Out/);

    virtual void setLatTime(double  lat_time);
    virtual void getLatTime(double& lat_time /Out/);

    virtual void setNbHwFrames(int  nb_frames);
    virtual void getNbHwFrames(int& nb_frames /Out/);

    virtual void getValidRanges(ValidRangesType& valid_ranges /Out/);

};


class Interface : HwInterface
{
 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, 
		  Maxipix::PriamAcq& acq, Maxipix::MaxipixDet& det);
	virtual ~Interface();

//	virtual void getCapList(CapList&) const;
        virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;

	virtual void reset(HwInterface::ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(HwInterface::StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();

};

}; // namespace Maxipix

