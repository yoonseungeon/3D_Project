#include "CGameInstance.h"

#include "CGraphic_Device.h"
#include "CTimer_Manager.h"
#include "CLevel_Manager.h"
#include "CPrototype_Manager.h"
#include "CObject_Manager.h"
#include "CRenderer.h"
#include "CPipeline.h"
#include "CInput_Device.h"
#include "CLight_Manager.h"

#include "CThread_Manager.h"

#include "CGameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

#pragma region ENGINE
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (m_pTimer_Manager == nullptr)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (m_pPrototype_Manager == nullptr)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (m_pRenderer == nullptr)
		return E_FAIL;

	m_pPipeline = CPipeline::Create();
	if (nullptr == m_pPipeline)
		return E_FAIL;

	m_pThread_Manager = CThread_Manager::Create();
	if (m_pThread_Manager == nullptr)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (m_pInput_Device == nullptr)
		return E_FAIL;
	
	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (m_pLight_Manager == nullptr)
		return E_FAIL;
	
	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Parallel_Update(fTimeDelta);	
	while (!m_pObject_Manager->Is_Parallel_Update_Finished())
	{
		_bool bResult = m_pThread_Manager->DoMainWork();

		if (!bResult && !m_pObject_Manager->Is_Parallel_Update_Finished())
		{
			// 양보는 thread의 최대 하나의 스레드 스케줄링 time slice만큼 적용된다.
			SwitchToThread();
		}

	}

	m_pObject_Manager->Update(fTimeDelta);

	m_pPipeline->Update();

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw()
{
	_float4     vColor = _float4(0.f, 0.f, 1.f, 1.f);

	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(&vColor)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRenderer->Draw()))
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

void CGameInstance::Clear_Resources(_int iLevelIndex)
{
	if (iLevelIndex == -1) {
		return;
	}

	/*iLevelIndex용 자원을 정리한다. */
	m_pObject_Manager->Clear(iLevelIndex);
	m_pPrototype_Manager->Clear(iLevelIndex);
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pThread_Manager);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeline);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);

	DestroyInstance();
}
_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + static_cast<_float>(rand()) / RAND_MAX * (fMax - fMin);
}
#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_Timer(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_Timer(strTimerTag);
}
#pragma endregion

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Change_Level(_int iNewLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iNewLevelIndex, pNewLevel);
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(eType, iLevelIndex, strPrototypeTag, pArg);
}

#ifdef _DEBUG
_uint CGameInstance::Get_PrototypeCnt(_uint iLevelIndex)
{ 
	return m_pPrototype_Manager->Get_PrototypeCnt(iLevelIndex);
}

void CGameInstance::Get_PrototypeTags(_uint iLevelIndex, vector<_wstring>& PrototypeTags) {
	m_pPrototype_Manager->Get_PrototypeTags(iLevelIndex, PrototypeTags);
}
#endif
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg, CGameObject** ppOut)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg, ppOut);
}
void CGameInstance::Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE eParallelMode)
{
	m_pObject_Manager->Set_Parallel_Update_Mode(eParallelMode);
}

#ifdef _DEBUG
size_t CGameInstance::Get_NumLevels()
{
	return m_pObject_Manager->Get_NumLevels();
}

const map<const _wstring, CLayer*>* CGameInstance::Get_MapLevelLayers()
{
	return m_pObject_Manager->Get_MapLevelLayers();
}
#endif

#pragma endregion

#pragma region RENDERER
void CGameInstance::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
	m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}
#pragma endregion

#pragma region PIPELINE
const _float4x4* CGameInstance::Get_Transform(D3DTS eState) const
{
	return m_pPipeline->Get_Transform(eState);
}

const _float4x4* CGameInstance::Get_Transform_Inverse(D3DTS eState) const
{
	return m_pPipeline->Get_Transform_Inverse(eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeline->Get_CamPosition();
}

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix StateMatrix)
{
	m_pPipeline->Set_Transform(eState, StateMatrix);
}
#pragma endregion

#pragma region DInput

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Mouse_Pressing(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_bool CGameInstance::Mouse_Up(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Up(eMouse);
}

_bool CGameInstance::Mouse_Down(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Down(eMouse);
}

_long CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

const POINT CGameInstance::Get_MouseClientPos()
{
	return m_pInput_Device->Get_MouseClientPos();
}

#ifdef _DEBUG
void CGameInstance::Set_InputBlock(_bool bImGuiCapture)
{
	m_pInput_Device->Set_InputBlock(bImGuiCapture);
}
#endif

#pragma endregion

#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
#pragma endregion

#pragma region THREAD_MANAGER
void CGameInstance::Add_Job(function<void()> func)
{
	m_pThread_Manager->Add_Job(func);
}
#pragma endregion

void CGameInstance::Free()
{
	__super::Free();
}
