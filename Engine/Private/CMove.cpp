#include "CMove.h"

#include "CTransform.h"

CMove::CMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CMove::CMove(const CMove& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CMove::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMove::Initialize(void* pArg)
{
    if (pArg == nullptr)
    {
        MSG_BOX("Need Transform Component: CMove");
        return E_FAIL;
    }

    MOVE_DESC* pDesc = static_cast<MOVE_DESC*>(pArg);

    m_pTransform = pDesc->pTransform;
    Safe_AddRef(m_pTransform);

    m_fSpeed = pDesc->fSpeed;

    return S_OK;
}

void CMove::Move_To_Pos(_float3 vPos)
{
    m_iCurMoveFlag |= FLAG_MOVE_TO_POS;
    m_vMovePos = vPos;
}

void CMove::Stop_Move_To_Pos()
{
    m_iCurMoveFlag &= ~FLAG_MOVE_TO_POS;
}

_bool CMove::Update_Move_To_Pos(_float fTimeDelta)
{
    if (m_iCurMoveFlag & FLAG_MOVE_TO_POS)
    {
        _vector vCurPos = m_pTransform->Get_State(STATE::POSITION);

        _vector vMovePos = XMVectorSetW(XMLoadFloat3(&m_vMovePos), 1.f);
        _vector vDistance = vMovePos - vCurPos;
        _float fDistanceSq = XMVectorGetX(XMVector3LengthSq(vDistance));

        _vector vDir = XMVector3Normalize(vDistance);

        m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);

        _vector vMoveDistance = vDir * m_fSpeed * fTimeDelta;
        _float fMoveDistanceSq = XMVectorGetX(XMVector3LengthSq(vMoveDistance));

        if(fDistanceSq <= fMoveDistanceSq)
        {
            m_pTransform->Set_Pos(vMovePos);
            m_iCurMoveFlag &= ~FLAG_MOVE_TO_POS;

            return true;
        }

        m_pTransform->Set_Pos(vCurPos + vMoveDistance);
        return false;
    }

    return false;
}

CMove* CMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMove* pInstance = new CMove(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CMove");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMove::Clone(void* pArg)
{
    CMove* pInstance = new CMove(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMove");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMove::Free()
{
    Safe_Release(m_pTransform);

    __super::Free();
}
