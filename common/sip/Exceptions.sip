
enum Layer {
	Common, Control, Hardware,
};

enum ErrorType {
	InvalidValue, NotSupported, Error,
};


class Exception
{
%TypeHeaderCode
#include <Exceptions.h>
using namespace lima;
%End
 public:
	Exception(Layer layer, ErrorType err_type, std::string err_desc,
		  std::string file_name, std::string funct_name, int line_nr);


	std::string getErrMsg() const;

};
