#ifndef CTACQUISITION_H
#define CTACQUISITION_H

#include "LimaCompatibility.h"
#include "Constants.h"
#include "HwInterface.h"
#include "HwCap.h"
#include "CtControl.h"
#include "Debug.h"

namespace lima 
{	

  class LIMACORE_API CtAcquisition 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Acquisition","Control");
    friend class CtControl;

  public:
    enum AccTimeMode {Live,Real};

    struct Parameters {
      DEB_CLASS_NAMESPC(DebModControl,"Acquisition::Parameters","Control");
    public:
      Parameters();
      void reset();
      
      AcqMode		acqMode;
      AccTimeMode 	accTimeMode;
      int		acqNbFrames;
      double		acqExpoTime;
      double		accMaxExpoTime;
      int		concatNbFrames;
      double		latencyTime;
      TrigMode 		triggerMode;
      
    };

    CtAcquisition(HwInterface *hw);
    ~CtAcquisition();

    // --- global

    void setPars(const Parameters &pars);
    void getPars(Parameters& pars) const;

    void reset();
    void apply(CtControl::ApplyPolicy policy);
    void sync();

    // --- acq modes

    void setAcqMode(AcqMode mode);
    void getAcqMode(AcqMode& mode) const;

    void setAccTimeMode(AccTimeMode mode);
    void getAccTimeMode(AccTimeMode &mode) const;

    void setAcqNbFrames(int nframes);
    void getAcqNbFrames(int& nframes) const;

    void setAcqExpoTime(double acq_time);
    void getAcqExpoTime(double& acq_time) const;

    void setAccMaxExpoTime(double max_time);
    void getAccMaxExpoTime(double& max_time) const;

    void getAccNbFrames(int& nframes) const;
    void getAccExpoTime(double& acc_time) const;
    void getAccLiveTime(double& acc_live_time) const;
    void getAccDeadTime(double& acc_dead_time) const;

    void setConcatNbFrames(int nframes);
    void getConcatNbFrames(int& nframes) const; 

    // --- common

    void setLatencyTime(double latency_time);
    void getLatencyTime(double& latency_time) const;

    void setTriggerMode(TrigMode mode);
    void getTriggerMode(TrigMode& mode) const;

  private:
    class _ValidRangesCallback;
    friend class _ValidRangesCallback;

    struct ChangedPars {
      DEB_CLASS_NAMESPC(DebModControl,"Acquisition::ChangedPars","Control");
    public:
      ChangedPars();
      void set(bool);
      void check(Parameters p1, Parameters p2);

      bool	acqExpoTime;
      bool	acqNbFrames;
      bool	latencyTime;
      bool	triggerMode;
      bool	accMaxExpoTime;
      bool	acqMode;
    };

    void _updateAccPars() const;
    void _setDefaultPars(Parameters* pars);
    void _apply();
    void _hwRead();

    HwSyncCtrlObj	*m_hw_sync;
    HwSyncCtrlObj::ValidRangesType	m_valid_ranges;
    Parameters	m_inpars, m_hwpars;
    ChangedPars	m_changes;
    double		m_readout_time;
    double		m_frame_rate;
    mutable int		m_acc_nframes;
    mutable double	m_acc_exptime;
    mutable double	m_acc_live_time;
    mutable double	m_acc_dead_time;
    bool		m_applied_once;
    _ValidRangesCallback *m_valid_ranges_cb;
  };

  inline std::ostream& operator<<(std::ostream &os,const CtAcquisition::Parameters &params)
  {
    os << "<"
       << "acqMode=" << params.acqMode << ", "
       << "acqNbFrames=" << params.acqNbFrames << ", "
       << "acqExpoTime=" << params.acqExpoTime << ", "
       << "accMaxExpoTime=" << params.accMaxExpoTime << ", "
       << "concatNbFrames=" << params.concatNbFrames << ", "
       << "latencyTime=" << params.latencyTime << ", "
       << "triggerMode=" << params.triggerMode
       << ">";
    return os; 
  }
  
} // namespace lima

#endif // CTACQUISITION_H
