#ifndef CTEVENT_H
#define CTEVENT_H

#include "LimaCompatibility.h"
#include "CtControl.h"
#include "Event.h"

namespace lima
{
  class HwEventCtrlObj;

  class LIMACORE_API CtEvent : public EventCallbackGen
  {
    DEB_CLASS_NAMESPC(DebModControl,"Event","Control");
    friend class CtControl;

  public:
    typedef std::vector<Event *> EventList;

    CtEvent(CtControl& ct);
    ~CtEvent();
    
    bool hasCapability() const;

    void getEventList(EventList& event_list);
    void resetEventList();

    virtual void   registerEventCallback(EventCallback& cb);
    virtual void unregisterEventCallback(EventCallback& cb);

    virtual void reportEvent(Event *event);

  private:
    class _InternalEventCBK;
    friend class _InternalEventCBK;

    AutoMutex lock();
    void _prepareAcq();

    HwEventCtrlObj     *m_event_ctrl_obj;
    Cond		m_cond;
    CtControl&		m_ct;
    _InternalEventCBK  *m_cb;
    EventList		m_event_list;
  };

  inline AutoMutex CtEvent::lock()
  {
    return AutoMutex(m_cond.mutex());
  }

}
#endif
