#include "pch.h"
#include "ServiceLocator.h"

namespace Util
{
	// file logger
	std::shared_ptr<Logger> ServiceLocator::m_Logger = NULL;
	void ServiceLocator::provideLoggingService(const std::shared_ptr<Logger> providedLogger)
	{
		m_Logger = providedLogger;
	}

  // input handler
  std::shared_ptr<PAEngine::GameInput> ServiceLocator::m_GameInput = NULL;
  void ServiceLocator::provideGameInputService(const std::shared_ptr<PAEngine::GameInput> providedInputHandler)
  {
    m_GameInput = providedInputHandler;
  }
}