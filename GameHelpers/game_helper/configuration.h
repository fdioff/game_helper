#pragma once

#include <fstream>
#include <string>
#include <algorithm>

namespace gta::configuration
{
	class processor
	{
	private:
		processor()
			: _capture_delay(1000)
			, _press_delay(100)
		{
			this->init();
		}
	private:
		void init()
		{
			try
			{
				std::ifstream file("config.ini");
				if (!file)
					return;

				auto find = [](const auto name, const auto& str, auto& value)
				{
					if (str.find(name) != std::string::npos)
					{
						std::string temp;
						std::for_each(str.begin(), str.end(),
							[&temp](const char c)
							{
								if (c >= '0' && c <= '9')
									temp.push_back(c);
							});
						value = std::stoi(temp);
					}
				};

				std::string temp;
				while (!file.eof())
				{
					std::getline(file, temp);

					find("delay:", temp, _capture_delay);
					find("press delay:", temp, _press_delay);
				}
			}
			catch (...) {}
		}
	public:
		static processor& instance()
		{
			static processor instance;
			return instance;
		}
		int capture_delay_ms() const
		{
			return _capture_delay;
		}
		int press_delay_ms() const
		{
			return _press_delay;
		}
	private:
		int _capture_delay;
		int _press_delay;
	};
}
