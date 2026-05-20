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
#include "CFont_Manager.h"
#include "CTarget_Manager.h"
#include "CShadow.h"
#include "CFrustum.h"

#include "CSound_Manager.h"
#include "CPicking_Manager.h"
#include "CCollision_Manager.h"
#include "CThread_Manager.h"

#include "CGameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

#pragma region ENGINE
HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_tViewportDesc = { EngineDesc.iViewportWidth, EngineDesc.iViewportHeight };

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

	// Render 생성보다 위에 있어야 함.
	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (m_pRenderer == nullptr)
		return E_FAIL;

	m_pPipeline = CPipeline::Create();
	if (m_pPipeline == nullptr)
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
	
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (m_pFont_Manager == nullptr)
		return E_FAIL;

	m_pCollision_Mananger = CCollision_Manager::Create();
	if (m_pCollision_Mananger == nullptr)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create();
	if (m_pPicking_Manager == nullptr)
		return E_FAIL;

	m_pShadow = CShadow::Create(EngineDesc.iNumLevels);
	if (m_pShadow == nullptr)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (m_pFrustum == nullptr)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create(*ppDevice, *ppContext);
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	m_pPicking_Manager->Update_Picking_Manager();

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

	m_pShadow->Update_Shadow();
	m_pPipeline->Update();
	m_pFrustum->Update();

	m_pCollision_Mananger->Update_Collision();

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);

	m_pSound_Manager->Update_Sound();
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

void CGameInstance::Clear_ResourcesPrevious()
{
#ifdef _DEBUG
	m_pRenderer->Clear_Debug();
#endif

	m_pCollision_Mananger->Clear();
	m_pPicking_Manager->Clear();
}

void CGameInstance::Clear_Resources(_int iLevelIndex)
{
	if (iLevelIndex == -1) {
		return;
	}

	/*iLevelIndex용 자원을 정리한다. */
	m_pObject_Manager->Clear(iLevelIndex);
	m_pPrototype_Manager->Clear(iLevelIndex);

	// 그림자 광원 제거
	m_pShadow->Clear_Shadow(iLevelIndex);
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pThread_Manager);
	
	Safe_Release(m_pFrustum);
	Safe_Release(m_pShadow);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pCollision_Mananger);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeline);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pSound_Manager); // Level Manager보다 늦게 호출
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

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
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

#ifdef _DEBUG
void CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	m_pRenderer->Add_DebugComponent(pComponent);
}
#endif
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
HRESULT CGameInstance::Render_Light(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, const _float2& vScale, _float fRotation, const _float2& vOrigin)
{
	return m_pFont_Manager->Draw(strFontTag, pText, vPosition, vColor, vScale, fRotation, vOrigin);
}
_float2 CGameInstance::Cal_FontLength(const _wstring& strFontTag, const _wstring& Text)
{
	return m_pFont_Manager->Cal_Length(strFontTag, Text);
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

#ifdef _DEBUG

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif

#pragma region PICKING_MANAGER
HRESULT CGameInstance::Add_PickingCollider(CCollider* pCollider)
{
	return m_pPicking_Manager->Add_PickingCollider(pCollider);
}
void CGameInstance::Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir)
{
	m_pPicking_Manager->Get_WorldRay(vOutRayPos, vOutRayDir);
}
_bool CGameInstance::Picking_Object(COLLISION_RAY_INFO& tOutColInfo)
{
	return m_pPicking_Manager->Picking_Object(tOutColInfo);
}
#pragma endregion

#pragma region SHADOW
const _float4x4* CGameInstance::Get_Shadow_Transform(D3DTS eState) const
{
	return m_pShadow->Get_Transform(eState);
}

HRESULT CGameInstance::Add_ShadowLight(_uint iNumLevels, const SHADOW_LIGHT_DESC& ShadowDesc)
{
	return m_pShadow->Add_ShadowLight(iNumLevels, ShadowDesc);
}
#pragma endregion

#pragma region FRUSTUM	
_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRange);
}
#pragma endregion

#pragma region SOUND_KEY MANAGER	
HRESULT CGameInstance::PlaySound_Loop(_uint iKey, _uint iChannelID, _float fVolume)
{
	return m_pSound_Manager->PlaySound_Loop(iKey, iChannelID, fVolume);
}

HRESULT CGameInstance::PlaySound_OnceFixed(_uint iKey, _uint iChannelID, _float fVolume)
{
	return m_pSound_Manager->PlaySound_OnceFixed(iKey, iChannelID, fVolume);
}

HRESULT CGameInstance::PlaySound_Once(_uint iKey, _float fVolume)
{
	return m_pSound_Manager->PlaySound_Once(iKey, fVolume);
}

HRESULT CGameInstance::StopSoundChannel(_uint iChannelID)
{
	return m_pSound_Manager->StopSoundChannel(iChannelID);
}

HRESULT CGameInstance::StopAll()
{
	return m_pSound_Manager->StopAll();
}

HRESULT CGameInstance::Set_ChannelVolume(_uint iChannelID, _float fVolume)
{
	return m_pSound_Manager->Set_ChannelVolume(iChannelID, fVolume);
}

_bool CGameInstance::IsPlaying(_uint iChannelID)
{
	return m_pSound_Manager->IsPlaying(iChannelID);
}

HRESULT CGameInstance::Add_Sound(const string& strPath, _uint iKey)
{
	return m_pSound_Manager->Add_Sound(strPath, iKey);
}

void CGameInstance::Clear_Sound()
{
	m_pSound_Manager->Clear_Sound();
}
#pragma endregion

#pragma region COLLISION_MANAGER
HRESULT CGameInstance::Add_Collider(CCollider* pCollider)
{
	return m_pCollision_Mananger->Add_Collider(pCollider);
}
HRESULT CGameInstance::Substract_Collider(CCollider* pCollider)
{
	return m_pCollision_Mananger->Substract_Collider(pCollider);
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
