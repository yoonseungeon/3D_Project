#pragma once

#include "CBase.h"

#pragma region 프로토타입
#include "CShader.h"
#include "CVIBuffer_Rect.h"
#include "CTexture.h"
#include "CModel.h"
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
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr, CGameObject** ppOut = nullptr);
	void Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE eParallelMode);

#ifdef _DEBUG
	size_t Get_NumLevels();
	const map<const _wstring, class CLayer*>* Get_MapLevelLayers();
#endif

#pragma endregion

#pragma region RENDERER
	void Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
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

#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
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
	CLight_Manager* m_pLight_Manager			= { nullptr };

	CThread_Manager*	m_pThread_Manager		= { nullptr };

protected:
	virtual void Free() override;
};

NS_END