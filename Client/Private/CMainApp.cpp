#include "CMainApp.h"

#include "CGameInstance.h"
#include "CLevel_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};
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

	if (FAILED(Ready_Prototype_For_Static()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (FAILED(m_pGameInstance->Begin_Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

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

HRESULT CMainApp::Ready_Prototype_For_Static()
{
	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	// 내가 그릴려하는 정점의 정보(구조체 하나가 정점 안에 선언된 멤버 변수 하나 묘사)
	// 1. 문자열(시맨틱) 2. 시맨틱 idx(같은 시맨틱 여러 개면) 3. 포맷(float3 개, float 2개)
	// 4. 장치에 바인딩된 정 버퍼 중 몇 번째에서 꺼내와야 되는지 5. 구조체 멤버 변수 시작 주소
	// 6. VERTEX_DATA(일반적인 정점) or INSTANCE_DATA 7. INSTANCE_DATA인 StepRate 설정
	D3D11_INPUT_ELEMENT_DESC        Elements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), Elements, 2))))
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
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);

	__super::Free();
}