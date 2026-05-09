#include "CCamera.h"
#include "CGameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewports{ 1 };

	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

	m_fNear = pDesc->fNear;
	m_fAspect = static_cast<_float>(ViewPortDesc.Width) / static_cast<_float>(ViewPortDesc.Height);
	m_fFar = pDesc->fFar;
	m_fFovy = pDesc->fFovy;

	/* 트랜스폼 컴에게 카메라의 초기 상태를 세팅해준다. */
	/* 트랜스폼이 들고 있는 월드행렬의 역 == 뷰스페이스 변환 행렬 */
	/* 추후 트랜스폼으로 카메라의 상태를 제어해주면. 뷰스페이스 변환을 갱신해주는 것과 마찬가지다. */
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vEye), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));


	/* 원근 투영행령를 만들어주기위한 데이터들을 멤버로 보관. */
	/* 게임 내에서 데이터들을 조작하여 언제든 원근 투영행렬을 컨트롤할 수 있도록 하겠다. */
	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	Update_PipeLine();

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
	Update_PipeLine();
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Update_PipeLine()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMLoadFloat4x4(&m_ProjMatrix));
}

void CCamera::Free()
{
	__super::Free();
}
