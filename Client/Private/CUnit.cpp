#include "CAbstractPlayer.h"

#include "CGameInstance.h"

#include "CPartObject.h"

#include "CInGameHPBar.h"

CUnit::CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInvenOwner{ pDevice, pContext }
{
}

CUnit::CUnit(const CUnit& Prototype)
    : CInvenOwner{ Prototype }
{
}

HRESULT CUnit::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUnit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUnit::Priority_Update(_float fTimeDelta)
{
    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CUnit::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }
}

void CUnit::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }

    Update_Recovery(fTimeDelta);
}

void CUnit::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }
}

HRESULT CUnit::Render()
{
    return S_OK;
}

void CUnit::AddEXP(_uint iEXP)
{
    if (m_tCurStat.iLevel >= m_tFinalStat.iLevel)
    {
        return;
    }

    m_tCurStat.iEXP += iEXP;

    while (m_tCurStat.iEXP >= m_tFinalStat.iEXP)
    {
        LevelUp();
        ++ m_iSkillPoint;
    }
}

void CUnit::AddHP(_int iHP)
{
    m_tCurStat.iHP += iHP;

    if (m_tCurStat.iHP > m_tFinalStat.iHP)
    {
        m_tCurStat.iHP = m_tFinalStat.iHP;
    }
    else if (m_tCurStat.iHP < 0)
    {
        m_tCurStat.iHP = 0;
    }
}

void CUnit::AddMP(_int iMP)
{
    m_tCurStat.iMP += iMP;

    if (m_tCurStat.iMP > m_tFinalStat.iMP)
    {
        m_tCurStat.iMP = m_tFinalStat.iMP;
    }
    else if (m_tCurStat.iMP < 0)
    {
        m_tCurStat.iMP = 0;
    }
}

void CUnit::Add_Recovery(_uint iConsumableHP, _uint iConsumableMP, _float fRemainTime)
{
    RECOVERY tRecovery{};
    tRecovery.fRemainTime = fRemainTime;
    tRecovery.fAccTime = { 1.f };
    tRecovery.iRemainHPRecovery = iConsumableHP;
    tRecovery.iHPRecoveryPerSec = iConsumableHP / static_cast<_uint>(tRecovery.fRemainTime);

    m_Recoverys.push_back(tRecovery);
}

void CUnit::Damaged(const DAMAGE_INFO& tDamageInfo)
{
    AddHP(-tDamageInfo.iDamage);
}

void CUnit::LevelUp()
{
    m_tCurStat.iEXP -= m_tFinalStat.iEXP;

    m_tBaseStat += m_tLevelUpStat;
    m_tCurStat += m_tLevelUpStat;

    m_tCurStat.iLevel += 1;

    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::EFFECT_LEVELUP));

    if (m_tCurStat.iLevel >= m_tFinalStat.iLevel)
    {
        m_tCurStat.iEXP = m_tFinalStat.iEXP;
    }

    SetFinalStat();
}

void CUnit::SetStat(UNIT_STAT& tStat
    , _int iLevel, _int iExp, _int iHp, _int iMp,
    _float fHpRec, _float fMpRec, _int iATK, _int iSkillAmp,
    _int iPen, _int iPenPct, _int iDef,
    _float fATKSpeed, _int iCool, _int iCrit, _float fSpeed)
{
    tStat.iLevel = iLevel;
    tStat.iEXP = iExp;
    tStat.iHP = iHp;
    tStat.iMP = iMp;

    tStat.fHPRecoveryP = fHpRec;
    tStat.fMPRecoveryP = fMpRec;

    tStat.iATKPower = iATK;
    tStat.iSkillAmp = iSkillAmp;
    tStat.iPenetrationDefense = iPen;
    tStat.iPenetrationDefensePercent = iPenPct;
    tStat.iDefense = iDef;
    tStat.fATKSpeed = fATKSpeed;
    tStat.iCoolDown = iCool;
    tStat.iCritical = iCrit;
    tStat.fSpeed = fSpeed;
}

void CUnit::AddStat(UNIT_STAT& tStat, const UNIT_STAT& tAddStat, _bool bAdd)
{
    if (bAdd == true)
    {
        tStat += tAddStat;
    }
    else
    {
        tStat -= tAddStat;
    }
}

void CUnit::SetFinalStat()
{
    ZeroMemory(&m_tFinalStat, sizeof(m_tFinalStat));

    m_tFinalStat += m_tBaseStat;
    m_tFinalStat += m_tItemStat;

    m_tFinalStat.iLevel = 20;

    if (m_tCurStat.iHP > m_tFinalStat.iHP)
    {
        m_tCurStat.iHP = m_tFinalStat.iHP;
    }
}

HRESULT CUnit::Initialize_Stat()
{
    return S_OK;
}

void CUnit::Update_Recovery(_float fTimeDelta)
{
    for (auto iter = m_Recoverys.begin(); iter != m_Recoverys.end();)
    {
        iter->fAccTime += fTimeDelta;
        iter->fRemainTime -= fTimeDelta;

        while (iter->fAccTime >= 1.f)
        {
            iter->fAccTime -= 1.f;

            _uint iHPRecover = iter->iHPRecoveryPerSec;

            if (iHPRecover > iter->iRemainHPRecovery)
            {
                iHPRecover = iter->iRemainHPRecovery;
            }

            AddHP(iHPRecover);
            iter->iRemainHPRecovery -= iHPRecover;
        }

        if (iter->fRemainTime <= 0.f)
        {
            iter = m_Recoverys.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

_bool XM_CALLCONV CUnit::IsInRange(_fvector vPos, _float fRange)
{
    _vector vTargetPos = XMVectorSetY(vPos, 0.f);

    _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
    vMyPos = XMVectorSetY(vMyPos, 0.f);

    _float fLength = XMVectorGetX(XMVector3Length(vMyPos - vTargetPos));

    if (fLength <= fRange)
        return true;

    return false;
}

_bool CUnit::IsUnitDead()
{
    return false;
}

void CUnit::Free()
{
    Safe_Release(m_pInGameHPBar);

    __super::Free();
}