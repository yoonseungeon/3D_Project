#ifdef _DEBUG

#include "CCharData_Manager.h"

IMPLEMENT_SINGLETON(CCharData_Manager)

const CCharData_Manager::CHAR_SKIN_DESC& CCharData_Manager::Get_CharSkinInfo(CHAR_NAME eCharName)
{
    
    auto iter = m_CharSkins.find(eCharName);
    if(iter == m_CharSkins.end())
    {
        MSG_BOX("No Char_Skin_Info: CCharData_Manager");

        return CHAR_SKIN_DESC();
    }

    return iter->second;
}

CCharData_Manager::CCharData_Manager()
{
    if (FAILED(Initialize()))
        MSG_BOX("CCharData_Manager: Error");
}

HRESULT CCharData_Manager::Initialize()
{ 
    //LiDailin
    CHAR_SKIN_DESC tCharSkinInfo{};
    tCharSkinInfo.eCharName = CHAR_NAME::LIDAILIN;

    tCharSkinInfo.wstrSkinSmallTexTag = L"Prototype_Texture_PickLiDailinSkin";

    SKIN_DESC tSkinInfo{};
    tSkinInfo.wstrSkinName = L"Default";
    tSkinInfo.iSkinIdx = 0;
    tCharSkinInfo.Skins.push_back(tSkinInfo);

    tSkinInfo.wstrSkinName = L"Dragon_Dailin";
    ++tSkinInfo.iSkinIdx;
    tCharSkinInfo.Skins.push_back(tSkinInfo);

    m_CharSkins.emplace(tCharSkinInfo.eCharName,tCharSkinInfo);


    //Hyunwoo
    tCharSkinInfo.Skins.clear();

    tCharSkinInfo.eCharName = CHAR_NAME::HYUNWOO;

    tCharSkinInfo.wstrSkinSmallTexTag = L"Prototype_Texture_PickHynwooSkin";

    tSkinInfo.wstrSkinName = L"Default";
    tSkinInfo.iSkinIdx = 0;
    tCharSkinInfo.Skins.push_back(tSkinInfo);

    m_CharSkins.emplace(tCharSkinInfo.eCharName, tCharSkinInfo);


    return S_OK;
}

void CCharData_Manager::Free()
{
    __super::Free();
}

#endif