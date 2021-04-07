#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <future>
#include <condition_variable>
#include <thread>
#include <shared_mutex>
#include <queue>

#include <windows.h>
#include <psapi.h>

#include "framework.h"
#include "capturer.h"
#include "process_helper.h"
#include "configuration.h"
#include "time_measure.h"
#include "keys.h"

namespace gta::emulation
{
	class processor
	{
		struct kb_event
		{
			input::e_keyboard_key_id key{};
			bool press{};
		};
		
		processor()
			: _ctrl_pressed(false)
			, _shift_pressed(false)
		{
			_thread = std::thread(&processor::work_thread, this);
		}
	public:
		static processor& instance()
		{
			static processor instance;
			return instance;
		}
		~processor()
		{
			_stop = true;
			_cv.notify_one();
			if (_thread.joinable())
				_thread.join();
		}
	public:
		void on_keyboard_event(input::e_keyboard_key_id key, bool press)
		{
			logger::instance().log(__func__, ": key=", key, ", press=", press);

			time::log_timespend_on_death timer0("on_keyboard_event all");

			std::unique_lock l(_mtx);
			_queue.push({ key, press });
			l.unlock();
			_cv.notify_one();
		}

		bool no_data_too_long()
		{
			auto now = std::chrono::steady_clock::now();
			if ((now - _last_check) < 1s)
				return false;
			_last_check = now;
			return (now - _last_press) > std::chrono::seconds(30);
		}
	private:
		int32_t to_virtual_key(const input::e_keyboard_key_id vk)
		{
			using id = input::e_keyboard_key_id;

			if (vk == id::_0) return 0x30;
			if (vk == id::_1) return 0x31;
			if (vk == id::_2) return 0x32;
			if (vk == id::_3) return 0x33;
			if (vk == id::_4) return 0x34;
			if (vk == id::_5) return 0x35;
			if (vk == id::_6) return 0x36;
			if (vk == id::_7) return 0x37;
			if (vk == id::_8) return 0x38;
			if (vk == id::_9) return 0x39;

			if (vk == id::_a) return 0x41;
			if (vk == id::_b) return 0x42;
			if (vk == id::_c) return 0x43;
			if (vk == id::_d) return 0x44;
			if (vk == id::_e) return 0x45;
			if (vk == id::_f) return 0x46;
			if (vk == id::_g) return 0x47;
			if (vk == id::_h) return 0x48;
			if (vk == id::_i) return 0x49;
			if (vk == id::_j) return 0x4A;
			if (vk == id::_k) return 0x4B;
			if (vk == id::_l) return 0x4C;
			if (vk == id::_m) return 0x4D;
			if (vk == id::_n) return 0x4E;
			if (vk == id::_o) return 0x4F;
			if (vk == id::_p) return 0x50;
			if (vk == id::_q) return 0x51;
			if (vk == id::_r) return 0x52;
			if (vk == id::_s) return 0x53;
			if (vk == id::_t) return 0x54;
			if (vk == id::_u) return 0x55;
			if (vk == id::_v) return 0x56;
			if (vk == id::_w) return 0x57;
			if (vk == id::_x) return 0x58;
			if (vk == id::_y) return 0x59;
			if (vk == id::_z) return 0x5A;

			if (vk == id::numpud_0) return VK_NUMPAD0;
			if (vk == id::numpud_1) return VK_NUMPAD1;
			if (vk == id::numpud_2) return VK_NUMPAD2;
			if (vk == id::numpud_3) return VK_NUMPAD3;
			if (vk == id::numpud_4) return VK_NUMPAD4;
			if (vk == id::numpud_5) return VK_NUMPAD5;
			if (vk == id::numpud_6) return VK_NUMPAD6;
			if (vk == id::numpud_7) return VK_NUMPAD7;
			if (vk == id::numpud_8) return VK_NUMPAD8;
			if (vk == id::numpud_9) return VK_NUMPAD9;

			if (vk == id::f1) return VK_F1;
			if (vk == id::f2) return VK_F2;
			if (vk == id::f3) return VK_F3;
			if (vk == id::f4) return VK_F4;
			if (vk == id::f5) return VK_F5;
			if (vk == id::f6) return VK_F6;
			if (vk == id::f7) return VK_F7;
			if (vk == id::f8) return VK_F8;
			if (vk == id::f9) return VK_F9;
			if (vk == id::f10) return VK_F10;
			if (vk == id::f11) return VK_F11;
			if (vk == id::f12) return VK_F12;

			if (vk == id::enter) return VK_RETURN;
			if (vk == id::spacebar) return VK_SPACE;
			if (vk == id::escape) return VK_ESCAPE;
			if (vk == id::page_up) return VK_PRIOR;
			if (vk == id::page_down) return VK_NEXT;
			if (vk == id::end) return VK_END;
			if (vk == id::home) return VK_HOME;
			if (vk == id::arrow_left) return VK_LEFT;
			if (vk == id::arrow_up) return VK_UP;
			if (vk == id::arrow_right) return VK_RIGHT;
			if (vk == id::arrow_down) return VK_DOWN;
			if (vk == id::del) return VK_DELETE;
			if (vk == id::tab) return VK_TAB;
			if (vk == id::backspace) return VK_BACK;
			if (vk == id::print_screen) return VK_SNAPSHOT;
			if (vk == id::pause_break) return VK_PAUSE;
			if (vk == id::caps_lock) return VK_CAPITAL;
			if (vk == id::insert) return VK_INSERT;
			if (vk == id::apps) return VK_APPS;
			if (vk == id::multiply) return VK_MULTIPLY;
			if (vk == id::add) return VK_ADD;
			if (vk == id::substract) return VK_SUBTRACT;
			if (vk == id::decimal) return VK_DECIMAL;
			if (vk == id::divide) return VK_DIVIDE;
			if (vk == id::num_lock) return VK_NUMLOCK;
			if (vk == id::tilde) return VK_OEM_3;
			if (vk == id::minus) return VK_OEM_MINUS;
			if (vk == id::plus) return VK_OEM_PLUS;
			if (vk == id::bracket_open) return VK_OEM_4;
			if (vk == id::bracket_close) return VK_OEM_6;
			if (vk == id::slash) return VK_OEM_5;
			if (vk == id::quote) return VK_OEM_7;
			if (vk == id::semicolon) return VK_OEM_1;
			if (vk == id::slash_back) return VK_OEM_2;
			if (vk == id::period) return VK_OEM_PERIOD;
			if (vk == id::comma) return VK_OEM_COMMA;
			if (vk == id::scroll_lock) return VK_SCROLL;
			if (vk == id::volume_mute) return VK_VOLUME_MUTE;
			if (vk == id::volume_down) return VK_VOLUME_DOWN;
			if (vk == id::volume_up) return VK_VOLUME_UP;
			if (vk == id::media_next) return VK_MEDIA_NEXT_TRACK;
			if (vk == id::media_previous) return VK_MEDIA_PREV_TRACK;
			if (vk == id::media_stop) return VK_MEDIA_STOP;
			if (vk == id::media_play_pause) return VK_MEDIA_PLAY_PAUSE;
			if (vk == id::clear) return VK_CLEAR;

			if (vk == id::win_left) return VK_LWIN;
			if (vk == id::win_right) return VK_RWIN;

			if (vk == id::shift) return VK_SHIFT;

			if (vk == id::shift_left) return VK_LSHIFT;
			if (vk == id::shift_right) return VK_RSHIFT;

			if (vk == id::control_left) return VK_LCONTROL;
			if (vk == id::control) return VK_CONTROL;

			if (vk == id::menu_left) return VK_LMENU;
			if (vk == id::menu_right) return VK_RMENU;

			return 0;
		}

		bool scancode_filtered(const input::e_keyboard_key_id vk)
		{
			using id = input::e_keyboard_key_id;
			switch (vk)
			{
			case id::arrow_down:
			case id::arrow_left:
			case id::arrow_right:
			case id::arrow_up:
			case id::del:
			case id::insert:
			case id::home:
			case id::end:
			case id::page_down:
			case id::page_up:
			case id::win_left:
				return true;
			}

			return false;
		}

		INPUT generate_key_press(input::e_keyboard_key_id vk, bool press)
		{
			INPUT input{};
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = static_cast<WORD>(to_virtual_key(vk));

			input.ki.wScan = static_cast<WORD>(MapVirtualKey(static_cast<UINT>(input.ki.wVk), MAPVK_VK_TO_VSC));
			input.ki.dwFlags = KEYEVENTF_SCANCODE;

			if (scancode_filtered(vk))
				input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

			if (!press)
				input.ki.dwFlags |= KEYEVENTF_KEYUP;

			return input;
		}

		void emulate_press(input::e_keyboard_key_id vk, int times = 1)
		{
			INPUT input{};
			for (int idx = 0; idx < times; ++idx)
			{
				input = generate_key_press(vk, true);
				int res = SendInput(1, &input, sizeof(INPUT));
				if (!res)
					logger::instance().log("SendInput failed");
				std::this_thread::sleep_for(std::chrono::milliseconds(configuration::processor::instance().press_delay_ms()));
				input = generate_key_press(vk, false);
				res = SendInput(1, &input, sizeof(INPUT));
				if (!res)
					logger::instance().log("SendInput failed");
				std::this_thread::sleep_for(std::chrono::milliseconds(configuration::processor::instance().press_delay_ms()));
			}
		}

		void generate_armor_use()
		{
			using id = input::e_keyboard_key_id;

			logger::instance().log("generate_armor_use");
			emulate_press(id::_m);
			std::this_thread::sleep_for(std::chrono::milliseconds(configuration::processor::instance().capture_delay_ms()));
			auto down = capture::processor::instance().check();
			logger::instance().log(down);
			if (down != 0)
			{
				emulate_press(id::arrow_down, down);
				emulate_press(id::enter);
				emulate_press(id::arrow_down, 1);
				emulate_press(id::enter);
				emulate_press(id::arrow_down, 4);
				emulate_press(id::enter);
			}
			emulate_press(id::_m);
		}

		void generate_snack_use()
		{
			using id = input::e_keyboard_key_id;

			logger::instance().log("generate_snack_use");
			emulate_press(id::_m);
			std::this_thread::sleep_for(std::chrono::milliseconds(configuration::processor::instance().capture_delay_ms()));
			auto down = capture::processor::instance().check();
			logger::instance().log(down);
			if (down != 0)
			{
				emulate_press(id::arrow_down, down);
				emulate_press(id::enter);
				emulate_press(id::arrow_down, 2);
				emulate_press(id::enter, 2);
			}
			emulate_press(id::_m);
		}

		void work_thread()
		{
			while (!_stop)
			{
				std::unique_lock<std::mutex> l(_mtx);
				_cv.wait_for(l, 100ms, [this] { return !_queue.empty(); });
				if (_queue.empty())
					continue;
				kb_event evnt = _queue.front();
				_queue.pop();
				l.unlock();
				step(evnt);
			}
		}

		void step(const kb_event& evnt)
		{
			time::log_timespend_on_death timer("step");
			processor::instance()._last_press = std::chrono::steady_clock::now();

			using id = input::e_keyboard_key_id;

			if (evnt.press)
			{
				if (evnt.key == id::control_left)
				{
					processor::instance()._ctrl_pressed = true;
				}
				else if (evnt.key == id::shift_left)
				{
					processor::instance()._shift_pressed = true;
				}
				else if (evnt.key == id::f12)
				{
					if (processor::instance()._ctrl_pressed && processor::instance()._shift_pressed)
						processes::processor::instance().find_and_kill();
				}
			}
			else
			{
				if (evnt.key == id::control_left)
				{
					processor::instance()._ctrl_pressed = false;
				}
				else if (evnt.key == id::shift_left)
				{
					processor::instance()._shift_pressed = false;
				}
				else if (evnt.key == id::f11)
				{
					if (processor::instance()._shift_pressed)
						processor::instance().generate_armor_use();
				}
				else if (evnt.key == id::f10)
				{
					if (processor::instance()._shift_pressed)
						processor::instance().generate_snack_use();
				}
			}
		}
	private:
		bool _ctrl_pressed;
		bool _shift_pressed;
		std::chrono::time_point<std::chrono::steady_clock> _last_press{};
		std::chrono::time_point<std::chrono::steady_clock> _last_check{};

		std::mutex _mtx;
		std::condition_variable _cv;
		std::queue<kb_event> _queue;
		std::thread _thread;
		bool _stop{ false };
	};
}