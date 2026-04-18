#include "CMyAnimation.h"
#include "CMyChannel.h"

CMyAnimation::CMyAnimation()
{
}

CMyAnimation::CMyAnimation(const CMyAnimation& Prototype)
    : m_fDuration{ Prototype.m_fDuration }
    , m_fTickPerSecond{ Prototype.m_fTickPerSecond }
    , m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
    , m_iNumChannels{ Prototype.m_iNumChannels }
    , m_Channels{ Prototype.m_Channels }
    , m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
    // 채널은 갱신되지 않기 때문에 얕은 복사
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CMyAnimation::Initialize(const myAnimation* pMyAnimation, CMyModel* pModel)
{
    m_fDuration = static_cast<_float>(pMyAnimation->mDuration);
    m_fTickPerSecond = static_cast<_float>(pMyAnimation->mTickPerSecond);

    m_iNumChannels = pMyAnimation->mNumChannels;

    // 채널 개수만큼 왼쪽 키프레임 인덱스 공간 확보
    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; ++i)
    {
        CMyChannel* pChannel = CMyChannel::Create(&(pMyAnimation->mChannels[i]), pModel);
        if (pChannel == nullptr)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

_bool CMyAnimation::Update_TransformationMatrices(const vector<CMyBone*>& Bones, _float fTimeDelta, _bool isLoop)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAniSpeed;

    // 애니메이션 끝났는지
    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        // 무한 재생이 아니면
        if (isLoop == false)
        {
            // 그냥 끝
            m_fCurrentTrackPosition = m_fDuration;
            return true;
        }
        else // 무한 재생이면 다시 처음부터 재생
        {
            m_fCurrentTrackPosition = 0.f;
        }
    }

    _uint iChannelIndex{};

    for (auto& pChannel : m_Channels)
    {
        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iChannelIndex++]);
    }

    return false;
}

void CMyAnimation::Update_TransformationMatZeorKeyFrame(const vector<CMyBone*>& Bones)
{
    _uint iDumy{ 0 };

    for (auto& pChannel : m_Channels)
    {
        pChannel->Update_TransformationMatrix(Bones, 0.f, &iDumy);
    }
}

void CMyAnimation::Reset_KeyFrameIndex()
{
    for (auto& iKeyFrameIdx : m_CurrentKeyFrameIndices) {
        iKeyFrameIdx = 0;
    }
}

_bool CMyAnimation::Update_OverlayBones(const vector<CMyBone*>& Bones, const unordered_set<string>& OverlayBoneNames, _float fTimeDelta, _bool isLoop)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAniSpeed;

    // 애니메이션 끝났는지
    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        // 무한 재생이 아니면
        if (isLoop == false)
        {
            // 그냥 끝
            m_fCurrentTrackPosition = m_fDuration;
            return true;
        }
        else // 무한 재생이면 다시 처음부터 재생
        {
            m_fCurrentTrackPosition = 0.f;
        }
    }

    _int iChannelIndex{ -1 };

    for (auto& pChannel : m_Channels)
    {
        ++iChannelIndex;

        auto iter = OverlayBoneNames.find(pChannel->Get_ChannelName());
       
        if (iter == OverlayBoneNames.end()) {
            continue;
        }

        pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iChannelIndex]);
    }

    return false;
}

CMyAnimation* CMyAnimation::Create(const myAnimation* pMyAnimation, CMyModel* pModel)
{
    CMyAnimation* pInstance = new CMyAnimation();

    if (FAILED(pInstance->Initialize(pMyAnimation, pModel)))
    {
        MSG_BOX("Failed to Created: CMyAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMyAnimation* CMyAnimation::Clone()
{
    return new CMyAnimation(*this);
}

void CMyAnimation::Free()
{
    for (auto& pChannel : m_Channels) {
        Safe_Release(pChannel);
    }
    m_Channels.clear();

    __super::Free();
}
