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

public:
	HRESULT Add_ShadowLight(const SHADOW_LIGHT_DESC& ShadowDesc);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4x4			 m_TransformStateMatrices[ETOUI(D3DTS::END)] = {};

public:
	static CShadow* Create();
protected:
	virtual void Free() override;
};

NS_END