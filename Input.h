#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment( lib, "dinput8.lib" )
#include <dinput.h>
#include <directxmath.h>
#include <string>

using namespace DirectX;

class Input
{
private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseLastState;
	XMFLOAT2 _mouseMovement;

	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
	void Shutdown();

public:
	Input();
	bool Initialise(HINSTANCE hinst, HWND hwnd);

	//Input Functions
	bool IsEscapePressed();


	//Camera Controls
	//Zoom
	bool IsZPressed();
	bool IsXPressed();
	bool IsCPressed();
	//Look Movement
	bool IsWPressed();
	bool IsAPressed();
	bool IsSPressed();
	bool IsDPressed();

	bool IsQPressed();
	bool IsEPressed();

	bool HasMouseMoved();
	XMFLOAT2 GetMouseMove() { return _mouseMovement; }

	bool Update();

};