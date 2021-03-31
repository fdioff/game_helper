#pragma once

#include <shared_mutex>
#include <fstream>
#include <chrono>

#ifndef _log_ 
#define _log_ logger::instance().log((std::string(__FUNCTION__) + std::to_string(__LINE__)).c_str());
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

		template<typename t>
		void log(t data)
		{
			std::unique_lock l(_mtx);
			_file << data << std::endl;
		}

		template<typename t>
		void log(const t& data)
		{
			std::unique_lock l(_mtx);
			_file << data << std::endl;
		}

		template<typename t>
		void log(const t* data)
		{
			std::unique_lock l(_mtx);
			_file << data << std::endl;
		}

		void log(const std::string& data)
		{
			std::unique_lock l(_mtx);
			_file << data << std::endl;
		}

	private:
		std::shared_mutex _mtx;
		std::ofstream _file;
	};
}