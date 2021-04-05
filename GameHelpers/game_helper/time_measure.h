#pragma once

#include <chrono>
#include <mutex>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "framework.h"

namespace gta::time
{
	class log_timespend_on_death
	{
	public:
		log_timespend_on_death(std::string key)
			: _start(std::chrono::steady_clock::now())
			, _key(std::move(key))
		{
		}
		~log_timespend_on_death()
		{
			std::unique_lock l(_mtx);

			auto now = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _start);
			auto& value = _time[_key];
			value.push_back(diff);
			if (value.size() % 100 == 0)
			{
				std::chrono::milliseconds summ{};
				std::for_each(value.begin(), value.end(), [&summ](const auto time) {summ += time; });
				std::sort(value.begin(), value.end());
				std::string str =
					"time measure (ms): key=" + _key +
					", min="s + std::to_string(value.begin()->count()) +
					", med="s + std::to_string(value.at(value.size() / 2).count()) +
					", avg="s + std::to_string(summ.count() / value.size()) +
					", max="s + std::to_string(value.rbegin()->count());
				logger::instance().log(str);
				//value.clear();
			}
		}
	public:
		static inline std::shared_mutex _mtx;
		static inline std::map<std::string, std::vector<std::chrono::milliseconds>> _time;
		std::chrono::time_point<std::chrono::steady_clock> _start;
		std::string _key;
	};
}