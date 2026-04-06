#include "CGame_Manager.h"

IMPLEMENT_SINGLETON(CGame_Manager)

CGame_Manager::CGame_Manager()
{

}

void CGame_Manager::Set_SelectChar(CHAR_NAME eCharName)
{
    m_tSelectedChar.eCharName = eCharName;
}

void CGame_Manager::Set_SelectSkin(const wstring& wstrSkinName)
{
    m_tSelectedChar.wstrSkinName = wstrSkinName;
}

void CGame_Manager::Set_SelectSpawnMap(MAP_NAME eSpawnMap)
{
    m_tSelectedChar.eSpawn_Map = eSpawnMap;
}

CHAR_NAME CGame_Manager::Get_SelectedChar()
{
    return m_tSelectedChar.eCharName;
}

MAP_NAME CGame_Manager::Get_SelectedMap()
{
    return m_tSelectedChar.eSpawn_Map;
}

void CGame_Manager::Free()
{
    __super::Free();
}