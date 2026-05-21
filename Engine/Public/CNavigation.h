#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class CCell;
class CTransform;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	struct NAVIGATION_DESC
	{
		_bool		bIsGround{};
		_float3		vObjectWorldPos{};

		// 지형의 월드 행렬
		const _float4x4* pParentMarix{ nullptr };

		CTransform* pTransformCom{};
	};

private:
	struct AStar_Info
	{
		_int ParentCellIndex{};
		_int CellIndex{};
		_float fF{};
		_float fG{};
		_float fH{};

		_bool operator < (const AStar_Info& rhs) {
			return fF > rhs.fF;
		}
	};

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

private:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile, const _tchar* pNeighborIndicesFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool isMove(_fvector vResultPos);
	void Compute_OnNavigation();

	const list<_vector>* Make_Route(_float3 vTargetPos);

	_int Get_CurAreaIndex();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	vector<CCell*>			m_Cells;
	_int					m_iCurrentCellIndex{};

	CTransform* m_pParentTransform{};
	// 지형이 포인터를 넘겨주면 모두가 같은 행렬 사용
	static const _float4x4* m_pParentMatrixPtr;


	_int m_iOldTargetCellIndex{ -1 };
	list<AStar_Info> CloseList;
	priority_queue<AStar_Info> OpenPriQ;
	list<_vector>				m_Route;

#ifdef _DEBUG
private:
	class CShader* m_pShader{ nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();
	HRESULT SetUp_Neighbors(const _tchar* pNeighborIndicesFilePath);

	_bool Find_CurCell_Index(_float3& vWorldPos);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile, const _tchar* pNeighborIndicesFilePath);
	virtual CComponent* Clone(void* pArg);
protected:
	virtual void Free() override;
};

NS_END