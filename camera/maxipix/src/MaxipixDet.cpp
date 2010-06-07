#include <vector>
#include <string>
#include <sstream>

#include "MaxipixDet.h"
#include "PriamAcq.h"

using namespace lima;
using namespace Maxipix;

MaxipixDet::MaxipixDet()
	  :m_xchip(0), m_ychip(0), 
	   m_xgap(0), m_ygap(0),
	   m_type(Bpp16), m_version(MXR2),
           m_mis_cb_act(false)
{
    setNbChip(1, 1);
}

MaxipixDet::~MaxipixDet()
{
}

void MaxipixDet::setVersion(Version version)
{
    m_version= version;
}

void MaxipixDet::setNbChip(int xchip, int ychip)
{
    if ((xchip != m_xchip)||(ychip != m_ychip)) {
        m_xchip= xchip;
        m_ychip= ychip;
        m_nchip= m_xchip*m_ychip;
	_updateSize();
    }
}

void MaxipixDet::setPixelGap(int xgap, int ygap)
{
    if ((xgap != m_xgap)||(ygap != m_ygap)) {
        m_xgap= xgap;
    	m_ygap= ygap;
	_updateSize();
    }
}

void MaxipixDet::_updateSize() {
    int w= m_xchip*256 + (m_xchip-1)*m_xgap;
    int h= m_ychip*256 + (m_ychip-1)*m_ygap;
    m_size= Size(w, h);
    if (m_mis_cb_act) {
	maxImageSizeChanged(m_size, m_type);
    }
}
	
void MaxipixDet::getImageSize(Size& size)
{
    size= m_size;
}

void MaxipixDet::getPixelSize(double& size)
{
    size= PixelSize;
}

void MaxipixDet::getImageType(ImageType& type)
{
    type= m_type;
}

void MaxipixDet::getDetectorType(std::string& type)
{
    std::ostringstream os;
    os << "MAXIPIX";
    type= os.str();
}

void MaxipixDet::getDetectorModel(std::string& type)
{
    std::ostringstream os;

    os << m_xchip << "x" << m_ychip << "-";
    switch (m_version) {
      case MPX2:	os << "MPX2" ; break;
      case MXR2:	os << "MXR2"; break;
      case TPX1:	os << "TPX1"; break;
      default:		os << "DUMMY";
    };

    type= os.str();
}

bool MaxipixDet::needReconstruction()
{
    if (((m_xchip==2)&&(m_ychip==2)) || \
	((m_xchip==5)&&(m_ychip==1))) {
	    return true;
    }
    return false;
}

void MaxipixDet::getReconstruction(MaxipixReconstruction::Model& model)
{
    if ((m_xchip==2)&&(m_ychip==2)) {
	model= MaxipixReconstruction::M_2x2;
    }
    else if ((m_xchip==5)&&(m_ychip)) {
	model= MaxipixReconstruction::M_5x1;
    }
    else {
	throw LIMA_HW_EXC(Error, "Unknown reconstruction model");
    }
}

void MaxipixDet::setMaxImageSizeCallbackActive(bool cb_active)
{
        m_mis_cb_act = cb_active;
}
     
