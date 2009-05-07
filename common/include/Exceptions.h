#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>

namespace lima
{

enum Layer {
	Common, Control, Hardware,
};

enum ErrorType {
	InvalidValue, NotSupported, Error,
};


class Exception
{
 public:
	typedef std::string String;

	Exception(Layer layer, ErrorType err_type, String err_desc,
		  String file_name, String funct_name, int line_nr);


 private:
	Layer m_layer;
	ErrorType m_err_type;
	String m_err_desc;
	String m_file_name;
	String m_funct_name;
	int m_line_nr;
};


#define LIMA_EXC(layer, err_type, err_desc) \
	Exception(layer, err_type, err_desc, __FILE__, __FUNCTION__, __LINE__)

#define LIMA_COM_EXC(err_type, err_desc) \
	LIMA_EXC(Common, err_type, err_desc)

#define LIMA_CTL_EXC(err_type, err_desc) \
	LIMA_EXC(Control, err_type, err_desc)

#define LIMA_HW_EXC(err_type, err_desc) \
	LIMA_EXC(Hardware, err_type, err_desc)


} // namespace lima

#endif // EXCEPTIONS_H
