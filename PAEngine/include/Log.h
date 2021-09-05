/**
 * @file Log.h
 *
 * @brief This is able to display on visual studio's "output"
 *        Probably, It may need to change to display on an actual debug window.
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include <sstream> // string streams

namespace Util
{
	// MESSAGE TYPES
	enum SeverityType
	{
		info = 0,
		debug,
		warning,
		error,
		config
	};

	class Logger
	{
	public:
		// constructor and destructor
		Logger();
		~Logger();

		// print message (varies based on severity level)
		void print(SeverityType severity, std::stringstream stream);
		void print(SeverityType severity, std::string msg);

	private:
		unsigned int logLineNumber;	// used to save the current line number
	};
}