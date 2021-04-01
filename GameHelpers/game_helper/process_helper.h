#pragma once

#include <psapi.h>

namespace gta::processes
{
	class processor
	{
		processor()
		{

		}
	public:
		static processor& instance()
		{
			static processor instance;
			return instance;
		}
		void find_and_kill()
		{
			auto id = find_process_id();
			kill_process_by_id(id);
		}

		bool is_updated()
		{
			auto id = find_process_id();
			auto last_id = _last_id;
			_last_id = id;
			return id && id != last_id;
		}

		bool is_presented()
		{
			return 0!= find_process_id();
		}

		bool is_outdated()
		{
			auto id = find_process_id();
			return id == 0;
		}

	private:
		void kill_process_by_id(uint32_t process_id)
		{
			HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process_id);
			TerminateProcess(process_handle, 0);
			CloseHandle(process_handle);
		}

		uint32_t find_process_id()
		{
			DWORD process_ids[1024]{}, size{};

			if (!EnumProcesses(process_ids, sizeof(process_ids), &size))
				return 0;

			auto amount = size / sizeof(DWORD);
			for (int i = 0; i < amount; i++)
			{
				if (process_ids[i] != 0)
				{
					if (check_name(process_ids[i]))
						return process_ids[i];
				}
			}
			return 0;
		}

		bool check_name(uint32_t process_id)
		{
			bool result{ false };

			char process_name[256]{};

			HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				FALSE, process_id);

			if (process_handle)
			{
				HMODULE module{};
				DWORD temp{};

				if (EnumProcessModules(process_handle, &module, sizeof(module), &temp))
				{
					GetModuleBaseNameA(process_handle, module, process_name,
						sizeof(process_name) / sizeof(char));
				}

				if (_name == process_name)
				{
					result = true;
				}
				CloseHandle(process_handle);
			}

			return result;
		}

	private:
		static const inline std::string _name = "GTA5.exe";
		uint32_t _last_id{};
	};
}