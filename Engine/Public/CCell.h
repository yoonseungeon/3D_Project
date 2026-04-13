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
	_uint Get_CellIdx() { return m_iIndex; }

	_bool XM_CALLCONV isIn(_fvector vResultPos, _int* pNeighborIndex);
	_bool XM_CALLCONV Compare(_fvector vSourPoint, _fvector vDestPoint);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_float3					m_vPoints[ETOUI(CELL_POINT::END)] = {};
	_float3					m_vNormals[ETOUI(LINE::END)] = {};
	_uint					m_iIndex = {};
	_int					m_iNeighbors[ETOUI(LINE::END)] = { -1, -1, -1 };

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