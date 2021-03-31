#pragma once

#include <windows.h>

#include "framework.h"
#include "emulator.h"
#include "Resource.h"

namespace gta::gui
{
	class processor
	{
	public:
		int run(HINSTANCE instance, int show)
		{
			register_class(instance);

			if (!init_instance(instance, show))
			{
				return FALSE;
			}

			HACCEL accelerator_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_KILLGTA));
			
			auto hookKeyboard = SetWindowsHookExA(WH_KEYBOARD_LL,
				[](int code, WPARAM w_param, LPARAM l_param)->LRESULT
				{
					static emulation::processor emulator;

					emulator.on_keyboard_event(&emulator, static_cast<uint32_t>(w_param), reinterpret_cast<KBDLLHOOKSTRUCT*>(l_param));

					return CallNextHookEx(0, code, w_param, l_param);
				}, 0, 0);

			MSG msg;
			while (GetMessage(&msg, nullptr, 0, 0))
			{
				if (!TranslateAccelerator(msg.hwnd, accelerator_table, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			UnhookWindowsHookEx(hookKeyboard);
			return (int)msg.wParam;
		}
	private:
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
			HWND window = CreateWindowW(_class_name.c_str(), L"GTA Helper", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, 350, 160, nullptr, nullptr, instance, nullptr);

			if (!window)
			{
				return FALSE;
			}

			ShowWindow(window, show);
			UpdateWindow(window);

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

	private:
		const std::wstring _class_name = L"gtaonlinehelperclass";
	};
}