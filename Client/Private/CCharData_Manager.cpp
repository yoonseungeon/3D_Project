#include "CCharData_Manager.h"

IMPLEMENT_SINGLETON(CCharData_Manager)

CCharData_Manager::CHAR_INFO_DESC* CCharData_Manager::Get_CharInfo(CHAR_NAME eCharName)
{
    auto iter = m_CharSkins.find(eCharName);

    if(iter == m_CharSkins.end())
    {
        MSG_BOX("No Char_Skin_Info: CCharData_Manager");

        return nullptr;
    }

    return &(iter->second);
}

CCharData_Manager::CCharData_Manager()
{
    if (FAILED(Initialize()))
        MSG_BOX("CCharData_Manager: Error");
}

HRESULT CCharData_Manager::Initialize()
{ 
    for (auto& Character : Characters)
    {
        CHAR_INFO_DESC tDesc{};
        tDesc.wstrCharacterName = Character.CharacterName;
        tDesc.eCharName = Character.eCharacterName;

        tDesc.wstrPickTag = Character.PickTag;

        tDesc.wstrFullSkinTag = Character.FullSkinTag;
        tDesc.wstrFullSkinPath = Character.FullSkinPath;

        tDesc.wstrSkinTag = Character.SkinTag;
        tDesc.wstrSkinPath = Character.SkinPath;

        tDesc.Skins.reserve(Character.SkinCnt);

        for (size_t i = 0; i < Character.SkinCnt; ++i)
        {
            const SKIN_META& SkinMeta = Character.pSkins[i];

            SKIN_INFO_DESC tSkinDesc{};
            tSkinDesc.wstrSkinName = SkinMeta.SkinName;
            tSkinDesc.eSkinClass = SkinMeta.eSkinClass;
            tSkinDesc.iSkinIdx = SkinMeta.SkinIdx;

            tDesc.Skins.push_back(tSkinDesc);
        }

        m_CharSkins.emplace(Character.eCharacterName, tDesc);
    }

    return S_OK;
}

void CCharData_Manager::Free()
{
    __super::Free();
}