#include "CAbstractPlayer.h"

#include "CGameInstance.h"

#include "CPartObject.h"

CUnit::CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CUnit::CUnit(const CUnit& Prototype)
    : CContainerObject{ Prototype }
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

void CUnit::LevelUp()
{
    m_tBaseStat += m_tLevelUpStat;
    m_tCurStat += m_tLevelUpStat;

    m_tCurStat.iLevel += 1;
    m_tCurStat.iEXP = 0;
}

void CUnit::SetStat(UNIT_STAT& tStat
    , _ubyte iLevel, _ushort iExp, _ushort iHp, _ushort iMp,
    _float fHpRec, _float fMpRec, _ushort iATK, _ushort iSkillAmp,
    _ushort iPen, _ushort iPenPct, _ushort iDef,
    _float fATKSpeed, _ushort iCool, _ushort iCrit, _float fSpeed)
{
    tStat.iLevel = iLevel;
    tStat.iEXP = iExp;
    tStat.iHP = iHp;
    tStat.iMP = iMp;

    tStat.fHPRecovery = fHpRec;
    tStat.fMPRecovery = fMpRec;

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

void CUnit::SetFinalStat()
{
    ZeroMemory(&m_tFinalStat, sizeof(m_tFinalStat));

    m_tFinalStat += m_tBaseStat;
    m_tFinalStat += m_tItemStat;

    m_tFinalStat.iLevel = 20;
}

void CUnit::Free()
{
    __super::Free();
}