#include "CtDebug.h"

using namespace lima;

CtDebug::CtDebug(std::string class_name)
	: m_class_name(class_name)
	
{
}

CtDebug::~CtDebug()
{
}

void CtDebug::print(short level, std::string func_name, std::string type, std::string message) {
	if (m_level & level) {
		std::cout << m_class_name << "::" << func_name << " > ";
		std::cout << type << " > ";
		std::cout << message << std::endl;
	}
}
