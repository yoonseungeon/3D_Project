#include "CSkillState.h"

CSkillState::CSkillState()
{
}

HRESULT CSkillState::Initialize()
{

    return S_OK;
}

void CSkillState::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
}

void CSkillState::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
}

void CSkillState::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}

_bool CSkillState::SkillLevelUp()
{
    if (m_iCurLevel >= m_iMaxLevel)
        return false;

    ++m_iCurLevel;
    return true;
}

void CSkillState::Free()
{
    __super::Free();
}