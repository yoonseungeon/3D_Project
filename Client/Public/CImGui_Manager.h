#pragma once
#ifdef _DEBUG

#include "Client_Defines.h"
#include "CBase.h"

#pragma region ImGui Header

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#ifdef _DEBUG
#ifndef new
#define new DBG_NEW
#endif
#endif

#pragma endregion

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Update(_float fTimeDelta);
	HRESULT Render();

	bool IsInputCapturedByUI();

private:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };

	CGameInstance*			m_pGameInstance{ nullptr };

private:
	void Show_GameObjects();
	void Show_Transform();

private:
	std::string WStringToUTF8(const std::wstring& wstr);
	std::wstring UTF8ToWString(const std::string& str);

protected:
	virtual void Free() override;
};

NS_END

#endif
