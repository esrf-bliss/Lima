
namespace Espia
{
%TypeHeaderCode
#include "Espia.h"
%End

enum {
	Invalid = -1,
	NoBlock = 0,
	BlockForever = -1,
	MetaDev = SCDXIPCI_META_DEV,
};

unsigned long Sec2USec(double sec);
double USec2Sec(unsigned long usec);
void ThrowError(int ret, std::string file, std::string func, int line);

}; // namespace Espia