#include "CSound_Manager.h"

#include <fmod.hpp>

CSound_Manager::CSound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CSound_Manager::Initialize()
{
    FMOD_RESULT FMODResult{};

    FMODResult = FMOD::System_Create(&m_pSystem);
    if (FMODResult != FMOD_OK)
        return E_FAIL;

    FMODResult = m_pSystem->init(m_iMaxSound, FMOD_INIT_NORMAL, nullptr);
    if (FMODResult != FMOD_OK)
        return E_FAIL;

    return S_OK;
}

void CSound_Manager::Update_Sound()
{
    if (m_pSystem != nullptr)
        m_pSystem->update();
}

HRESULT CSound_Manager::PlaySound_Loop(_uint iKey, _uint iChannelID, _float fVolume)
{
    auto iter = m_Sounds.find(iKey);
    if (iter == m_Sounds.end())
        return E_FAIL;

    if (iChannelID >= m_iMaxSound)
        return E_FAIL;

    if (m_pChannels[iChannelID] != nullptr)
    {
        m_pChannels[iChannelID]->stop();
        m_pChannels[iChannelID] = nullptr;
    }

    FMOD_RESULT  FMODResult = m_pSystem->playSound(iter->second, nullptr, true, &m_pChannels[iChannelID]);
    if (FMODResult != FMOD_OK)
        return E_FAIL;

    m_pChannels[iChannelID]->setMode(FMOD_LOOP_NORMAL);
    m_pChannels[iChannelID]->setVolume(fVolume);
    m_pChannels[iChannelID]->setPaused(false);

    return S_OK;
}

HRESULT CSound_Manager::PlaySound_Once(_uint iKey, _uint iChannelID, _float fVolume)
{
    auto iter = m_Sounds.find(iKey);
    if (iter == m_Sounds.end())
        return E_FAIL;

    if (iChannelID >= m_iMaxSound)
        return E_FAIL;

    if (m_pChannels[iChannelID] != nullptr)
    {
        m_pChannels[iChannelID]->stop();
        m_pChannels[iChannelID] = nullptr;
    }

    FMOD_RESULT  FMODResult = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannels[iChannelID]);
    if (FMODResult != FMOD_OK)
        return E_FAIL;

    m_pChannels[iChannelID]->setVolume(fVolume);

    return S_OK;
}

HRESULT CSound_Manager::StopSoundChannel(_uint iChannelID)
{
    if (iChannelID >= m_iMaxSound)
        return E_FAIL;

    if (m_pChannels[iChannelID] == nullptr)
        return E_FAIL;

    m_pChannels[iChannelID]->stop();
    m_pChannels[iChannelID] = nullptr;

    return S_OK;
}

HRESULT CSound_Manager::StopAll()
{
    for (_uint i = 0; i < m_iMaxSound; ++i)
    {
        if(m_pChannels[i]!=nullptr)
        {
            m_pChannels[i]->stop();
            m_pChannels[i] = nullptr;
        }
    }

    return S_OK;
}

HRESULT CSound_Manager::Set_ChannelVolume(_uint iChannelID, _float fVolume)
{
    if (iChannelID >= m_iMaxSound)
        return E_FAIL;

    if (m_pChannels[iChannelID] == nullptr)
        return E_FAIL;

    m_pChannels[iChannelID]->setVolume(fVolume);

    return S_OK;
}

_bool CSound_Manager::IsPlaying(_uint iChannelID)
{
    if (iChannelID >= m_iMaxSound)
        return false;

    if (m_pChannels[iChannelID] == nullptr)
        return false;

    _bool bIsPlaying = { false };

    m_pChannels[iChannelID]->isPlaying(&bIsPlaying);

    return bIsPlaying;
}

HRESULT CSound_Manager::Load_Sound(const string& strPath, _uint iKey)
{
    FMOD::Sound* pSound = nullptr;
    FMOD_RESULT FMODResult = m_pSystem->createSound(strPath.c_str(), FMOD_DEFAULT, nullptr, &pSound);

    if (FMODResult != FMOD_OK)
    {
        return E_FAIL;
    }

    auto pair = m_Sounds.emplace(iKey, pSound);

    if (pair.second == false)
    {
        MSG_BOX("CSound_Manager - Failed to add Sound");
        pSound->release();
        return E_FAIL;
    }

    return S_OK;
}

CSound_Manager* CSound_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSound_Manager* pInstance = new CSound_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CSound_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Manager::Free()
{
    for (auto& pair : m_Sounds)
        pair.second->release();
    m_Sounds.clear();

    if (m_pSystem != nullptr)
    {
        m_pSystem->close();
        m_pSystem->release();
        m_pSystem = nullptr;
    }

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
