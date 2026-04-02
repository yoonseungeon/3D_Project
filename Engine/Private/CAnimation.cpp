#include "CAnimation.h"
#include "CChannel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, CModel* pModel)
{
    m_fDuration = static_cast<_float>(pAIAnimation->mDuration);
    m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);

    m_iNumChannels = pAIAnimation->mNumChannels;

    for (size_t i = 0; i < m_iNumChannels; ++i)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (pChannel == nullptr)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    for (auto& pChannel : m_Channels)
    {
        //pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
    }
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, CModel* pModel)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created: CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
    for (auto& pChannel : m_Channels) {
        Safe_Release(pChannel);
    }
    m_Channels.clear();

    __super::Free();
}
