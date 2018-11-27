/*
*	File:			Logger.cpp
*	Purpose:		Contains functions for class Logger
*
*/
#include "Logger.hpp"

/*
*	Default constructor
*
*
*/
Logger::Logger(std::string directory_) {

	directory = directory_;

}

/*
*	Function:		void Logger::start()
*	Purpose:		Initializes Logger
*
*/
void Logger::start() {

	eventLogStreamFileName		= "event.log";
	errorLogStreamFileName		= "error.log";
	startStopStreamFileName		= "startup.log";

}

/*
*	Function:		void log()
*	Purpose:		Writes to selected Log-File
*
*/
void Logger::log(int logNr, std::string text) {

	static int countEvent = 0;
	static int countError = 0;
	std::ofstream stream;

	switch (logNr) {
	case EVENT_LOG:
		if (countEvent == 0) {

			stream.open(directory + eventLogStreamFileName, std::ios::trunc);
			countEvent++;
		
		}
		else {

			stream.open(directory + eventLogStreamFileName, std::ios::app);
		
		}
		break;
	case ERROR_LOG:
		if (countError == 0) {

			stream.open(directory + errorLogStreamFileName, std::ios::trunc);
			countError++;
		
		}
		else {
		
			stream.open(directory + errorLogStreamFileName, std::ios::app);
		
		}
		break;
	case START_STOP_LOG:
		stream.open(directory + startStopStreamFileName, std::ios::app);
		break;
	default:
		break;

	}

	time_t current_time;
	struct tm local_time;

	time(&current_time);
	localtime_s(&local_time, &current_time);

	int Year	= local_time.tm_year + 1900;
	int Month	= local_time.tm_mon + 1;
	int Day		= local_time.tm_mday;

	int Hour	= local_time.tm_hour;
	int Min		= local_time.tm_min;
	int Sec		= local_time.tm_sec;


	stream << Day << ":" 
		<< Month << ":" 
		<< Year << "   " 
		<< Hour << ":" 
		<< Min << ":" 
		<< Sec << "		===		" 
		<< text << std::endl;
	stream.close();

}

/*
*	Default destructor
*	
*
*/
Logger::~Logger() {



}
