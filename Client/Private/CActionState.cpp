#include "CActionState.h"

CActionState::CActionState()
{
}

void CActionState::Free()
{
    __super::Free();
}

void CActionState::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
}

void CActionState::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
}

void CActionState::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}
