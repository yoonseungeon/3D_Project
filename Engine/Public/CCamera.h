#pragma once

#include "CGameObject.h"

NS_BEGIN(Engine)

class CPipeline;

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	struct CAMERA_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vEye{};
		_float3 vAt{};
		_float fFovy{};
		_float fNear{};
		_float fFar{};
	};

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float4x4 m_ProjMatrix{};

	_float m_fFovy{};
	_float m_fAspect{};
	_float m_fNear{};
	_float m_fFar{};

	CPipeline* m_pPipeline{ nullptr };

protected:
	void Update_PipeLine();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END