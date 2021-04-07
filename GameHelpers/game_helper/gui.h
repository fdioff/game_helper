#pragma once

#include <thread>

#include <windows.h>

#include "framework.h"
#include "emulator.h"
#include "process_helper.h"
#include "time_measure.h"
#include "input.h"
#include "Resource.h"

namespace gta::gui
{
	class processor
	{
	public:
		int run(HINSTANCE instance, int show)
		{
			//_thread = std::thread([this]()
			//	{
			//		check_thread();
			//	});

			register_class(instance);

			if (!init_instance(instance, show))
			{
				return FALSE;
			}

			HACCEL accelerator_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_KILLGTA));
			
			input::processor input(instance, _window);

			MSG msg{};
			//HHOOK hook_keyboard{};
			while (GetMessage(&msg, nullptr, 0, 0))
			{
				//if (emulation::processor::instance().no_data_too_long())
				//{
				//	if (hook_keyboard)
				//	{
				//		UnhookWindowsHookEx(hook_keyboard);
				//		hook_keyboard = nullptr;
				//	}
				//	if (processes::processor::instance().is_presented())
				//	{
				//		hook_keyboard = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_ll_hook, 0, 0);
				//	}
				//}

				if (!TranslateAccelerator(msg.hwnd, accelerator_table, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			
			//_stop = true;
			//if (_thread.joinable())
			//	_thread.join();
			
			return (int)msg.wParam;
		}
	private:
		//void check_thread()
		//{
		//	while (!_stop)
		//	{
		//		PostMessage(_window, WM_USER, 0, 0);
		//
		//		std::this_thread::sleep_for(1s);
		//	}
		//}

		ATOM register_class(HINSTANCE instance)
		{
			WNDCLASSEXW wcex{};

			wcex.cbSize = sizeof(WNDCLASSEX);

			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = window_procedure;
			wcex.hInstance = instance;
			wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_KILLGTA));
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
			wcex.lpszClassName = _class_name.c_str();
			wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

			return RegisterClassExW(&wcex);
		}

		BOOL init_instance(HINSTANCE instance, int show)
		{
			_window = CreateWindowW(_class_name.c_str(), L"GTA Helper", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, 350, 160, nullptr, nullptr, instance, nullptr);

			if (!_window)
			{
				return FALSE;
			}

			ShowWindow(_window, show);
			UpdateWindow(_window);

			return TRUE;
		}

		static LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
		{
			switch (message)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(window, message, w_param, l_param);
			}
			return 0;
		}

		//static LRESULT keyboard_ll_hook(int code, WPARAM w_param, LPARAM l_param)
		//{
		//	time::log_timespend_on_death timer0("keyboard_ll_hook all");
		//	emulation::processor::instance().on_keyboard_event(
		//		&emulation::processor::instance(), static_cast<uint32_t>(w_param), reinterpret_cast<KBDLLHOOKSTRUCT*>(l_param));
		//	time::log_timespend_on_death timer1("keyboard_ll_hook only CallNextHookEx");
		//	return CallNextHookEx(0, code, w_param, l_param);
		//}
	private:
		const std::wstring _class_name = L"gtaonlinehelperclass";
		std::thread _thread;
		bool _stop{ false };
		HWND _window{};
	};
}