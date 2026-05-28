#include "CMove.h"

#include "CGameInstance.h"
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

    m_pNavigationCom = pDesc->pNavigationCom;
    Safe_AddRef(m_pNavigationCom);

    m_fSpeed = pDesc->fSpeed;

    return S_OK;
}

void CMove::Move_To_Pos(_float3 vPos, _bool bOperateNavi)
{
    m_iCurMoveFlag |= FLAG_MOVE_TO_POS;
    m_vMovePos = vPos;

    if (bOperateNavi == true && m_pNavigationCom == nullptr) {
        m_bOperateNavi = false;
    }
    else
    {
        m_bOperateNavi = bOperateNavi;
    }
}

void CMove::Stop_Move_To_Pos()
{
    m_iCurMoveFlag &= ~FLAG_MOVE_TO_POS;
}

void CMove::Go_Straight(_float fTimeDelta, _float fSpeed, _bool bOperateNavi)
{
    _vector vPosition = m_pTransform->Get_State(STATE::POSITION);
    _vector vLook = m_pTransform->Get_State(STATE::LOOK);

    vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

    if (bOperateNavi == false ||
        m_pNavigationCom != nullptr && m_pNavigationCom->isMove(vPosition) == true)
    {
        m_pTransform->Set_State(STATE::POSITION, vPosition);
    }
}

_bool CMove::Update_Move_To_Pos(_float fTimeDelta)
{
    if (m_iCurMoveFlag & FLAG_MOVE_TO_POS)
    {
        _vector vCurPos = m_pTransform->Get_State(STATE::POSITION);

        _vector vMovePos = XMVectorSetW(XMLoadFloat3(&m_vMovePos), 1.f);
        _vector vDistance = vMovePos - vCurPos;

        // 지형 클릭하고 내비메시 y값 차이 때문에 문제 생김 
        vDistance = XMVectorSetY(vDistance, 0.f);
        _float fDistanceSq = XMVectorGetX(XMVector3LengthSq(vDistance));

        _vector vDir = XMVector3Normalize(vDistance);

        _vector vMoveDistance = vDir * m_fSpeed * fTimeDelta;
        _float fMoveDistanceSq = XMVectorGetX(XMVector3LengthSq(vMoveDistance));

        m_pTransform->TurnAxisY(vDir, fTimeDelta, XMConvertToRadians(1080.f));

        if(fDistanceSq <= fMoveDistanceSq)
        {
            const _bool m_bMoveBlock = m_pNavigationCom->isMove(vMovePos);

            if(m_bOperateNavi == true && m_bMoveBlock || m_bOperateNavi == false)
            {
                m_pTransform->Set_Pos(vMovePos);
            }
            m_iCurMoveFlag &= ~FLAG_MOVE_TO_POS;

            return true;
        }

        _vector vNextPos = vCurPos + vMoveDistance;

        if (m_bOperateNavi == false)
        {
            m_pTransform->Set_Pos(vNextPos);
        }
        else if (m_bOperateNavi == true && m_pNavigationCom->isMove(vNextPos))
        {
            m_pTransform->Set_Pos(vNextPos);
        }
        else
        {
            m_iCurMoveFlag &= ~FLAG_MOVE_TO_POS;
            return true;
        }

        return false;
    }

    return true;
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
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pTransform);

    __super::Free();
}
