#include "CInGame_Manager.h"

#include "CMap_Lumia.h"

IMPLEMENT_SINGLETON(CInGame_Manager)

CInGame_Manager::CInGame_Manager()
{

}

void CInGame_Manager::Set_Map(CMap_Lumia* pMap_Lumia)
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

void CInGame_Manager::Free()
{
    Safe_Release(m_pMap_Lumia);

    __super::Free();
}