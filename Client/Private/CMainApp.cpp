#include "CMainApp.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"

#ifdef _DEBUG
#include "CImGui_Manager.h"
#endif

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iViewportWidth = g_iWinSizeX;
	EngineDesc.iViewportHeight = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX("CMainApp.cpp - Failed to Initialize : Engine");
		return E_FAIL;
	}

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

#ifdef _DEBUG
	m_pImGui_Manager = CImGui_Manager::GetInstance();
	m_pImGui_Manager->Initialize(m_pDevice, m_pContext);
#endif

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

#ifdef _DEBUG
	m_pImGui_Manager->Update(fTimeDelta);
#endif
}

HRESULT CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pImGui_Manager->Render())) 
		return E_FAIL;	
#endif

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	// Loading 리소스와 다음 Level의 리소스 Load.
	CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID);
	if (pLoadingLevel == nullptr)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize())) {
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pImGui_Manager);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);

	__super::Free();
}