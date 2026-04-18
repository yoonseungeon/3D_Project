#include "CMyBone.h"

CMyBone::CMyBone()
{
}

HRESULT CMyBone::Initialize(const myNode* pMyNode, _int iParentIndex)
{
    // 이름 저장 
    strcpy_s(m_szName, pMyNode->mName.c_str());

    // column-major라 row-major로 바꾸기 위해 전치까지
    memcpy(&m_TransformationMatrix, &pMyNode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    // 여기서 못채워서 나중에
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    // 부모 인덱스(CModel의 vector index)
    m_iParentIndex = iParentIndex;

    // 기본 뼈 나눠서 저장
    XMVECTOR vScale{}, vRotation{}, vTransform{};
    XMMatrixDecompose(&vScale, &vRotation, &vTransform, XMLoadFloat4x4(&m_TransformationMatrix));

    XMStoreFloat3(&m_tLocalPos.vScale, vScale);
    XMStoreFloat4(&m_tLocalPos.vRotation, vRotation);
    XMStoreFloat3(&m_tLocalPos.vTranslation, vTransform);

    return S_OK;
}

void XM_CALLCONV CMyBone::Update_CombinedTransformMatrices(const vector<CMyBone*>& Bones, _fmatrix PreTransformMatrix)
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

void XM_CALLCONV CMyBone::Set_TransformationMatrix(_fmatrix TransformationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
}

CMyBone* CMyBone::Create(const myNode* pMyNode, _int iParentIndex)
{
    CMyBone* pInstance = new CMyBone();

    if (FAILED(pInstance->Initialize(pMyNode, iParentIndex)))
    {
        MSG_BOX("Failed to Created: CMyBone");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMyBone* CMyBone::Clone()
{
    return new CMyBone(*this);
}

void CMyBone::Free()
{
    __super::Free();
}
