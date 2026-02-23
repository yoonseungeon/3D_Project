#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

// Engine namespace 안에 넣어줘야 함.
class CGraphic_Device;
class CTimer_Manager;
class CLevel_Manager;
class CPrototype_Manager;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void	Update_Engine(_float fTimeDelta);
	HRESULT Begin_Draw();
	HRESULT Draw();
	HRESULT End_Draw();
	void	Clear_Resources(_int iLevelIndex);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);
#pragma endregion

private:
	CGraphic_Device*	m_pGraphic_Device		= { nullptr };
	CTimer_Manager*		m_pTimer_Manager		= { nullptr };
	CLevel_Manager*		m_pLevel_Manager		= { nullptr };
	CPrototype_Manager* m_pPrototype_Manager	= { nullptr };

protected:
	virtual void Free() override;
};

NS_END