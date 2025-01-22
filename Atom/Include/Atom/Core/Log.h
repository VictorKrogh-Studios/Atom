#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

namespace Atom
{

	class Log
	{
	public:
		static void Initialize();
		static void Shutdown();

		static std::shared_ptr<spdlog::logger> GetEngineLogger()
		{
			return s_EngineLogger;
		}
		static std::shared_ptr<spdlog::logger> GetClientLogger()
		{
			return s_ClientLogger;
		}
	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define AT_CORE_TRACE(...)    ::Atom::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define AT_CORE_INFO(...)     ::Atom::Log::GetEngineLogger()->info(__VA_ARGS__)
#define AT_CORE_WARN(...)     ::Atom::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define AT_CORE_ERROR(...)    ::Atom::Log::GetEngineLogger()->error(__VA_ARGS__)
#define AT_CORE_CRITICAL(...) ::Atom::Log::GetEngineLogger()->critical(__VA_ARGS__)

// Client log macros
#define AT_TRACE(...)         ::Atom::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AT_INFO(...)          ::Atom::Log::GetClientLogger()->info(__VA_ARGS__)
#define AT_WARN(...)          ::Atom::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AT_ERROR(...)         ::Atom::Log::GetClientLogger()->error(__VA_ARGS__)
#define AT_CRITICAL(...)      ::Atom::Log::GetClientLogger()->critical(__VA_ARGS__)