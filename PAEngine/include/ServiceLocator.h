/**
 * @file ServiceLocator.h
 *
 * @brief Just a service locator which is a well-known design pattern.
 *  ref. "geeksforgeeks.org/service-locator-pattern"
 *
 * @author Minsu kang
 *
 */

#pragma once

#include "Log.h"
#include "GameInputHandler.h"

namespace Util
{
	class ServiceLocator
	{		
	public:
    // returns the logger
		static Logger* getLogger() { return m_Logger.get(); }
    // sets the logging service to the given logger
    static void provideLoggingService(const std::shared_ptr<Logger> providedLogger);

    // returns the input handler
    static PAEngine::GameInput* getGameInput() { return m_GameInput.get(); }
    // sets the input handler service to the given i
    static void provideGameInputService(const std::shared_ptr<PAEngine::GameInput> providedLogger);

	private:
		static std::shared_ptr<Logger> m_Logger;	// the logger
    static std::shared_ptr<PAEngine::GameInput> m_GameInput;	// the input handler
	};
}