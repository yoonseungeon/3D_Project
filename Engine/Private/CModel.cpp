#include "CModel.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CBone.h"
#include "CAnimation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
    , m_eType{ Prototype.m_eType }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes } // 얕은 복사
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials } // 얕은 복사
    , m_Bones{ Prototype.m_Bones }  // 일단 얕은 복사
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_Animations{ Prototype.m_Animations }
{
    for (auto& pAnimation : m_Animations)
        Safe_AddRef(pAnimation);

    for (auto& pBone : m_Bones)
        Safe_AddRef(pBone);

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}

HRESULT XM_CALLCONV CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    _uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    m_eType = eType;
    
    if (m_eType == MODEL::NONANIM)
        iFlag |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (m_pAIScene == nullptr)
        return E_FAIL;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    // aiScene에 mNumNodes 이런 것 없음 계층 구조로 최상위 부모만
    // 최상위 부모의 부모 인덱스는 -1로
    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;
    
    // Ready_Bones보다 나중에
    // 여기서 이 mesh가 필요로하는 뼈(node)의 인덱스를 보관해서
    if (FAILED(Ready_Meshes(PreTransformMatrix)))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

_int CModel::Get_BoneIndex(const _char* pBoneName)
{
    // 이름이 같은 CBone(node)의 인덱스를 리턴하는 함수

    // 객체가 필요한게 아니라서 인덱스가 필요
    _int iIndex = { -1 };

    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
        {
            ++iIndex;

            if (pBone->Compare_Name(pBoneName) == true)
                return true;

            return false;
        });

    if (iter == m_Bones.end())
        return -1;

    return iIndex;
}

void CModel::Play_Animation(_float fTimeDelta)
{
    /* 현재 애니메이션 이용하고 있는 뼈들의 TransformationMatrix를 갱신해준다.  */
    // 현재 애니메이션으로 가서 뼈들의 행렬을 업데이트 해준다.
    m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta);

    /* 위의 갱신이 끝났다면, 모든 뼈의 CombinedTransformationMatrix갱신한다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformMatrices(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    //메쉬가 갖고있는 어떤 머테리얼을 써야할지 인덱스 가지고 옴
    _uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
    if (iMaterialIndex >= m_iNumMaterials)
        return E_FAIL;

    // 꺼내온 머테리얼들 중 이 인덱스한테 셰이더 바인딩 시킴
    return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iIndex);
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    // mesh한테 자기가 필요한 뼈(node)의 인덱스를 들고 있어서 mesh에서 처리. 뼈 배열도 넘김
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT XM_CALLCONV CModel::Ready_Meshes(_fmatrix PreTransformMatrix)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, m_pAIScene->mMeshes[i], PreTransformMatrix);
        if (pMesh == nullptr)
            return E_FAIL;
        
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath);
        if (pMaterial == nullptr)
            return E_FAIL;
        
        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
{
    // 최상위 부모로 CBone 객체 하나 생성
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (pBone == nullptr)
        return E_FAIL;

    m_Bones.push_back(pBone);

    // 현재 내 위치가 자식한테는 부모 위치이니 size - 1
    _int iParent = static_cast<_int>(m_Bones.size()) - 1;

    // pAINode 자식이 몇 개인지 알 수 있음
    // 재귀로 모두 순회
    // 전위 순회(부모 -> 왼쪽 -> 오른쪽)
    // 전위 순회해야 vector를 처음부터 갱신할 때 부모부터 알아서 쭉 갱신됨.
    for (_uint i = 0; i < pAINode->mNumChildren; ++i)
    {
        Ready_Bones(pAINode->mChildren[i], iParent);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
        if (pAnimation == nullptr)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

CModel* XM_CALLCONV CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created: CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);
    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);
    m_Materials.clear();

    m_Importer.FreeScene(); // aiScene 해제하는 함수

    __super::Free();
}
