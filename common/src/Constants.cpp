#include "Constants.h"

using namespace lima;
using namespace std;

ostream& lima::operator <<(ostream& os, AlignDir align_dir)
{
	const char *name = "Unknown";
	switch (align_dir) {
	case Floor:		name = "Floor";		break;
	case Ceil:		name = "Ceil";		break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, ImageType image_type)
{
	const char *name = "Unknown";
	switch (image_type) {
	case Bpp8:		name = "Bpp8";		break;
	case Bpp10:		name = "Bpp10";		break;
	case Bpp12:		name = "Bpp12";		break;
	case Bpp14:		name = "Bpp14";		break;
	case Bpp16:		name = "Bpp16";		break;
	case Bpp32:		name = "Bpp32";		break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, AcqMode acq_mode)
{
	const char *name = "Unknown";
	switch (acq_mode) {
	case Single:		name = "Single";	break;
	case Accumulation:	name = "Accumulation";	break;
	case Concatenation:	name = "Concatenation";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, TrigMode trig_mode)
{
	const char *name = "Unknown";
	switch (trig_mode) {
	case IntTrig:		name = "IntTrig";	break;
	case ExtTrigSingle:	name = "ExtTrigSingle";	break;
	case ExtTrigMult:	name = "ExtTrigMult";	break;
	case ExtGate:		name = "ExtGate";	break;
	case ExtStartStop:	name = "ExtStartStop";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, BufferMode buffer_mode)
{
	const char *name = "Unknown";
	switch (buffer_mode) {
	case Linear:		name = "Linear";	break;
	case Circular:		name = "Circular";	break;
	}
	return os << name;
}

