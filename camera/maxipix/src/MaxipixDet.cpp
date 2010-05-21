#include <vector>
#include <string>
#include <sstream>

#include "MaxipixDet.h"
#include "PriamAcq.h"

using namespace lima;
using namespace Maxipix;

MaxipixDet::MaxipixDet(PriamAcq& priam_acq)
      :m_priam(priam_acq),
       m_init(false),
       m_version(MPX2MXR20),
       m_polarity(POSITIVE),
       m_frequency(80.0)
{
    setNbChip(1, 1);
    setPixelGap(0, 0);
    setPriamPort(0, 0, 0);
    m_fsr0.assign(32, (char)0xff);
}

MaxipixDet::~MaxipixDet()
{
}

PriamAcq& MaxipixDet::priamAcq()
{
    return m_priam;
}

void MaxipixDet::resetAllFifo()
{
    for (int i=0; i<m_nchip; i++)
	m_priam.resetFifo(m_port[i]);
}

void MaxipixDet::resetAllChip()
{
    for (int i=0; i<m_nchip; i++)
	m_priam.resetFifo(m_port[i]);
}

void MaxipixDet::setNbChip(int xchip, int ychip)
{
    m_xchip= xchip;
    m_ychip= ychip;
    m_nchip= m_xchip*m_ychip;

    for (int i=0; i<MaxChips; i++)
	m_port[i]= (i<m_nchip) ? i : 0;
}

void MaxipixDet::setPixelGap(int xgap, int ygap)
{
    m_xgap= xgap;
    m_ygap= ygap;
}

void MaxipixDet::setPriamPort(int x, int y, int port)
{
    m_port[x + m_ychip*y]= port;
}

void MaxipixDet::getImageSize(Size& size)
{
    int w, h;

    w= m_xchip*256 + (m_xchip-1)*m_xgap;
    h= m_ychip*256 + (m_ychip-1)*m_ygap;
    size= Size(w, h);
}

void MaxipixDet::getPixelSize(double& size)
{
    size= PixelSize;
}

void MaxipixDet::getDetectorType(std::string& type)
{
    std::ostringstream os;
    os << "MAXIPIX";
}

void MaxipixDet::getDetectorModel(std::string& type)
{
    std::ostringstream os;

    os << m_xchip << "x" << m_ychip << "-";
    switch (m_version) {
      case MPX2:	os << "MPX2" ; break;
      case MPX2MXR20:	os << "MPX2MXR20"; break;
      case TPX10:	os << "TPX10"; break;
      default:		os << "DUMMY";
    };

    type= os.str();
}

void MaxipixDet::setup()
{
    std::vector<bool> chips;
    int i;

    m_priam.setup(m_version, m_polarity, m_frequency, m_fsr0);

    for (i=0; i<MaxChips; i++)
 	chips.push_back(false);
    for (i=0; i<m_nchip; i++)
	chips.assign(m_port[i], true);
    m_priam.setParalellReadout(chips);
    m_init= true;
}

bool MaxipixDet::needReconstruction()
{
    if (((m_xchip==2)&&(m_ychip==2)) || \
	((m_xchip==5)&&(m_ychip==5))) {
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
void MaxipixDet::setVersion(Version version)
{
    m_version= version;
}

void MaxipixDet::getVersion(Version& version)
{
    version= m_version;
}

void MaxipixDet::setPolarity(Polarity polarity)
{
    m_polarity= polarity;
}

void MaxipixDet::getPolarity(Polarity& polarity)
{
    polarity= m_polarity;
}

void MaxipixDet::setFrequency(float frequency)
{
    m_frequency= frequency;
}

void MaxipixDet::getFrequency(float& frequency)
{
    if (m_init==true) {
	m_priam.getOscillator(m_frequency);
    }
    frequency= m_frequency;
}

void MaxipixDet::setFsr0(std::string fsr0)
{
    m_fsr0= fsr0;
}

