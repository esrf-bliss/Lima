#ifndef CTSAVING_EDF_H
#define CTSAVING_EDF_H

#include "CtSaving.h"

namespace lima {

  class SaveContainerEdf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving EDF Container","Control");
  public:
    SaveContainerEdf(CtSaving &ct_saving);
    virtual ~SaveContainerEdf();
  protected:
    virtual bool _open(const std::string &filename,
		       std::ios_base::openmode flags);
    virtual void _close();
    virtual void _writeFile(Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
  private:
    void _writeEdfHeader(Data&,CtSaving::HeaderMap&);

    std::ofstream                m_fout;
  };

}
#endif // CTSAVING_EDF_H
