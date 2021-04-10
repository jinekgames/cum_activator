// 
// WinMain.h
// 
// a start file
// 
////////////////////////////////////////

#pragma warning (disable : 4996 )

#include "MainConsts.h"

#include <Mmsystem.h>
#include <string>
#include <cctype>

#include "Window.h"
#include "resource.h"



// START WINDOW NAME
#define START_WINDOW_NAME "saction "
#define W_START_WINDOW_NAME L"saction "



// char buffer (10 symbls)
char buffChar[11];



// WinMain function
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow) {

	try {

		// Register window class
		Window wnd;


		// Message pump

		// message variable
		MSG msg;
		// exit code
		BOOL gResult;
		// start loop time
		unsigned long startTick;
		// end loop time
		unsigned long endTick;


		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {

			startTick = GetTickCount64();

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while ((GetTickCount64() - startTick) < 50);

		}


		if (gResult == -1) {
			throw CHWND_LAST_EXCEPT();
		}

		return msg.wParam;

	}
	catch (const myException& e) {
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;

}
