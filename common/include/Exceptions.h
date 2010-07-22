#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string.h>
#include <ostream>
#include <sstream>

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
	Exception(Layer layer, ErrorType err_type, std::string err_desc,
		  std::string file_name, std::string funct_name, int line_nr);

	Layer       getLayer()     const;
	ErrorType   getErrType()   const;
	std::string getErrDesc()   const;
	std::string getFileName()  const;
	std::string getFunctName() const;

	std::string getErrMsg()    const;

	template <class T>
	Exception& operator <<(const T& o);

 private:
	Layer m_layer;
	ErrorType m_err_type;
	std::string m_err_desc;
	std::string m_file_name;
	std::string m_funct_name;
	int m_line_nr;
};

template <class T>
Exception& Exception::operator <<(const T& o)
{
	std::ostringstream os;
	os << o;
	m_err_desc += os.str();
	return *this;
}


std::ostream& operator <<(std::ostream& os, Layer layer);
std::ostream& operator <<(std::ostream& os, ErrorType err_type);
std::ostream& operator <<(std::ostream& os, const Exception& e);

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
