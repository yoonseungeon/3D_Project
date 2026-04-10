#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CCharData_Manager;

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	_bool	isFinished();

public:
	void Show_Loading_Status();

private:
	ID3D11Device*			m_pDevice			= { nullptr };
	ID3D11DeviceContext*	m_pContext			= { nullptr };

	CGameInstance*			m_pGameInstance		= { nullptr };

	LEVEL					m_eNextLevelID		= { LEVEL::END };

	HANDLE					m_hThread			= { };

private:
	atomic<_bool>		m_bIsAllJobsQueued		= { false };
	atomic<_int>		m_iTotalJobCnt			= { 0 };
	atomic<_int>		m_iFinishedJobCnt		= { 0 };

	CCharData_Manager* m_pCharData_Manager{};

private:
	HRESULT Ready_Resources_For_Static();

	HRESULT Ready_Resources_For_Logo();
	HRESULT Ready_Resources_For_Lobby();
	HRESULT Ready_Resources_For_GamePlay();
	HRESULT Ready_Resources_For_Ending();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
protected:
	virtual void Free() override;
};

NS_END