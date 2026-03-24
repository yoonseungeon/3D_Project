#include "CInput_Device.h"

Engine::CInput_Device::CInput_Device(void)
{
}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(
		hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;;


	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 키보드 상태를 읽을 때 어떤 형식으로 받을지(DInput이 제공하는 표준 포맷 사용)
	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	// 장치에 대한 독점권을 설정해주는 함수. (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	if (FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		MSG_BOX("Failed to SetCooperativeLevel(KeyBoard): CInput_Device");
		return E_FAIL;
	}
	
	// 키보드 입력 받을 준비 끝났으니 장치를 활성화해라(입력 장치에 대한 접근 권한을 얻는다.)
	if (FAILED(m_pKeyBoard->Acquire()))
		return E_FAIL;



	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 마우스 상태를 읽을 때 어떤 형식으로 받을지(DInput이 제공하는 표준 포맷 사용)
	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	// 장치에 대한 독점권을 설정해주는 함수. (클라이언트가 떠있는 상태에서 마우스 입력을 받을지 말지를 결정하는 함수)
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		MSG_BOX("Failed to SetCooperativeLevel(Mouse): CInput_Device");
		return E_FAIL;
	}
	
	// 마우스 입력 받을 준비 끝났으니 장치를 활성화해라(입력 장치에 대한 접근 권한을 얻는다.)
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void Engine::CInput_Device::Update(void)
{
	// 입력 장치의 현재 상태를 한 번 읽어오는 함수
	if (FAILED(m_pKeyBoard->GetDeviceState(256, m_byKeyState)))
	{
		m_pKeyBoard->Acquire();
		m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	}

	if (FAILED(m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState)))
	{
		m_pMouse->Acquire();
		m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
	}
}

CInput_Device* CInput_Device::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Failed to Created: CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);

	__super::Free();
}

