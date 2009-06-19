#include "Espia.h"

using namespace lima::Espia;
using namespace std;

void lima::Espia::ThrowError(int ret, string file, string func, int line)
{
	string err_desc = string("Espia: ") + espia_strerror(ret);
	throw Exception(Hardware, Error, err_desc, file, func, line);
}
