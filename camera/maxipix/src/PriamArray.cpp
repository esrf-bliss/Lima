
#include "PriamArray.h"
#include "MaxipixDet.h"

using namespace std;
using namespace lima;
using namespace lima::Maxipix;

const PriamArray::PriamArrayBitType PriamArrayBit[] = {
    // --- DUMMY
    {0, 1, 
        3, {4, 3, 2, -1}, 
        3, {7, 6, 5, -1}},
    // --- MPX2
    {11, 10, 
        3, {9, 8, 7, -1}, 
        3, {12, 5, 4, -1}},
    // --- MPX2MXR20
    {0, 9, 
        3, {7, 6, 8, -1}, 
        3, {12, 10, 11, -1}},
    // --- TPX10
    {7, 13, 
        4, {8, 12, 10, 11},
        2, {9, 6, -1, -1}} 
};

PriamArray::PriamArray(MaxipixDet::Version version)
          :m_version(version)
{
    m_bit= PriamArrayBit[version];
}

PriamArray::~PriamArray()
{
}

void PriamArray::pixelConfig2String(char *mask, char *test, char *low, char *high, string& buffer)
{
    char maskBit[4];
    int size, base, baseMask, baseTest, baseLow[4], baseHigh[4];
    int ib, irow, wcol, wbit, idx, val;

    size= (256 * 256 * 14) / 8;
    buffer.assign(size, (char)0x00);

    for (ib=0; ib<4; ib++)
	maskBit[ib]= 1<<ib;

    for (irow= 0; irow<256; irow++) {

	base= irow * 14 * 32;
	baseMask= base + 32 * (13 - m_bit.mask);
	baseTest= base + 32 * (13 - m_bit.test);
	for (ib= 0; ib<m_bit.nbLow; ib++)
	    baseLow[ib]= base + 32 * (m_bit.low[ib]);
	for (ib= 0; ib<m_bit.nbHigh; ib++)
	    baseHigh[ib]= base + 32 * (m_bit.high[ib]);

	for (wcol= 0; wcol<32; wcol++) {
	    for (wbit=0; wbit<8; wbit++) {
		idx= (irow * 256) + 255 - (wcol * 8 + wbit);
	        val= 1 << (7 - wbit);

		if (mask[idx] & maskBit[0])
		    buffer[baseMask + wcol] |= val;
		if (test[idx] & maskBit[0])
		    buffer[baseTest + wcol] |= val;
		for (ib=0; ib<m_bit.nbLow; ib++) {
		    if (low[idx] & maskBit[ib])
			buffer[baseLow[ib] + wcol] |= val;
		}
		for (ib=0; ib<m_bit.nbHigh; ib++) {
		    if (high[idx] & maskBit[ib])
			buffer[baseHigh[ib] + wcol] |= val;
		}
	    }
	}
    }

}
	
void PriamArray::serialData2Array(string buffer, unsigned short *data)
{
    int row, dbit, wcol, dcol, wbit;

    memset(data, 0, 256*256*sizeof(unsigned short));

    for (row=0; row<256; row++) {
        for (dbit=0; dbit<14; dbit++) {
            for (wcol=0; wcol<32; wcol++) {
                for (wbit=7; wbit>=0; wbit--) {
                    dcol= 255-(wcol*8+wbit);
                    if (buffer[row*14*32+dbit*32+wcol]&(1<<(7-wbit))) {
                        data[row*256+dcol] |= (1<<(13-dbit));
                    }
                }
            }
        }
    }
}
