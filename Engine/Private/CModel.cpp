#include "CModel.h"
#include "CMesh.h"
#include "CMaterial.h"

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
{
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
    
    if (FAILED(Ready_Meshes(PreTransformMatrix)))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
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

HRESULT XM_CALLCONV CModel::Ready_Meshes(_fmatrix PreTransformMatrix)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], PreTransformMatrix);
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
    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);
    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);
    m_Materials.clear();

    m_Importer.FreeScene(); // aiScene 해제하는 함수

    __super::Free();
}
