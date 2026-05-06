#include "CAbstractMonster.h"

#include "CInGame_Manager.h"

CAbstractMonster::CAbstractMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUnit{ pDevice, pContext }
{
}

CAbstractMonster::CAbstractMonster(const CAbstractMonster& Prototype)
    : CUnit{ Prototype }
{
}

HRESULT CAbstractMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAbstractMonster::Initialize(void* pArg)
{
    CABSTRACTMONSTER_DESC* pDesc = static_cast<CABSTRACTMONSTER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_vStartPos = pDesc->tTransformDesc.vStartPos;

    m_pInGame_Manager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pInGame_Manager);

    m_fBewareRange = 6.f;

    return S_OK;
}

void CAbstractMonster::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CAbstractMonster::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);
}

void CAbstractMonster::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CAbstractMonster::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CAbstractMonster::Render()
{
    return S_OK;
}

void CAbstractMonster::Damaged(const DAMAGE_INFO& tDamageInfo)
{
    AddHP(tDamageInfo.iDamage);
    if (tDamageInfo.pUnit != nullptr)
    {
        if (m_pTargetPlayer == nullptr) {
            m_pTargetPlayer = tDamageInfo.pUnit;
            Safe_AddRef(m_pTargetPlayer);
        }
    }
}

_bool CAbstractMonster::PlayerIsInRange(_float fRange)
{
    _float3 vTempPlayerPos = m_pInGame_Manager->Get_PlayerPos();
    vTempPlayerPos.y = 0.f;
    _vector vPlayerPos = XMLoadFloat3(&vTempPlayerPos);

    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    vMyPos = XMVectorSetY(vMyPos, 0.f);

    _float fLength = XMVectorGetX(XMVector3Length(vMyPos - vPlayerPos));

    if (fLength <= fRange)
        return true;

    return false;
}

// 함수 이름 바꾸기
_bool CAbstractMonster::IsInRange(_float fRange)
{
    _vector vStartPos = XMLoadFloat3(&m_vStartPos);
    vStartPos = XMVectorSetY(vStartPos, 0.f);

    _vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
    vCurPos = XMVectorSetY(vCurPos, 0.f);

    _float fLength = XMVectorGetX(XMVector3Length(vCurPos - vStartPos));

    if (fLength <= fRange)
        return true;

    return false;
}

void CAbstractMonster::Free()
{
    Safe_Release(m_pTargetPlayer);
    Safe_Release(m_pInGame_Manager);

    __super::Free();
}