#ifndef CTSAVING_NXS_H
#define CTSAVING_NXS_H
#include <iostream>
#include <nexus4tango.h>
#include "CtSaving.h"

using namespace std;
//--------------------------------------------------------------------------------------------------------------------
namespace lima
{

  class SaveContainerNxs : public CtSaving::SaveContainer
  {
	  DEB_CLASS_NAMESPC(DebModControl,"Saving NXS Container","Control");
	public:
	  SaveContainerNxs(CtSaving::Stream& stream);
	  virtual ~SaveContainerNxs();
	protected:
	  virtual bool _open(const std::string &filename, std::_Ios_Openmode flags);
	  virtual void _close();
	  virtual void _writeFile(Data &data, CtSaving::HeaderMap &aHeader, CtSaving::FileFormat);
	private:
	  n4t::BufferedData1D* 	m_writer;
	  CtSaving::Parameters			m_pars;
  };

}
//--------------------------------------------------------------------------------------------------------------------
#endif // CTSAVING_NXS_H
