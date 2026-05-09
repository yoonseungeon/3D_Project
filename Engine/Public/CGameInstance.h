#pragma once

#include "CBase.h"

#pragma region 프로토타입
#include "CShader.h"
#include "CVIBuffer_Rect.h"
#include "CVIBuffer_Cube.h"
#include "CTexture.h"
#include "CMyModel.h"
#include "CImage.h"
#include "CMove.h"
#include "CNavigation.h"
#include "CCollider.h"
#include "CVIBuffer_Rect_Instance.h"
#include "CVIBuffer_Point_Instance.h"
#pragma endregion

NS_BEGIN(Engine)

// Engine namespace 안에 넣어줘야 함.
class CGraphic_Device;
class CTimer_Manager;
class CLevel_Manager;
class CPrototype_Manager;
class CObject_Manager;
class CRenderer;
class CPipeline;
class CInput_Device;
class CLight_Manager;
class CFont_Manager;
class CTarget_Manager;
class CShadow;

class CPicking_Manager;
class CCollision_Manager;
class CThread_Manager;

class CGameObject;

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
	void	Release_Engine();

	_float Random(_float fMin, _float fMax);
	VIEWPORT_SIZE Get_ViewportDesc() { return m_tViewportDesc; }
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

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);

#ifdef _DEBUG
	_uint Get_PrototypeCnt(_uint iLevelIndex);
	void Get_PrototypeTags(_uint iLevelIndex, vector<_wstring>& PrototypeTags);
#endif

#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr, CGameObject** ppOut = nullptr);
	void Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE eParallelMode);

	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
#ifdef _DEBUG
	size_t Get_NumLevels();
	const map<const _wstring, class CLayer*>* Get_MapLevelLayers();
#endif

#pragma endregion

#pragma region RENDERER
	void Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);

#ifdef _DEBUG
	void Add_DebugComponent(class CComponent* pComponent);
#endif
#pragma endregion

#pragma region PIPELINE
	const _float4x4* Get_Transform(D3DTS eState) const;
	const _float4x4* Get_Transform_Inverse(D3DTS eState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eState, _fmatrix StateMatrix);
#pragma endregion

#pragma region INPUT_DEVICE
	_bool Key_Pressing(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);
	_bool Key_Down(_ubyte byKeyID);

	_bool Mouse_Pressing(DIMB eMouse);
	_bool Mouse_Up(DIMB eMouse);
	_bool Mouse_Down(DIMB eMouse);

	_long	Get_DIMouseMove(DIMM eMouseState);

	const POINT Get_MouseClientPos();

#ifdef _DEBUG
	void Set_InputBlock(_bool bImGuiCapture);
#endif

#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition,
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _float2& vScale = _float2(1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f));
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region PICKING_MANAGER
	void Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir);
	_bool Picking_Object(COLLISION_RAY_INFO& tOutColInfo);
#pragma endregion

#pragma region SHADOW
	const _float4x4* Get_Shadow_Transform(D3DTS eState) const;
	HRESULT Add_ShadowLight(_uint iNumLevels, const SHADOW_LIGHT_DESC& ShadowDesc);
#pragma endregion

#pragma region COLLISION_MANAGER
	HRESULT Add_Collider(CCollider* pCollider);
	HRESULT Substract_Collider(CCollider* pCollider);
	_bool XM_CALLCONV Collision_Ray(_fvector vRayPos, _fvector vRayDir, COLLISION_RAY_INFO& tOutColInfo);
#pragma endregion

#pragma region THREAD_MANAGER
	void Add_Job(function<void()> func);
#pragma endregion

private:
	CGraphic_Device*	m_pGraphic_Device		= { nullptr };
	CTimer_Manager*		m_pTimer_Manager		= { nullptr };
	CLevel_Manager*		m_pLevel_Manager		= { nullptr };
	CPrototype_Manager* m_pPrototype_Manager	= { nullptr };
	CObject_Manager*	m_pObject_Manager		= { nullptr };
	CRenderer*			m_pRenderer				= { nullptr };
	CPipeline*			m_pPipeline				= { nullptr };
	CInput_Device*		m_pInput_Device			= { nullptr };
	CLight_Manager*		m_pLight_Manager		= { nullptr };
	CFont_Manager*		m_pFont_Manager			= { nullptr };
	CTarget_Manager*	m_pTarget_Manager		= { nullptr };
	CShadow*			m_pShadow				= { nullptr };

	CPicking_Manager*	m_pPicking_Manager		= { nullptr };
	CCollision_Manager* m_pCollision_Mananger	= { nullptr };
	CThread_Manager*	m_pThread_Manager		= { nullptr };

	VIEWPORT_SIZE m_tViewportDesc{};

protected:
	virtual void Free() override;
};

NS_END