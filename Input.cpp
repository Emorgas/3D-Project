#include "Input.h"
#include <directxmath.h>

Input::Input()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
	_mouseMovement = { 0.0f, 0.0f };
}

bool Input::Initialise(HINSTANCE hinst, HWND hwnd)
{
	HRESULT hr;

	hr = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(hr))
		return false;

	//Keyboard
	hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(hr))
		return false;

	hr = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return false;

	hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return false;

	hr = m_keyboard->Acquire();
	if (FAILED(hr))
		if (hr == E_ACCESSDENIED)
		{
			hr = m_keyboard->Acquire();
		}
	if (FAILED(hr))
		return false;

	//Mouse
	hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(hr))
		return false;

	hr = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		return false;

	hr = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return false;

	hr = m_mouse->Acquire();
	if (FAILED(hr))
		return false;

	return true;

}

void Input::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool Input::Update()
{
	bool result;

	result = ReadKeyboard();

	if (!result)
		return false;

	result = ReadMouse();

	if (!result)
		return false;

	ProcessInput();

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT hr;

	hr = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else
			return false;
	}
	return true;

}

bool Input::ReadMouse()
{
	HRESULT hr;

	hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseLastState);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else
			return false;
	}
	return true;
}

bool Input::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}


//Camera Controls
//Zooming
bool Input::IsCPressed() //Zoom Out
{
	if (m_keyboardState[DIK_C] & 0x80)
	{
		return true;
	}
	return false;
}
bool Input::IsXPressed() //Reset Zoom
{
	if (m_keyboardState[DIK_X] & 0x80)
	{
		return true;
	}
	return false;
}
bool Input::IsZPressed() //Zoom In
{
	if (m_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}
	return false;
}

//Camera Look Movement
bool Input::IsWPressed()
{
	if (m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}

	return false;
}
bool Input::IsAPressed()
{
	if (m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}
	return false;
}
bool Input::IsSPressed()
{
	if (m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}
	return false;
}
bool Input::IsDPressed()
{
	if (m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}
	return false;
}
//Rotate Camera
bool Input::IsQPressed()
{
	if (m_keyboardState[DIK_Q] & 0x80)
	{
		return true;
	}
	return false;
}
bool Input::IsEPressed()
{
	if (m_keyboardState[DIK_E] & 0x80)
	{
		return true;

	}
	return false;
}

bool Input::HasMouseMoved()
{
	DIMOUSESTATE m_mouseCurrentState;
	m_mouse-> Acquire();
	m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrentState);
	if (m_mouseCurrentState.lX != m_mouseLastState.lX || m_mouseCurrentState.lY != m_mouseLastState.lY)
	{

		_mouseMovement.x = -(m_mouseCurrentState.lX - m_mouseLastState.lX);
		_mouseMovement.y = -(m_mouseCurrentState.lY - m_mouseLastState.lY);

		return true;
	}
	return false;
}

void Input::GetMouseLocation(int& x, int& y)
{
	DIMOUSESTATE m_mouseCurrentState;
	m_mouse->Acquire();
	m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrentState);

	x = m_mouseCurrentState.lX;
	y = m_mouseCurrentState.lY;
	return;
}

bool Input::IsLMBPressed()
{
#define BUTTONDOWN(name, key) (name.rgbButtons[key] & 0x80)

	DIMOUSESTATE m_mouseCurrentState;
	m_mouse->Acquire();
	m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrentState);

	if (BUTTONDOWN(m_mouseCurrentState, 0))
	{
		return true;
	}
	return false;
}
void Input::ProcessInput()
{
}

