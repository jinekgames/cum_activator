// 
// Exeption.cpp
// 
// error handlers
// 
////////////////////////////////////////

#include "Exception.h"
#include <sstream>



myException::myException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* myException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* myException::GetType() const noexcept
{
	return "jnk_gms Exception";
}

int myException::GetLine() const noexcept
{
	return line;
}

const std::string& myException::GetFile() const noexcept
{
	return file;
}

std::string myException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
