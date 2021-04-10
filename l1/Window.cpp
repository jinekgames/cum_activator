// 
// Window.cpp
// 
// a window class definision
// 
////////////////////////////////////////

#pragma warning (disable : 4996);

#include "Window.h"

#include <Mmsystem.h>
#include <string>
#include <sstream>
#include "resource.h"

#include "cumActivation.h"





///////////////////////////// HOOK

#include <cctype>
#include "cumActivation.h"


HWND hookHwnd;

HHOOK hk;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	//MessageBoxA(NULL, "Hook", "Hook", MB_OK);

	if (wParam == WM_KEYDOWN) {

		DWORD code = ((tagKBDLLHOOKSTRUCT*)lParam)->vkCode;

		if (isalpha(code)) {
			cumActivation::push(code + 'a' - 'A');
			//OutputDebugStringA(cumActivation::buf.c_str());
			//OutputDebugStringA("\n");
		}

		if (!(cumActivation::isPlaying) && cumActivation::check("cum")) {
			PlaySound(
				MAKEINTRESOURCE(IDI_MUS_CUM),
				0,
				SND_ASYNC | SND_RESOURCE
			);
			cumActivation::isPlaying = true;
		}

		if (cumActivation::isPlaying && cumActivation::check("nocum")) {
			PlaySound(NULL, 0, 0);
			cumActivation::isPlaying = false;
		}
		//PostMessage(hookHwnd, WM_CHAR, wParam, lParam);

	}

	return CallNextHookEx(hk, nCode, wParam, lParam);
}

void hookStart(HINSTANCE hinst) {
	hk = SetWindowsHookExA(WH_KEYBOARD_LL, (HOOKPROC)HookProc, hinst, NULL);
}

void hookStop() {
	UnhookWindowsHookEx(hk);
}

//////////////////////////////////////////





// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hinstance(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass(wndClassName, GetInstance());
}

LPCWSTR Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return wndClass.hinstance;
}


// Window Stuff
Window::Window() {
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = 10 + wr.left;
	wr.top = 100;
	wr.bottom = 10 + wr.top;


	// create window & get hwnd
	hwnd = CreateWindow(
		WindowClass::GetName(), L"cum window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// check for error
	if (hwnd == nullptr) {
		throw CHWND_LAST_EXCEPT();
	}


	// show window (i dnt need a window)
	//ShowWindow(hwnd, SW_SHOWDEFAULT);
}

Window::~Window() {
	DestroyWindow(hwnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE) {
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pwnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pwnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window instance handler
		return pwnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
	switch (msg) {
		case WM_CREATE:		// Initialisation
		{

			// creating hook
			hookStart( WindowClass::GetInstance() );

			return 0;
		} break;

		case WM_DESTROY:	// Closing
		{
			hookStop();
			PostQuitMessage(300);
			return 0;
		} break;
		case WM_CLOSE:
		{
			return SendMessage(hwnd, WM_DESTROY, 0, 0);
		} break;
		default:
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}



// Window Exception Stuff
Window::wndException::wndException(int line, const char* file, HRESULT hr) noexcept
	:
	myException(line, file),
	hr(hr)
{}

const char* Window::wndException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::wndException::GetType() const noexcept {
	return "jnk_gms wnd Exception";
}

std::string Window::wndException::TranslateErrorCode(HRESULT hr) noexcept {
	char* pMsgBuf = nullptr;

	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	// 0 string length returned indicates a failure
	if (nMsgLen == 0) {
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::wndException::GetErrorCode() const noexcept {
	return hr;
}

std::string Window::wndException::GetErrorString() const noexcept {
	return TranslateErrorCode(hr);
}
