#pragma once

#include <memory>
#include <thread>
#include <map>

#include <InitGuid.h>
#include <dinput.h>

#include "framework.h"
#include "keys.h"

#pragma comment(lib, "dinput8.lib")

namespace gta::input
{
	class processor
	{
	public:
		processor(HINSTANCE instance, HWND window)
		{
			auto res = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W,
				(void**)&_dinput, nullptr);

			if (FAILED(res) || !_dinput)
				return;

			res = _dinput->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr);
			if (FAILED(res) || !_keyboard)
				return;

			res = _keyboard->SetCooperativeLevel(window, 
				DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
			if (FAILED(res))
				return;

			res = _keyboard->SetDataFormat(&c_dfDIKeyboard);
			if (FAILED(res))
				return;

			res = _keyboard->Acquire();
			if (FAILED(res))
				return;

			if (c_dfDIKeyboard.dwNumObjs == c_dfDIKeyboard.dwDataSize)
			{
				_keys_current.reset(new uint8_t[c_dfDIKeyboard.dwNumObjs]);
				_keys_previous.reset(new uint8_t[c_dfDIKeyboard.dwNumObjs]);
				memset(_keys_current.get(), 0, c_dfDIKeyboard.dwNumObjs);
				memset(_keys_previous.get(), 0, c_dfDIKeyboard.dwNumObjs);
			}
			else
			{
				throw std::runtime_error("");
			}

			auto sc = [](const auto vk)->uint32_t
			{
				return MapVirtualKey(static_cast<UINT>(vk), MAPVK_VK_TO_VSC);
			};

			_keys[sc(VK_LSHIFT)] = e_keyboard_key_id::shift_left;
			_keys[sc(VK_LCONTROL)] = e_keyboard_key_id::control_left;

			_keys[sc(VK_F10)] = e_keyboard_key_id::f10;
			_keys[sc(VK_F11)] = e_keyboard_key_id::f11;
			_keys[sc(VK_F12)] = e_keyboard_key_id::f12;
			
			_keys[sc(VK_NUMPAD0)] = e_keyboard_key_id::numpud_0;
			_keys[sc(VK_NUMPAD1)] = e_keyboard_key_id::numpud_1;
			_keys[sc(VK_NUMPAD2)] = e_keyboard_key_id::numpud_2;
			_keys[sc(VK_NUMPAD3)] = e_keyboard_key_id::numpud_3;
			_keys[sc(VK_NUMPAD4)] = e_keyboard_key_id::numpud_4;
			_keys[sc(VK_NUMPAD5)] = e_keyboard_key_id::numpud_5;
			_keys[sc(VK_NUMPAD6)] = e_keyboard_key_id::numpud_6;
			_keys[sc(VK_NUMPAD7)] = e_keyboard_key_id::numpud_7;
			_keys[sc(VK_NUMPAD8)] = e_keyboard_key_id::numpud_8;
			_keys[sc(VK_NUMPAD9)] = e_keyboard_key_id::numpud_9;

			_keys[sc(VK_MULTIPLY)] = e_keyboard_key_id::multiply;
			_keys[sc(VK_SUBTRACT)] = e_keyboard_key_id::substract;
			_keys[sc(VK_DECIMAL)] = e_keyboard_key_id::decimal;
			_keys[sc(VK_DIVIDE)] = e_keyboard_key_id::divide;
			_keys[sc(VK_ADD)] = e_keyboard_key_id::add;

			_work = std::thread(&processor::work_thread, this);
		}
		~processor()
		{
			_stop = true;
			if (_work.joinable())
				_work.join();

			if (_keyboard)
			{
				_keyboard->Unacquire();
				_keyboard->Release();
			}

			if (_dinput)
			{
				_dinput->Release();
			}
		}
	private:
		void work_thread()
		{
			while (!_stop)
			{
				{
					//time::log_timespend_on_death timer("input one iteration");
					if (!_keyboard)
						throw std::runtime_error("_keyboard is nill");

					if (c_dfDIKeyboard.dwNumObjs != c_dfDIKeyboard.dwDataSize)
						throw std::runtime_error("unknown size");

					//auto res = _keyboard->Acquire();
					//if (FAILED(res))
					//	throw std::runtime_error("");
					
					//res = _keyboard->Poll();
					//if (FAILED(res))
					//	throw std::runtime_error("");

					auto res = _keyboard->GetDeviceState(c_dfDIKeyboard.dwNumObjs, _keys_current.get());
					if (FAILED(res))
						throw std::runtime_error("");

					//res = _keyboard->Unacquire();
					//if (FAILED(res))
					//	throw std::runtime_error("");

					//for (int i = 0; i < 256; ++i)
					//{
					//	if (_keys_current.get()[i] != 0)
					//		logger::instance().log(__func__, ": idx=", i);
					//}

					if (memcmp(_keys_previous.get(), _keys_current.get(), c_dfDIKeyboard.dwDataSize) != 0)
					{
						for (const auto& [sc, key] : _keys)
						{
							if (_keys_current.get()[sc] != _keys_previous.get()[sc])
							{
								const bool press_state = _keys_current.get()[sc] != 0;
								emulation::processor::instance().on_keyboard_event(key, press_state);
							}
						}
					}

					std::swap(_keys_previous, _keys_current);
				}

				std::this_thread::sleep_for(10ms);
			}
		}
	private:
		IDirectInput8W* _dinput{};
		IDirectInputDevice8W* _keyboard{};
		std::unique_ptr<uint8_t[]> _keys_current;
		std::unique_ptr<uint8_t[]> _keys_previous;
		bool _stop{ false };
		std::thread _work;
		std::map<uint32_t/*scancode*/, input::e_keyboard_key_id> _keys;
	};
}