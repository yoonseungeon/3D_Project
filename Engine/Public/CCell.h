#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CVIBuffer_Cell;

class CCell final : public CBase
{

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

private:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);

public:
	_fvector Get_Point(CELL_POINT ePoint) { return XMLoadFloat3(&m_vPoints[ETOUI(ePoint)]); }

	void Set_Neighbor(LINE eLine, CCell* pNeighborCell) { m_iNeighbors[ETOUI(eLine)] = pNeighborCell->m_iIndex; }
	void Set_Neighbors(const _int* pNeighborIndices) { memcpy(m_iNeighbors, pNeighborIndices, sizeof(_int) * 3); }

	void Get_Neighbors(_int* pNeighborIndices) { memcpy(pNeighborIndices, m_iNeighbors, sizeof(_int) * 3); }

	_uint Get_CellIdx() { return m_iIndex; }

	const _float4& Get_Center() { return m_vCenter; }

	_bool XM_CALLCONV isIn(_fvector vResultPos, _int* pNeighborIndex);
	_bool XM_CALLCONV Compare(_fvector vSourPoint, _fvector vDestPoint, LINE& eLine);

	_float Compute_Height(_fvector vTargetPos);

	_float Compute_Cost(const _float4& vCenter);

	const _int* Get_NeighborIndices() { return m_iNeighbors; }

	void Set_AreaIndex(_uint iAreaIndex) { m_iAreaIndex = iAreaIndex; }
	_uint Get_AreaIndex() { return m_iAreaIndex; }

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3					m_vPoints[ETOUI(CELL_POINT::END_DECO)] = {};
	_float3					m_vNormals[ETOUI(LINE::END_DECO)] = {};
	_uint					m_iIndex = {};
	_int					m_iNeighbors[ETOUI(LINE::END_DECO)] = { -1, -1, -1 };
	_float4					m_vPlane = {};

	_float4					m_vCenter{};

	_uint					m_iAreaIndex{};

#ifdef _DEBUG
private:
	CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
protected:
	virtual void Free() override;
};

NS_END