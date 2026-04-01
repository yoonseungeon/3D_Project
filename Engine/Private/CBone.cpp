#include "CBone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
    // 이름 저장 
    strcpy_s(m_szName, pAINode->mName.data);

    // column-major라 row-major로 바꾸기 위해 전치까지
    memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    // 여기서 못채워서 나중에
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    // 부모 인덱스(CModel의 vector index)
    m_iParentIndex = iParentIndex;

    return S_OK;
}

void XM_CALLCONV CBone::Update_CombinedTransformMatrices(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
    // 부모 행렬 가져와서 자신의 최종 행렬 구하기

    // 최상위 부모에 PreTransformMatrix 적용하면
    // 모든 자식들이 최상위 부모 행렬이 들어가 다 PreTransformMatrix가 적용된다.
    if (-1 == m_iParentIndex)
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            PreTransformMatrix * XMLoadFloat4x4(&m_TransformationMatrix));
    else // 부모 인덱스 저장해놨으니 행렬 가져와서 계산
        XMStoreFloat4x4(&m_CombinedTransformationMatrix,
            XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
    {
        MSG_BOX("Failed to Created: CBone");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBone::Free()
{
    __super::Free();
}
