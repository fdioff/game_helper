#pragma once

#include <memory>
#include <tuple>
#include <Windows.h>
#include "framework.h"

namespace gta::capture
{
	class processor
	{
		struct argb
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};
		processor()
		{
			static_assert(sizeof(argb) == sizeof(uint32_t));
			_valid = reinit();
		}
	public:
		static processor& instance()
		{
			static processor instance;
			return instance;
		}
		~processor()
		{
			if (_screen_bitmap)
			{
				DeleteObject(_screen_bitmap);
				_screen_bitmap = 0;
			}

			if (_memory_dc)
			{
				DeleteObject(_memory_dc);
				_memory_dc = 0;
			}

			if (_window_dc)
			{
				ReleaseDC(GetDesktopWindow(), _window_dc);
				_window_dc = 0;
			}

			if (_screen_dc)
			{
				ReleaseDC(nullptr, _screen_dc);
				_screen_dc = 0;
			}
		}
		int check()
		{
			logger::instance().log(__func__);

			capture();

			first_capture_process();

			// YES -> 3
			// NO -> 2
			// YES OFFICE -> 5
			// NO OFFICE -> 4
			// 
			// yes office 716-725 = 9
			// no office 753 - 762 = 9
			// no street 828 - 837 = 9
			// yes street 791 - 800 = 9

			//const int x_0 = 42;
			//const int x_1 = 141;

			const int x_0 = _x_shift + 13;
			const int x_1 = _x_shift + 112;

			//const int y_

			{
				// NO STREET
				size_t sum_no_street{};
				//for (int y = 828; y < 838; ++y)
				for (int y = 942 - _y_shift; y <= 950 - _y_shift; ++y)
				{
					size_t idx_0 = x_0 + y * _width;
					size_t idx_1 = x_1 + y * _width;
					uint32_t color_0 = ((uint32_t*)(_buffer.get()))[idx_0];
					uint32_t color_1 = ((uint32_t*)(_buffer.get()))[idx_1];

					argb* argb_color_0 = (argb*)&color_0;
					argb* argb_color_1 = (argb*)&color_1;
					int sum_0 = argb_color_0->r + argb_color_0->g + argb_color_0->b;
					int sum_1 = argb_color_1->r + argb_color_1->g + argb_color_1->b;

					sum_no_street += sum_0;
					sum_no_street += sum_1;
				}
				logger::instance().log("sum_no_street = "s  + std::to_string(sum_no_street));
				if (9 * 2 * 650 < sum_no_street)
					return 2;
			}
			{
				// YES STREET
				size_t sum_yes_street{};
				//for (int y = 791; y < 801; ++y)
				for (int y = 905 - _y_shift; y <= 913 - _y_shift; ++y)
				{
					size_t idx_0 = x_0 + y * _width;
					size_t idx_1 = x_1 + y * _width;
					uint32_t color_0 = ((uint32_t*)(_buffer.get()))[idx_0];
					uint32_t color_1 = ((uint32_t*)(_buffer.get()))[idx_1];

					argb* argb_color_0 = (argb*)&color_0;
					argb* argb_color_1 = (argb*)&color_1;
					int sum_0 = argb_color_0->r + argb_color_0->g + argb_color_0->b;
					int sum_1 = argb_color_1->r + argb_color_1->g + argb_color_1->b;

					sum_yes_street += sum_0;
					sum_yes_street += sum_1;
				}
				logger::instance().log("no sum_yes_street = "s + std::to_string(sum_yes_street));
				if (9 * 2 * 650 < sum_yes_street)
					return 3;
			}
			{
				// NO OFFICE
				size_t sum_no_office{};
				//for (int y = 753; y < 763; ++y)
				for (int y = 867 - _y_shift; y <= 875 - _y_shift; ++y)
				{
					size_t idx_0 = x_0 + y * _width;
					size_t idx_1 = x_1 + y * _width;
					uint32_t color_0 = ((uint32_t*)(_buffer.get()))[idx_0];
					uint32_t color_1 = ((uint32_t*)(_buffer.get()))[idx_1];

					argb* argb_color_0 = (argb*)&color_0;
					argb* argb_color_1 = (argb*)&color_1;
					int sum_0 = argb_color_0->r + argb_color_0->g + argb_color_0->b;
					int sum_1 = argb_color_1->r + argb_color_1->g + argb_color_1->b;

					sum_no_office += sum_0;
					sum_no_office += sum_1;
				}
				logger::instance().log("sum_no_office = "s + std::to_string(sum_no_office));
				if (9 * 2 * 650 < sum_no_office)
					return 4;
			}
			{
				// YES OFFICE
				size_t sum_yes_office{};
				//for (int y = 716; y < 726; ++y)
				for (int y = 830 - _y_shift; y <= 838 - _y_shift; ++y)
				{
					size_t idx_0 = x_0 + y * _width;
					size_t idx_1 = x_1 + y * _width;
					uint32_t color_0 = ((uint32_t*)(_buffer.get()))[idx_0];
					uint32_t color_1 = ((uint32_t*)(_buffer.get()))[idx_1];

					argb* argb_color_0 = (argb*)&color_0;
					argb* argb_color_1 = (argb*)&color_1;
					int sum_0 = argb_color_0->r + argb_color_0->g + argb_color_0->b;
					int sum_1 = argb_color_1->r + argb_color_1->g + argb_color_1->b;

					sum_yes_office += sum_0;
					sum_yes_office += sum_1;
				}
				logger::instance().log("sum_yes_office = "s + std::to_string(sum_yes_office));
				if (9 * 2 * 650 < sum_yes_office)
					return 5;
			}
			logger::instance().log("nothing"s);
			_fist_capture_processed = false;
			return 0;
		}
	private:
		bool reinit()
		{
			logger::instance().log(__func__);
			if (!_need_reinit)
				return false;

			if (_screen_bitmap)
			{
				DeleteObject(_screen_bitmap);
				_screen_bitmap = 0;
			}

			if (_memory_dc)
			{
				DeleteObject(_memory_dc);
				_memory_dc = 0;
			}

			if (_window_dc)
			{
				ReleaseDC(GetDesktopWindow(), _window_dc);
				_window_dc = 0;
			}

			if (_screen_dc)
			{
				ReleaseDC(nullptr, _screen_dc);
				_screen_dc = 0;
			}

			RECT rect{};
			GetWindowRect(GetDesktopWindow(), &rect);

			_width = rect.right - rect.left;
			_height = rect.bottom - rect.top;

			_screen_dc = GetDC(nullptr);
			_window_dc = GetDC(GetDesktopWindow());
			_memory_dc = CreateCompatibleDC(_window_dc);

			SetStretchBltMode(_window_dc, HALFTONE);
			auto cxs = GetSystemMetrics(SM_CXSCREEN);
			auto cys = GetSystemMetrics(SM_CYSCREEN);
			if (FALSE == StretchBlt(_window_dc, 0, 0, _width, _height, _screen_dc, 0, 0, cxs, cys, SRCCOPY))
			{
				//logger::instance().log("capturer::reinit: StretchBlt failed");
				return false;
			}

			_screen_bitmap = CreateCompatibleBitmap(_window_dc, _width, _height);
			if (!_screen_bitmap)
			{
				//logger::instance().log("capturer::reinit: CreateCompatibleBitmap failed");
				return false;
			}

			_buffer_size = _width * _height * sizeof(uint32_t);
			_buffer = std::make_unique<uint8_t[]>(_buffer_size);

			_need_reinit = false;
			return true;
		}
		void capture()
		{
			logger::instance().log(__func__);

			if (_need_reinit)
				reinit();

			SelectObject(_memory_dc, _screen_bitmap);
			if (!BitBlt(_memory_dc, 0, 0, _width, _height, _window_dc, 0, 0, SRCCOPY))
			{
				//logger::instance().log("capturer::capture: BitBlt failed");
				_need_reinit = true;
				return;
			}

			BITMAP bitmap{};
			if (!GetObjectW(_screen_bitmap, sizeof(bitmap), &bitmap))
			{
				//logger::instance().log("capturer::capture: GetObjectW failed");
				_need_reinit = true;
				return;
			}

			BITMAPINFOHEADER bi;
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = bitmap.bmWidth;
			bi.biHeight = bitmap.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = 32;
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			if (const auto lines = GetDIBits(_window_dc, _screen_bitmap, 0, (UINT)bitmap.bmHeight, _buffer.get(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);
				lines != _height)
			{
				//logger::instance().log("capturer::capture: GetDIBits failed");
				_need_reinit = true;
				return;
			}
		}
		void first_capture_process()
		{
			if (_fist_capture_processed) return;
			logger::instance().log(__func__);
			const int x_0 = 0;
			const int x_1 = 96;
			const int y_0 = _height - 1 - 96;
			const int y_1 = _height - 1 - 150;
			const int y_diff_blue_black_begin = 1;
			const int y_diff_blue_black_end = 38;
			const int y_diff_blue_white = 39;

			for (int y_blue = y_0; y_blue >= y_1; --y_blue)
			{
				const int y_black_begin = y_blue - y_diff_blue_black_begin;
				const int y_black_end = y_blue - y_diff_blue_black_end;
				const int y_white = y_blue - y_diff_blue_white;

				for (int x = x_0; x <= x_1; ++x)
				{
					const int idx_b = x + y_blue * _width;
					const int idx_bb = x + y_black_begin * _width;
					const int idx_be = x + y_black_end * _width;
					const int idx_w = x + y_white * _width;

					uint32_t color_b = ((uint32_t*)(_buffer.get()))[idx_b];
					uint32_t color_bb = ((uint32_t*)(_buffer.get()))[idx_bb];
					uint32_t color_be = ((uint32_t*)(_buffer.get()))[idx_be];
					uint32_t color_w = ((uint32_t*)(_buffer.get()))[idx_w];

					const bool b = is_blue(color_b);
					const bool bb = is_black(color_bb);
					const bool be = is_black(color_be);
					const bool w = is_white(color_w);

					if (b && bb && be && w)
					{
						_x_shift = x;
						_y_shift = _height - y_blue - 1;
						_fist_capture_processed = true;
						std::string str = "found: _x_shift="s + std::to_string(_x_shift) +
							", _y_shift="s + std::to_string(_y_shift);
						logger::instance().log(str);
						return;
					}
				}
			}

			//const int x_mult = 16;
			//const int y_mult = 9;
			//0x96 -> 96x150 -> blue
			//0x97 -> 96x151 -> black
			//0x134 -> 96x188 -> black
			//0x135 -> 96x189 -> white
		}
		bool is_blue(uint32_t color)
		{
			argb* argb_color = (argb*)&color;
			if (argb_color->r < 5)
				return false;
			if (argb_color->r > 14)
				return false;
			if (argb_color->g < 58)
				return false;
			if (argb_color->g > 61)
				return false;
			if (argb_color->b < 119)
				return false;
			if (argb_color->b > 125)
				return false;
			return true;
		}
		bool is_black(uint32_t color)
		{
			argb* argb_color = (argb*)&color;
			if (argb_color->r > 10)
				return false;
			if (argb_color->g > 10)
				return false;
			if (argb_color->b > 10)
				return false;
			return true;
		}
		bool is_white(uint32_t color)
		{
			argb* argb_color = (argb*)&color;
			if (argb_color->r < 230)
				return false;
			if (argb_color->g < 230)
				return false;
			if (argb_color->b < 230)
				return false;
			return true;
		}
	private:
		int _width{};
		int _height{};
		HDC _screen_dc{};
		HDC _window_dc{};
		HDC _memory_dc{};
		HBITMAP _screen_bitmap{};
		uint32_t _buffer_size{};
		std::unique_ptr<uint8_t[]> _buffer;
		bool _valid{};
		bool _need_reinit{ true };
		bool _fist_capture_processed{ false };
		int _x_shift{ 0 };
		int _y_shift{ 0 };
	};
}