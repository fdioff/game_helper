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
			: _delay(1000)
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

				std::string temp;
				while (!file.eof())
				{
					std::getline(file, temp);
					const std::string name_delay = "delay:";
					if (temp.find(name_delay) != std::string::npos)
					{
						std::string delay;
						std::for_each(temp.begin(), temp.end(),
							[&delay](const char c)
							{
								if (c >= '0' && c <= '9')
									delay.push_back(c);
							});
						_delay = std::stoi(delay);
					}
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
		int delay_ms() const
		{
			return _delay;
		}
	private:
		int _delay;
	};
}
