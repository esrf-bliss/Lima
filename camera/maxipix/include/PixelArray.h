#ifndef _PRIAMARR_H
#define _PRIAMARR_H

#include "MaxipixDet.h"

namespace lima {
namespace Maxipix {

class PixelConfigArray {

  public:

    PixelConfigArray(MaxipixDet::Version);
    ~PixelConfigArray();

    void convert(std::string&);

    char* maskArray;
    char* testArray;
    char* lowArray;
    char* highArray;

  private:
    struct PixelArrayBitType {
	short mask;
	short test;
	short nbLow;
	short low[4];
	short nbHigh;
	short high[4];
    };

    MaxipixDet::Version m_version;
    PixelArrayBitType m_bit;

};

class PixelDataArray {
  public:
    PixelDataArray(MaxipixDet::Version);
    ~PixelDataArray();

    void convert(std::string, unsigned short*);

  private:
    MaxipixDet::Version m_version;
};

};
};

#endif // _PRIAMARR_H
