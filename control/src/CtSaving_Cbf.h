#ifndef CTSAVING_CBF_H
#define CTSAVING_CBF_H

#include <cbf.h>

#include "CtSaving.h"

namespace lima {

  class SaveContainerCbf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving CBF Container","Control");
  public:
    SaveContainerCbf(CtSaving &ct_saving);
    virtual ~SaveContainerCbf();
  protected:
    virtual bool _open(const std::string &filename,
		std::ios_base::openmode flags);
    virtual void _close();
    virtual void _writeFile(Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
  private:
    inline int _writeCbfHeader(Data&,CtSaving::HeaderMap&);
    inline int _writeCbfData(Data&);

    FILE* 	m_fout;
    cbf_handle 	m_cbf;
  };

}
#endif // CTSAVING_CBF_H
