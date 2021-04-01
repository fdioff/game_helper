
#include <windows.h>
#include "gui.h"
#include "configuration.h"
#include "framework.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show)
{
	gta::configuration::processor::instance();
	gta::gui::processor gui;
	return gui.run(instance, show);
}

