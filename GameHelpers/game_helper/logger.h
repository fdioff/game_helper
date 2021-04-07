#pragma once

#include <shared_mutex>
#include <fstream>
#include <chrono>

#ifndef _log_ 
#define _log_ //logger::instance().log((std::string(__FUNCTION__) + std::to_string(__LINE__)).c_str());
#endif

namespace gta
{
	class logger
	{
		logger()
		{
			using namespace std::literals;
			auto count = std::chrono::steady_clock::now().time_since_epoch().count();
			_file.open("log_"s + std::to_string(count) + ".log");
		}
	public:
		static logger& instance()
		{
			static logger instance;
			return instance;
		}

		template<typename ...Args>
		void log(const Args ...args)
		{
			std::string message;
			parse(message, args...);
			_file << message << std::endl;
#ifdef _DEBUG
			if (IsDebuggerPresent() == TRUE)
			{
				message.append("\n");
				OutputDebugStringA(message.c_str());
			}
#endif
		}

	private:
		void parse(std::string&) {}

		template<typename T>
		void parse(std::string& out, const T& t)
		{
			if constexpr (std::is_convertible_v<T, std::string>)
				out.append(t);
			else if constexpr (std::is_same_v<T, bool>)
				out.append(t ? "true" : "false");
			else if constexpr (std::is_arithmetic_v<T>)
				out.append(std::to_string(t));
			else if constexpr (std::is_enum_v<T>)
				out.append(std::to_string(static_cast<std::underlying_type_t<T>>(t)));
			else if constexpr (std::is_pointer_v<T>)
				out.append(std::to_string(reinterpret_cast<size_t>(t)));
			else
				static_assert(false);
		}

		template<typename T, typename ...Args>
		void parse(std::string& out, const T& t, Args... args)
		{
			//out << t;
			parse(out, t);
			parse(out, args...);
		}

	private:
		std::shared_mutex _mtx;
		std::ofstream _file;
	};
}