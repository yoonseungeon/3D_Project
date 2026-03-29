#ifdef _DEBUG

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

CHAR_NAME CGame_Manager::Get_SelectedChar()
{
    return m_tSelectedChar.eCharName;
}

void CGame_Manager::Free()
{
    __super::Free();
}

#endif