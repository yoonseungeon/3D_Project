#include "CVIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	: CComponent{ Prototype }
	, m_pVB{ Prototype.m_pVB }
	, m_pIB{ Prototype.m_pIB }
	, m_iNumVertexBuffers{ Prototype.m_iNumVertexBuffers }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
	, m_ePrimitiveType{ Prototype.m_ePrimitiveType }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint		iOffsets[] = {
		0,
	};

	// 1. 정적 버퍼 시작 슬롯 2. 정점 버퍼들의 배열 3. 각각의 정점 버퍼들의 정점 하나의 크기 배열 3. 각각의 버퍼의 시작 위치 배열
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	// 1. 인덱스 버퍼의 주소 2. 인덱스의 크기 3. 인덱스 버퍼의 시작 위치
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	// 1. 정점 데이터를 해석하는 방식 TRIANGLELIST, TRIANGLESTRIP 등
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	// 1. 그릴 인덱스의 개수 2. 인덱스 버퍼 시작 위치 3. 버텍스 버퍼 시작 위치
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	__super::Free();
}
