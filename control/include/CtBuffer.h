#ifndef CTBUFFER_H
#define CTBUFFER_H

#include "Constants.h"
#include "SizeUtils.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "HwInterface.h"
#include "HwFrameCallback.h"

#include "Data.h"

namespace lima {

  class LIMACORE_API CtBufferFrameCB : public HwFrameCallback
  {
    DEB_CLASS_NAMESPC(DebModControl,"BufferFrameCB","Control");
    friend class CtBuffer;
  public:
    CtBufferFrameCB(CtControl *ct): m_ct(ct),m_ct_accumulation(NULL) {}
  protected:
    bool newFrameReady(const HwFrameInfoType& frame_info);
  private:
    CtControl* 		m_ct;
    CtAccumulation* 	m_ct_accumulation;
  };

  class LIMACORE_API CtBuffer 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Buffer","Control");

  public:
    struct LIMACORE_API Parameters 
    {
      DEB_CLASS_NAMESPC(DebModControl,"Buffer::Parameters","Control");
    public:
      Parameters();
      void reset();
      BufferMode mode;
      long	nbBuffers;
      short	maxMemory;
    };

    CtBuffer(HwInterface *hw);
    ~CtBuffer();

    void setPars(Parameters pars);
    void getPars(Parameters& pars) const;

    void setMode(BufferMode mode);
    void getMode(BufferMode& mode) const;

    void setNumber(long nb_buffers);
    void getNumber(long& nb_buffers) const;

    void setMaxMemory(short max_memory);
    void getMaxMemory(short& max_memory) const;

    void registerFrameCallback(CtControl *ct);
    void unregisterFrameCallback();
	
    void getFrame(Data&,int frameNumber);

    void reset();
    void setup(CtControl *ct);

    static void getDataFromHwFrameInfo(Data&,const HwFrameInfoType&);
  private:

    HwBufferCtrlObj* 	m_hw_buffer;
    CtBufferFrameCB* 	m_frame_cb;
    Parameters		m_pars;
    CtAccumulation* 	m_ct_accumulation;
  };

  inline std::ostream& operator<<(std::ostream &os,
				  const CtBuffer::Parameters &params)
  {
    os << "<"
       << "mode=" << params.mode << ", "
       << "nbBuffers=" << params.nbBuffers << ", "
       << "maxMemory=" << params.maxMemory << ", "
       << ">";
    return os;
  }
} // namespace lima

#endif // CTBUFFER_H
