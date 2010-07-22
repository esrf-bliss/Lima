#include "Espia.h"

using namespace lima;
using namespace std;

void lima::Espia::ThrowError(int ret, string file, string func, int line,
			     DebObj *deb_ptr)
{
	string err_desc = string("Espia error: ") + StrError(ret);

	if (deb_ptr != NULL) {
		DEB_FROM_PTR(deb_ptr);
		DEB_ERROR() << err_desc;
	}

	throw Exception(Hardware, Error, err_desc, file, func, line);
}

string lima::Espia::StrError(int ret)
{
	return espia_strerror(ret);
}
