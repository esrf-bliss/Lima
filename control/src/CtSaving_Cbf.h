#ifndef CTSAVING_CBF_H
#define CTSAVING_CBF_H

#include <cbf.h>

#include "CtSaving.h"

namespace lima {

  class SaveContainerCbf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving CBF Container","Control");
    class Compression;
  public:
    SaveContainerCbf(CtSaving &ct_saving);
    virtual ~SaveContainerCbf();

    virtual bool needParralelCompression() const {return true;}
    virtual SinkTaskBase* getCompressionTask(const CtSaving::HeaderMap&);

  protected:
    virtual bool _open(const std::string &filename,
		       std::_Ios_Openmode flags);
    virtual void _close();
    virtual void _writeFile(Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
    virtual void _clear();
  private:
    inline int _writeCbfHeader(Data&,CtSaving::HeaderMap&);
    inline int _writeCbfData(Data&);
    
    
    
    typedef std::map<int,cbf_handle> dataId2cbfHandle;
    void _setHandle(int dataId,cbf_handle);
    cbf_handle _takeHandle(int dataId);
    
    FILE* 		m_fout;
    dataId2cbfHandle 	m_cbfs;
    cbf_handle  	m_current_cbf;
    Mutex		m_lock;
  };

}
#endif // CTSAVING_CBF_H
