#ifndef _PRIAMARR_H
#define _PRIAMARR_H

#include "PriamAcq.h"

namespace lima {
namespace Maxipix {

class PriamArray {

  public:
    struct PriamArrayBitType {
	short mask;
	short test;
	short nbLow;
	short low[4];
	short nbHigh;
	short high[4];
    };

    PriamArray(PriamAcq::MpxVersion);
    ~PriamArray();

    void pixelConfig2String(char*, char*, char*, char*, std::string&);
    void serialData2Array(std::string, unsigned short*);

  private:
    PriamAcq::MpxVersion m_version;
    PriamArrayBitType m_bit;
};

};
};

#endif // _PRIAMARR_H
