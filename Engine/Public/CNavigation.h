#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class CCell;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	struct NAVIGATION_DESC
	{
		// 현재 네비게이션을 이용하고자 하는 객체거 어떤 셀 안에 있는지
		// -1은 지형 안 탐
		_int				iCurrentCellIndex{};

		_float3				vObjectWorldPos{};

		// 지형의 월드 행렬
		const _float4x4* pParentMarix{ nullptr };
	};

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

private:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool isMove(_fvector vResultPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<CCell*>			m_Cells;
	_int					m_iCurrentCellIndex{};

	// 지형이 포인터를 넘겨주면 모두가 같은 행렬 사용
	static const _float4x4* m_pParentMatrixPtr;

#ifdef _DEBUG
private:
	class CShader* m_pShader{ nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();
	_bool Find_CurCell_Index(_float3& vWorldPos);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg);
protected:
	virtual void Free() override;
};

NS_END