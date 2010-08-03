#include "Espia.h"

using namespace lima;
using namespace std;


string lima::Espia::StrError(int ret)
{
	return espia_strerror(ret);
}
