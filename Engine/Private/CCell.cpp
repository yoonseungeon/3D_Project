#include "CCell.h"
#ifdef _DEBUG
#include "CVIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	// 자신의 인덱스
	m_iIndex = iIndex;

	// 삼각형 저장
	memcpy(m_vPoints, pPoints, sizeof(_float3) * ETOUI(CELL_POINT::END));

	// 한 변의 수직인 법선 구하기(반시계)
	m_vNormals[ETOUI(LINE::AB)] = _float3((m_vPoints[ETOUI(CELL_POINT::B)].z - m_vPoints[ETOUI(CELL_POINT::A)].z) * -1.f, 0.f, m_vPoints[ETOUI(CELL_POINT::B)].x - m_vPoints[ETOUI(CELL_POINT::A)].x);
	m_vNormals[ETOUI(LINE::BC)] = _float3((m_vPoints[ETOUI(CELL_POINT::C)].z - m_vPoints[ETOUI(CELL_POINT::B)].z) * -1.f, 0.f, m_vPoints[ETOUI(CELL_POINT::C)].x - m_vPoints[ETOUI(CELL_POINT::B)].x);
	m_vNormals[ETOUI(LINE::CA)] = _float3((m_vPoints[ETOUI(CELL_POINT::A)].z - m_vPoints[ETOUI(CELL_POINT::C)].z) * -1.f, 0.f, m_vPoints[ETOUI(CELL_POINT::A)].x - m_vPoints[ETOUI(CELL_POINT::C)].x);

	// 법선 정규화
	for (size_t i = 0; i < ETOUI(LINE::END); ++i)
		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;
#endif

	return S_OK;
}
_bool XM_CALLCONV CCell::isIn(_fvector vResultPos, _int* pNeighborIndex)
{
	// 세 변에 대한 검사
	for (size_t i = 0; i < ETOUI(LINE::END); ++i)
	{
		_vector vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_vPoints[i]));
		_vector vNormal = XMLoadFloat3(&m_vNormals[i]);

		// 내적해서 0보다 크면 셀 밖에 있음
		if (XMVectorGetX(XMVector3Dot(vDir, vNormal)) > 0)
		{
			// 이때의 이웃 인덱스 넣어줌
			*pNeighborIndex = m_iNeighbors[i];
			return false;
		}
	}

	return true;
}

_bool XM_CALLCONV CCell::Compare(_fvector vSourPoint, _fvector vDestPoint, LINE& eLine)
{
	// 두 점이 같은지 검사
	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::A)])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::B)])))
		{
			eLine = LINE::AB;
			return true;
		}
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::C)])))
		{
			eLine = LINE::CA;
			return true;
		}
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::B)])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::C)])))
		{
			eLine = LINE::BC;
			return true;
		}
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::A)])))
		{
			eLine = LINE::AB;
			return true;
		}
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::C)])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::A)])))
		{
			eLine = LINE::CA;
			return true;
		}
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[ETOUI(CELL_POINT::B)])))
		{
			eLine = LINE::BC;
			return true;
		}
	}

	return false;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created: CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
