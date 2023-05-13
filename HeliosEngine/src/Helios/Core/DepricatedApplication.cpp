/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "pch.h"

#include "DepricatedApplication.h"
#include "Time.h"
#include <Windows.h>
#include "Logger.h"
#include "Profiler.h"
#include "Helios/Input/InputManager.h"

#include "Helios/Scene/GameObject.h"

//Helios::DepricatedApplication::DepricatedApplication(bool fullscreen)
//{
//	GetMonitorInfo()
//}

HWND Helios::DepricatedApplication::hWnd = NULL;
Helios::DepricatedApplication* Helios::DepricatedApplication::instance = NULL;

float Helios::WindowCordinates::height = 600.0f;
float Helios::WindowCordinates::width = 800.0f;
float Helios::WindowCordinates::scale = 0.3888888888888889;

LRESULT Helios::DepricatedApplication::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		//DepricatedApplication::ShowMessage("Test Tawdaitkle", "before closing!");
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONUP:
		//if (currentGameSection) currentGameSection->OnClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_SIZE:
	{
		if (graphics) {
			SafeRelease(&graphics->m_mainRenderTarget);
			graphics->m_pSwapChain->ResizeBuffers(1, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			graphics->CreateD3D11RenderTarget();
			GameLoop();
		}
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		InputManager::s_MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) ? 1.0f : -1.0f;
		return 0;
	}
	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		break;
	}
}

LRESULT Helios::DepricatedApplication::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DepricatedApplication* pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (DepricatedApplication*)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;
	}
	else pThis = (DepricatedApplication*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (pThis)
		return pThis->WindowProc(uMsg, wParam, lParam);
	else return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Helios::DepricatedApplication::DepricatedApplication(int windowWidth, int windowHeight)
{
	WindowCordinates::width = 800;
	WindowCordinates::height = 600;
}

Helios::DepricatedApplication::~DepricatedApplication()
{
	if (graphics) delete graphics;
}

int Helios::DepricatedApplication::Run()
{
	std::ios::sync_with_stdio(false);
	DepricatedApplication::instance = this;
	
#pragma region Init Counter

	Time::Init();

#pragma endregion

#pragma region Check if the Window Class already exists and if not create it

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(GetModuleHandle(NULL), WindowClassName(), &wc)) {
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = DepricatedApplication::StaticWindowProc;
		wc.lpszClassName = WindowClassName();
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		
		RegisterClassEx(&wc);
	}

#pragma endregion

#pragma region Create Window

	// Calculate the Size to set for the actual window since client area always shrinks. This is done by AdjustWindowRect which is calculated by the os
	RECT rect = { 0, 0, WindowCordinates::width, WindowCordinates::height };
	AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

	m_hWnd = CreateWindowEx(
		dwExStyle, WindowClassName(), WindowTitle(), dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, GetModuleHandle(NULL), this
	);
	DepricatedApplication::hWnd = m_hWnd;

	if (!m_hWnd) {
		MessageBox(m_hWnd, L"Failed to Construct Window!", L"Error!", MB_ICONERROR);
		return -2;
	}

#pragma endregion

#pragma region Init Graphics

	graphics = new DepricatedGraphics(m_hWnd);
	if (!graphics->Init()) {		
		ShowMessage("Error initializing graphics!", (GetLastErrorAsString() + " : " + std::to_string(GetLastError())).c_str(), MB_ICONERROR);
		return -3;
	}

#pragma endregion

	Init();

	ShowWindow(m_hWnd, SW_SHOW);

#pragma region Game Loop

	MSG message;
	message.message = WM_NULL;

	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else GameLoop();
	}

#pragma endregion

	DepricatedApplication::ShowMessage("Test Titkle", "The app has quit game loop!");

	Shutdown();

	return 0;
}

void Helios::DepricatedApplication::ShowMessageBox(const wchar_t* title, const wchar_t* text, UINT type, bool pauseGame)
{
	MessageBox(pauseGame ? instance->m_hWnd : NULL, text, title, type);
}

void Helios::DepricatedApplication::ShowMessage(const wchar_t* title, const wchar_t* text, UINT type, bool pauseGame)
{
	MessageBox(pauseGame ? instance->m_hWnd : NULL, text, title, type);
}

void Helios::DepricatedApplication::ShowMessage(std::wstring title, std::wstring text, UINT type, bool pauseGame)
{
	MessageBox(pauseGame ? instance->m_hWnd : NULL, text.c_str(), title.c_str(), type);
}

void Helios::DepricatedApplication::ShowMessage(const char* title, const char* text, UINT type, bool pauseGame)
{
	ShowMessage(std::string(title), std::string(text), type, pauseGame);
}

void Helios::DepricatedApplication::ShowMessage(std::string title, std::string text, UINT type, bool pauseGame)
{
	MessageBox(pauseGame ? instance->m_hWnd : NULL, std::wstring(text.begin(), text.end()).c_str(), std::wstring(title.begin(), title.end()).c_str(), type);
}

void Helios::DepricatedApplication::GameLoop() {
	HL_PROFILE_FRAME_BEGIN();
	
	// Update Delta Time
	Time::frameUpdate();
	CheckEngineQueue();

	OnUpdate();
	OnRender();

	InputManager::s_MouseWheelDelta = 0.0f;

	HL_PROFILE_FRAME_END();
}

void Helios::DepricatedApplication::Shutdown()
{

}

void Helios::DepricatedApplication::CheckEngineQueue() {
	/*if (SceneRegistry::currentScene) {
		SceneRegistry::CheckQueue();
	}*/
}

void Helios::DepricatedApplication::OnUpdate() {
	SceneRegistry::OnRuntimeUpdate();
}

void Helios::DepricatedApplication::OnRender()
{
	graphics->ClearRenderTarget(0.0f, 0.0f, 0.5f);

	SceneRegistry::OnRuntimeRender();

	/*if (SceneRegistry::currentScene)
	{
		graphics->m_renderTarget2D->BeginDraw();

		SceneRegistry::Render(*graphics);

		Render(*graphics);

		graphics->m_renderTarget2D->EndDraw();
	}*/

	graphics->EndFrame();
}


void Helios::DepricatedApplication::Quit()
{
	PostMessage(DepricatedApplication::instance->m_hWnd, WM_CLOSE, 0, 0);
}