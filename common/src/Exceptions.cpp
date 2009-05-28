#include <string>
#include "Exceptions.h"

using namespace lima;
using namespace std;

Exception::Exception(Layer layer, ErrorType err_type, String err_desc,
		     String file_name, String funct_name, int line_nr)
	: m_layer(layer), m_err_type(err_type), m_err_desc(err_desc),
	  m_file_name(file_name), m_funct_name(funct_name), m_line_nr(line_nr)
{
}


string & Exception::getErrDesc()
{
	return m_err_desc;
}
