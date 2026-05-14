#pragma once

#include "CVIBuffer.h"

// Instancing이 필요한 애들의 부모

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	struct INSTANCE_DESC
	{
		// 개수마다 원형 생성해줘야 함.
		_uint		iNumInstance{};

		_float3		vCenter{};
		_float3		vRange{};
		_float2		vSize{};
	};

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	// 인스턴스의 개수가 곧 정점의 개수임
	// 정점은 각각의 인스턴스를 컨트롤하기 위한 행렬
	_uint					m_iNumInstances = { };
	_uint					m_iInstanceStride = { };
	// 인스턴스당 인덱스 몇개 쓸지
	_uint					m_iIndexCountPerInstance = {};


	D3D11_BUFFER_DESC		m_InstanceBufferDesc = {};
	VTXPARTICLE_INSTANCE*	m_pInstanceVertices = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END