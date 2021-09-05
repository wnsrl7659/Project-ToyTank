/**
 * @file Log.cpp
 *
 * @brief An implementation of Log.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include <Shlwapi.h> // SYSTEMTIME

#include "Log.h"

namespace Util
{
	Logger::Logger() : logLineNumber(0)
	{
	}

	Logger::~Logger()
	{
	}

	void Logger::print(SeverityType severity, std::stringstream stream)
	{
		std::stringstream logStream;

		// all severity types but the config type allow custom formatting
		if (!(severity == SeverityType::config))
		{
			// get time
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);

			// header: line number and date (x: xx/xx/xxxx xx:xx:xx)
			if (logLineNumber != 0)
				logStream << "\r\n";
			logStream << logLineNumber++ << ": " << localTime.wDay << "/" << localTime.wMonth << "/" << localTime.wYear << " " << localTime.wHour << ":" << localTime.wMinute << ":" << localTime.wSecond << "\t";

			// write down warning level
			switch (severity)
			{
			case SeverityType::info:
				logStream << "INFO:  ";
				break;
			case SeverityType::debug:
				logStream << "DEBUG:   ";
				break;
			case SeverityType::warning:
				logStream << "WARNING: ";
				break;
			case SeverityType::error:
				logStream << "ERROR:   ";
				break;
			};
		}

		// write the actual message
		logStream << stream.str();

		// show on the debug console
		printf("%s", logStream.str().c_str());

		//// show on visual studio's output (ASCII)
		//OutputDebugStringA(logStream.str().c_str());
	}

	void Logger::print(SeverityType severity, std::string msg)
	{
#ifdef _DEBUG
    std::stringstream stream;
    stream << msg.c_str();
    this->print(severity, std::stringstream(stream.str()));

#endif
	}
}