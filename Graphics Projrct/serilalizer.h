#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <sstream>
#include "structs.h"


//! Vector to hold struct defined above
std::vector<Drawing> operations;

//! Base case function to stop once there is no more parameters to serialize
template<typename T>
void serializeParameter(std::ostringstream& oss, const std::string& label, const T& value)
{
	oss << label << ":" << value << " ";
}

//! Recursive function to serialize parameters
template<typename T, typename... Args>
void serializeParameter(std::ostringstream& oss, const std::string& label, const T& value, Args... args)
{
	oss << label << ":" << value << " ";
	serializeParameter(oss, args...);  //! Recursively call with remaining parameters
}

//! Function for serializing dynamic arrays (needed for polygon clipping)
template<typename T, typename... Args>
void serializeParameter(std::ostringstream& oss, const std::string& label, const std::vector<T> value, Args... args)
{
	oss << label << ":{ ";
	for (int i = 0; i < value.size(); ++i)
		oss << "(" << value[i].x << "," << value[i].y << ") ";
	oss << "} ";
	serializeParameter(oss, args...);  //! Recursively call with remaining parameters
}

//! Function for serializing dynamic arrays
template<typename T>
void serializeParameter(std::ostringstream& oss, const std::string& label, const std::vector<T> value)
{
	oss << label << ":{ ";
	for (int i = 0; i < value.size(); ++i)
		oss << "(" << value[i].x << "," << value[i].y << ") ";
	oss << "} ";
}

//! Function template to serialize parameters of caller function
template<typename... Args>
std::string serializeParameters(Args... args)
{
	std::ostringstream oss;
	serializeParameter(oss, args...);

	//! remove the trailing whitespace
	std::string serialized = oss.str();
	if (!serialized.empty())
		serialized.pop_back();

	return serialized;
}

#endif