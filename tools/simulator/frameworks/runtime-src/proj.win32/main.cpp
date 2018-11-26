#include "main.h"
#include "SimulatorWin.h"
#include <shellapi.h>

#ifndef APIENTRY //if include "cocos2d.h", directly or indirectly, this APIENTRY is missing, can't find the reason
#define APIENTRY WINAPI
#endif

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    return SimulatorWin::getInstance()->run();
}
