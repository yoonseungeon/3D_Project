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

class CInGame_Manager;

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
public:
	struct LoadedObj
	{
		wstring wstrClassName{};
		_float3 vSacle{};
		_float3 vPos{};
	};

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

	CInGame_Manager* m_pInGame_Manager{};

	_uint m_iCurPrototypeCnt{};
	_uint m_iPrePrototypeCnt{};
	vector<wstring> m_PrototypeTags;

private:
	void Show_GameObjects();
	void Show_Transform();

	void Map_Picking();
	void Show_Object_Prototype();

private:
	string WStringToUTF8(const std::wstring& wstr);
	wstring UTF8ToWString(const std::string& str);

private:
	void GamePlayLevel_Save();
	void GamePlayLevel_Load();
	void Create_Object(LoadedObj& tLoadedObj);

protected:
	virtual void Free() override;
};

NS_END

#endif
