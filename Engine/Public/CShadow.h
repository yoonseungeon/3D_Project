#pragma once

// 오로지 그림자를 그리기 위한 광원
// 뷰스페이스 행렬, 투영 행렬만 보관
// 카메라처럼 사용

#include "CBase.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	const _float4x4* Get_Transform(D3DTS eState) const { return &m_TransformStateMatrices[ETOUI(eState)]; }

private:
	HRESULT Initialize(_uint iNumLevels);

public:
	HRESULT Add_ShadowLight(_uint iNumLevels, const SHADOW_LIGHT_DESC& ShadowDesc);
	void Update_Shadow();
	void Clear_Shadow(_uint iNumLevels);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4x4			 m_TransformStateMatrices[ETOUI(D3DTS::END_DECO)] = {};

	_float4 m_vEye{}, m_vAt{}, m_vUp{};

	_int m_iCurLevelIndex{ -1 };

	vector<_uint> m_ShadowCount;

public:
	static CShadow* Create(_uint iNumLevels);
protected:
	virtual void Free() override;
};

NS_END