#pragma once
#include <exception>
#include <iostream>
#include <string>

class ParsingError : public std::exception {
public:
	ParsingError(std::string error = std::string()) : message(error) {
#ifdef _DEBUG
		std::cout << message << "\n";
#endif
	}

	const char* what() const override { return message.c_str(); }
	std::string message;
};