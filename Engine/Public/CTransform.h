#pragma once

#include "CComponent.h"

/* 1. 객체의 월드 상태를 표현해주는 상태변환행렬을 보관한다.(월드변환행렬) */
/* 2. 월드행렬의 상태 표현을 위한 여러 인터페이스를 보관한다. */

// Transform은 원형 객체(프로토타입)를 만들지 않고,
// 객체 사본 생성하면 자동으로 호출되어 만들어지도록 하려함.(사본 객체에 Transform 원본 생성)
// 객체 사본 생성시 Transform 컴포넌트는 무조건 포함하는 형태

// 이유 1. 대부분의 객체가 Transform 컴포넌트를 필요로 할 것이다.
// 이유 2. Transform은 Transform만을 위한 정보가 아니라,(Transform만을 위한 정보가 딱히 없다.)
//		   Transform을 이용하는 객체에 대한  종속된 정보를 저장할 것이기 때문에 원본 프로토타입을 만들지 않는다.

NS_BEGIN(Engine)

class CShader;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	struct TRANSFORM_DESC
	{
		_float		fSpeedPerSec = {};
		_float		fRotationPerSec = {};
	};

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	// Clone 생성 시 호출이 아닌, 객체 사본에 Transform 원본 생성 시 추가 초기화로 사용
	// Create -> Initialize_Prototype -> Initialize
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(CShader* pShader, const _char* pConstantName);

public:
	_vector				Get_State(STATE eState);
	void XM_CALLCONV	Set_State(STATE eState, _fvector vState);

	_float3 Get_Scaled();

	const _float4x4* Get_WorldMatrixPtr() const { return &m_WorldMatrix; }

public:
	void Set_Scale(_float fScaleX = 1.f, _float fScaleY = 1.f, _float fScaleZ = 1.f);
	void Scaling(_float fScaleX = 1.f, _float fScaleY = 1.f, _float fScaleZ = 1.f);

	void XM_CALLCONV Rotation(_fvector vAxis, _float fRadian);
	void XM_CALLCONV Turn(_fvector vAxis, _float fTimeDelta);

	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

	void XM_CALLCONV LookAt(_fvector vAt);

private:
	_float4x4	m_WorldMatrix		= {};
	_float		m_fSpeedPerSec		= {};
	_float		m_fRotationPerSec	= {};

public:
	static CTransform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END