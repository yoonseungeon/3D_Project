#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CPipeline final : public CBase
{
private:
	CPipeline();
	virtual ~CPipeline() = default;

public:
	const _float4x4* Get_Transform(D3DTS eState) const;
	const _float4x4* Get_Transform_Inverse(D3DTS eState) const;
	const _float4* Get_CamPosition() const;

public:
	void XM_CALLCONV Set_Transform(D3DTS eState, _fmatrix StateMatrix);

public:
	void Update();

private:
	_float4x4 m_TransformStateMatrices[ETOUI(D3DTS::END_DECO)]{};
	_float4x4 m_TransformStateInverseMatrices[ETOUI(D3DTS::END_DECO)]{};
	_float4	m_vCamPosition{};

public:
	static CPipeline* Create();
protected:
	virtual void Free() override;
};

NS_END