#include "CPipeline.h"

CPipeline::CPipeline()
{
	for (size_t i = 0; i < ETOUI(D3DTS::END_DECO); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[i], XMMatrixIdentity());
	}
}

const _float4x4* CPipeline::Get_Transform(D3DTS eState) const
{
	return &m_TransformStateMatrices[ETOUI(eState)];
}

const _float4x4* CPipeline::Get_Transform_Inverse(D3DTS eState) const
{
	return &m_TransformStateInverseMatrices[ETOUI(eState)];
}

const _float4* CPipeline::Get_CamPosition() const
{
	return &m_vCamPosition;
}

void XM_CALLCONV CPipeline::Set_Transform(D3DTS eState, _fmatrix StateMatrix)
{
	XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(eState)], StateMatrix);
}

void CPipeline::Update()
{
	for (size_t i = 0; i < ETOUI(D3DTS::END_DECO); ++i)
	{
		// XMMatrixInverse(행렬식(역행렬 존재 여부 등), 행렬)
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformStateMatrices[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformStateInverseMatrices[ETOUI(D3DTS::VIEW)]._41, sizeof(m_vCamPosition));
}

CPipeline* CPipeline::Create()
{
	return new CPipeline();
}

void CPipeline::Free()
{
	__super::Free();
}