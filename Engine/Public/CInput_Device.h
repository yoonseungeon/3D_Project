#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class  CInput_Device final : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_bool Key_Pressing(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);
	_bool Key_Down(_ubyte byKeyID);

	_bool Mouse_Pressing(DIMB eMouse);
	_bool Mouse_Up(DIMB eMouse);
	_bool Mouse_Down(DIMB eMouse);

	// 현재 마우스의 특정 축 변화량을 반환
	_long	Get_DIMouseMove(DIMM eMouseState);

private:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);

public:
	void Update();

	const POINT Get_MouseClientPos();

private:
	LPDIRECTINPUT8			m_pInputSDK{ nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard{ nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse{ nullptr };

private:
	_byte			m_byCurKeyState[256]{};
	_byte			m_byPreKeyState[256]{};

	DIMOUSESTATE	m_tCurMouseState{};
	DIMOUSESTATE	m_tPreMouseState{};


	POINT			m_ptMouse{};
	HWND			m_hWnd{};

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
protected:
	virtual void Free(void) override;
};

NS_END