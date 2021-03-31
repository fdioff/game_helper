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
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
	public:
		processor()
		{
			static_assert(sizeof(argb) == sizeof(uint32_t));
			_valid = reinit();
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
		size_t check()
		{
			capture();

			// YES -> 3
			// NO -> 2
			// YES OFFICE -> 5
			// NO OFFICE -> 4
			// yes office 716-725 = 9
			// no office 753 - 762 = 9
			// no street 828 - 837 = 9
			// yes street 791 - 800 = 9

			const int x_0 = 42;
			const int x_1 = 141;

			{
				// NO STREET
				size_t sum_no_street{};
				for (int y = 828; y < 838; ++y)
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
				if (9 * 2 * 700 < sum_no_street)
					return 2;
			}
			{
				// YES STREET
				size_t sum_yes_street{};
				for (int y = 791; y < 801; ++y)
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
				if (9 * 2 * 700 < sum_yes_street)
					return 3;
			}
			{
				// NO OFFICE
				size_t sum_no_office{};
				for (int y = 753; y < 763; ++y)
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
				if (9 * 2 * 700 < sum_no_office)
					return 4;
			}
			{
				// YES OFFICE
				size_t sum_yes_office{};
				for (int y = 716; y < 726; ++y)
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
				if (9 * 2 * 700 < sum_yes_office)
					return 5;
			}

			return 0;
		}
	private:
		bool reinit()
		{
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
				return false;
			}

			_screen_bitmap = CreateCompatibleBitmap(_window_dc, _width, _height);
			if (!_screen_bitmap)
			{
				return false;
			}

			_buffer_size = _width * _height * sizeof(uint32_t);
			_buffer = std::make_unique<uint8_t[]>(_buffer_size);

			_need_reinit = false;
			return true;
		}
		void capture()
		{
			if (_need_reinit)
				reinit();

			SelectObject(_memory_dc, _screen_bitmap);
			if (!BitBlt(_memory_dc, 0, 0, _width, _height, _window_dc, 0, 0, SRCCOPY))
			{
				_need_reinit = true;
				return;
			}

			BITMAP bitmap{};
			if (!GetObjectW(_screen_bitmap, sizeof(bitmap), &bitmap))
			{
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
				static_cast<uint32_t>(lines) != _height)
			{
				_need_reinit = true;
				return;
			}
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
	};
}