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
    m_pTransformCom->Set_Rotation(0.f, static_cast<_float>(rand() % 360), 0.f);

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
    AddHP(-tDamageInfo.iDamage);
    if (tDamageInfo.pUnit != nullptr)
    {
        if (m_pTargetPlayer == nullptr) {
            m_pTargetPlayer = tDamageInfo.pUnit;
            Safe_AddRef(m_pTargetPlayer);
        }
    }

    if (m_tCurStat.iHP <= 0)
        m_iMonsterCondition |= MONSTER_CONDITION::CON_HPZERO;
}

_bool CAbstractMonster::IsUnitDead()
{
    return m_iMonsterCondition & MONSTER_CONDITION::CON_DEAD;
}

_bool XM_CALLCONV CAbstractMonster::IsInOpenRange(_fvector vPos, _float fWorldDistance)
{
    _matrix matWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    _matrix matWorldInverse = XMMatrixInverse(nullptr, matWorld);

    _vector vPlayerLocalPos = XMVector3TransformCoord(vPos, matWorldInverse);

    _float fX = XMVectorGetX(vPlayerLocalPos);
    _float fY = XMVectorGetY(vPlayerLocalPos);
    _float fZ = XMVectorGetZ(vPlayerLocalPos);

    MyHelper::FloatClamp(fX, tLocalMinMax.vMin.x, tLocalMinMax.vMax.x);
    MyHelper::FloatClamp(fY, tLocalMinMax.vMin.y, tLocalMinMax.vMax.y);
    MyHelper::FloatClamp(fZ, tLocalMinMax.vMin.z, tLocalMinMax.vMax.z);

    _vector vWorldClosePoint = XMVector3TransformCoord(XMVectorSet(fX, fY, fZ, 1.f), matWorld);

    // 2D로 볼 때 가장 가까운 거리는 아닌데... 쓸만함.
    _float fLength = XMVectorGetX(
        XMVector3Length(
            XMVectorSetY(vWorldClosePoint, 0.f) - XMVectorSetY(vPos, 0.f)
        )
    );

    if (fLength <= fWorldDistance)
        return true;

    return false;
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
_bool CAbstractMonster::IsNearSpawnPoint(_float fRange)
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

void CAbstractMonster::Cal_LocalMinMaxAABB(MODEL_LOCAL_MIN_MAX& tLocalMinMax, _float3& vCenter, _float3& vSize)
{
    tLocalMinMax.vMin = _float3(
        vCenter.x - vSize.x * 0.5f,
        vCenter.y - vSize.y * 0.5f,
        vCenter.z - vSize.z * 0.5f
    );

    tLocalMinMax.vMax = _float3(
        vCenter.x + vSize.x * 0.5f,
        vCenter.y + vSize.y * 0.5f,
        vCenter.z + vSize.z * 0.5f
    );
}

void CAbstractMonster::Free()
{
    Safe_Release(m_pTargetPlayer);
    Safe_Release(m_pInGame_Manager);

    __super::Free();
}