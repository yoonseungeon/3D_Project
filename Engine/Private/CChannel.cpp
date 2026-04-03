#include "CChannel.h"

#include "CModel.h"
#include "CBone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
    // CModel에서 CBone의 이름을 검색해서 인덱스 가지고 옴.
    m_iBoneIndex = pModel->Get_BoneIndex(pAINodeAnim->mNodeName.data);

    m_iNumKeyFrames = max(pAINodeAnim->mNumScalingKeys, pAINodeAnim->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAINodeAnim->mNumPositionKeys);

    _float3 vScale{};
    _float4 vRotation{};
    _float3 vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames; ++i)
    {
        KEYFRAME KeyFrame{};

        if (i < pAINodeAnim->mNumScalingKeys)
        {
            memcpy(&vScale, &pAINodeAnim->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mScalingKeys[i].mTime);
        }
        if (i < pAINodeAnim->mNumRotationKeys)
        {
            // memcpy(&vRotation, &pAINodeAnim->mRotationKeys[i].mValue, sizeof(_float4));
            vRotation.x = pAINodeAnim->mRotationKeys[i].mValue.x;
            vRotation.y = pAINodeAnim->mRotationKeys[i].mValue.y;
            vRotation.z = pAINodeAnim->mRotationKeys[i].mValue.z;
            vRotation.w = pAINodeAnim->mRotationKeys[i].mValue.w;
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mRotationKeys[i].mTime);
        }
        if (i < pAINodeAnim->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAINodeAnim->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAINodeAnim->mPositionKeys[i].mTime);
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyIndex)
{
    // 한 번 끝나고 다시 재생할 때 왼쪽 키프레임 인덱스 초기화
    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyIndex = 0;

    KEYFRAME LastKeyFrameDesc = m_KeyFrames.back();

    _vector vScale{};
    _vector vRotation{};
    _vector vTranslation{};

    // 마지막 키프레임일 때, 남은 트랙까지는 마지막 키프레임 상태로 유지시켜야 함.
    // 여러 번 넘어갔으면 계속 증가.(프레임 떨어질 (현재 - 왼쪽) 값이 엄청 커져서, 선형 보간에 문제 생김)
    if (fCurrentTrackPosition >= LastKeyFrameDesc.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrameDesc.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrameDesc.vRotation);
        // 위치 벡터 w 1
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrameDesc.vTranslation), 1.f);
    }
    else /* 무조건 보간이 필요한 상태. */
    {
        // 다음 키프레임으로 넘어가면 왼쪽 키프레임 증가
        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyIndex + 1].fTrackPosition)
            ++*pCurrentKeyIndex;

        _vector vLeftScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex].vScale);
        _vector vRightScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex + 1].vScale);

        _vector vLeftRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyIndex].vRotation);
        _vector vRightRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyIndex + 1].vRotation);

        _vector vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex].vTranslation), 1.f);
        _vector vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyIndex + 1].vTranslation), 1.f);

        // 왼쪽과 오른쪽 키프레임 사이에서의 위치 비율. (현재 - 왼쪽)/(오른쪽 - 왼쪽)
        _float  fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyIndex].fTrackPosition) /
            (m_KeyFrames[*pCurrentKeyIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyIndex].fTrackPosition);

        // 보간 함수 제공
        vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio); // (vLeftScale + vRightScale) * fRatio;
        // 회전(사원수) 일반 선형 보간하면 안된다. 
        vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
        vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
    }

    // 한 번에 하는 함수 제공
    /*_matrix     TransformationMatrix =
        XMMatrixScaling(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale)) *
        XMMatrixRotationQuaternion(vRotation) *
        XMMatrixTranslation(XMVectorGetX(vTranslation), XMVectorGetY(vTranslation), XMVectorGetZ(vTranslation));*/

        // 크기, 회전 중심, 회전, 위치
    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    // 뼈(node)의 행렬(TransformationMatrix) 업데이트
    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAINodeAnim, class CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAINodeAnim, pModel)))
    {
        MSG_BOX("Failed to Created: CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
