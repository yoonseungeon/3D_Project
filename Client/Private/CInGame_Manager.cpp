#include "CInGame_Manager.h"

#include "CLumia_Ground.h"
#include "CAbstractPlayer.h"

IMPLEMENT_SINGLETON(CInGame_Manager)

CInGame_Manager::CInGame_Manager()
{

}

void CInGame_Manager::Set_Map(CLumia_Ground* pMap_Lumia)
{
    if (m_pMap_Lumia == nullptr) {
        m_pMap_Lumia = pMap_Lumia;
        Safe_AddRef(m_pMap_Lumia);
    }
}

void CInGame_Manager::Release_Map()
{
    Safe_Release(m_pMap_Lumia);
    m_pMap_Lumia = nullptr;
}

_float3 CInGame_Manager::MapPIcking()
{
    _float3 vPos{};

    if (m_pMap_Lumia != nullptr) {
        vPos = m_pMap_Lumia->Picking();
    }

    return vPos;
}

void CInGame_Manager::Set_Player(CAbstractPlayer* pPlayer)
{
    if (m_pPlayer == nullptr) {
        m_pPlayer = pPlayer;
        Safe_AddRef(m_pPlayer);
    }
}

void CInGame_Manager::Release_Player()
{
    Safe_Release(m_pPlayer);
    m_pPlayer = nullptr;
}

_float3 CInGame_Manager::Get_PlayerPos()
{
    _float3 vPos{};

    if (m_pPlayer == nullptr)
    {
        return vPos;
    }

    const _float4x4* pWorldMatrix = m_pPlayer->Get_TransformCom()->Get_WorldMatrixPtr();

    memcpy(&vPos, (*pWorldMatrix).m[3], sizeof(_float3));

    return vPos;
}

void CInGame_Manager::Free()
{
    Safe_Release(m_pMap_Lumia);
    Safe_Release(m_pPlayer);

    __super::Free();
}