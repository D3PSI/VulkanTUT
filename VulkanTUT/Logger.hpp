/*
*	File:			Logger.hpp
*	Purpose:		Contains class Logger
*
*/
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

#define EVENT_LOG 0
#define ERROR_LOG 1
#define START_STOP_LOG 2
class Logger
{
public:
	Logger(std::string directory = "");
	void start(void);
	void log(int logNr, std::string text);
	~Logger();
private:
	std::string directory;
	std::string eventLogStreamFileName;
	std::string errorLogStreamFileName;
	std::string startStopStreamFileName;
};

