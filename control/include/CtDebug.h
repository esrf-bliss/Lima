#ifndef CTDEBUG_H
#define CTDEBUG_H

#include <string>
#include <iostream>

namespace lima {

class CtDebug {

    public:

	CtDebug(std::string class_name);
	~CtDebug();

	void setLevel(short level) { m_level= level; }
	void getLevel(short& level) const { level= m_level; }

	inline void trace(std::string func_name, std::string message)
		{ print(0x01, func_name, "INFO", message); }
	inline void warning(std::string func_name, std::string message)
		{ print(0x02, func_name, "WARNING", message); }
	inline void error(std::string func_name, std::string message)
		{ print(0x04, func_name, "ERROR", message); }

    private:
	void print(short level, std::string func_name, std::string type, std::string message);

	std::string	m_class_name;
	short	m_level;
};
} // namespace lima

#endif // CTDEBUG_H
