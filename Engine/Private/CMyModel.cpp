#include "CMyModel.h"

#include "CMyMesh.h"
#include "CMyMaterial.h"
#include "CMyBone.h"
#include "CMyAnimation.h"

CMyModel::CMyModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CMyModel::CMyModel(const CMyModel& Prototype)
    : CComponent{ Prototype }
    , m_eType{ Prototype.m_eType }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes } // 얕은 복사
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials } // 얕은 복사
    //, m_Bones{ Prototype.m_Bones }  // 일단 얕은 복사
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    //, m_Animations{ Prototype.m_Animations }
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnimation->Clone());

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}

HRESULT XM_CALLCONV CMyModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX)
{
    m_eType = eType;
    m_bStoreVtxIdx = bStoreVTXIDX;

    m_pMyScene = m_Importer.ReadFile(pModelFilePath);
    if (m_pMyScene == nullptr)
        return E_FAIL;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    // aiScene에 mNumNodes 이런 것 없음 계층 구조로 최상위 부모만
    // 최상위 부모의 부모 인덱스는 -1로
    if (FAILED(Ready_Bones(&(m_pMyScene->mRootNode), -1)))
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

HRESULT CMyModel::Initialize(void* pArg)
{
    return S_OK;
}

_int CMyModel::Get_BoneIndex(const _char* pBoneName)
{
    // 이름이 같은 CMyBone(node)의 인덱스를 리턴하는 함수

    // 객체가 필요한게 아니라서 인덱스가 필요
    _int iIndex = { -1 };

    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CMyBone* pBone)->_bool
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

const _float4x4* CMyModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CMyBone* pBone)->_bool
        {
            if (pBone->Compare_Name(pBoneName) == true)
            {
                return true;
            }

            return false;
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

void CMyModel::Set_AnimationIndex(_uint iIndex, _bool isLoop)
{
    m_iPreviousAnimationIndex = m_iCurrentAnimationIndex;

    m_bIsFinished = false;
    m_iCurrentAnimationIndex = iIndex;
    m_isAnimLoop = isLoop;    
    m_Animations[m_iCurrentAnimationIndex]->Reset_KeyFrameIndex();
    m_Animations[m_iCurrentAnimationIndex]->Reset_CurrentTrackPosition();

    if (m_iCurrentAnimationIndex != m_iPreviousAnimationIndex)
    {
        if (m_bAniInit == false) {
            m_bAniInit = true;
            return;
        }

        m_bInterpolationAni = true;
        m_bAniInterpolationStart = false;
    }
}

_bool CMyModel::Play_Animation(_float fTimeDelta)
{
    // 애니메이션이 끝났는지(무한 재생이면 항상 false)
    m_bIsFinished = { false };

    // Ani To Ani 보간
    if (m_bInterpolationAni == true && m_bAniInterpolationStart == false)
    {
        m_bAniInterpolationStart = true;

        m_fAccAniInterpolationTime = 0.f;
        m_PreAniFrames.clear();
        m_NextAniFrames.clear();
        m_PreAniFrames.reserve(m_Bones.size());
        m_NextAniFrames.reserve(m_Bones.size());

        // 이전 애니 전체 뼈 정보 저장
        for (auto* pBone : m_Bones)
        {
            const _float4x4* pTransformationMatrix = pBone->Get_TransformationMatrixPtr();

            XMVECTOR vScale{}, vRotation{}, vTransform{};
            XMMatrixDecompose(&vScale, &vRotation, &vTransform, XMLoadFloat4x4(pTransformationMatrix));

            KEYFRAME tFrame{};
            XMStoreFloat3(&tFrame.vScale, vScale);
            XMStoreFloat4(&tFrame.vRotation, vRotation);
            XMStoreFloat3(&tFrame.vTranslation, vTransform);

            m_PreAniFrames.push_back(tFrame);
        }

        // 다음 애니로 뼈 한 번 업데이트
        m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatZeorKeyFrame(m_Bones);

        // 다음 애니 전체 뼈 정보 저장
        for (auto* pBone : m_Bones)
        {
            const _float4x4* pTransformationMatrix = pBone->Get_TransformationMatrixPtr();

            XMVECTOR vScale{}, vRotation{}, vTransform{};
            XMMatrixDecompose(&vScale, &vRotation, &vTransform, XMLoadFloat4x4(pTransformationMatrix));

            KEYFRAME tFrame{};
            XMStoreFloat3(&tFrame.vScale, vScale);
            XMStoreFloat4(&tFrame.vRotation, vRotation);
            XMStoreFloat3(&tFrame.vTranslation, vTransform);

            m_NextAniFrames.push_back(tFrame);
        }
    }

    // 애니와 애니 사이 보간
    if (m_bInterpolationAni == true)
    {
        m_fAccAniInterpolationTime += fTimeDelta;

        _float  fRatio = m_fAccAniInterpolationTime / m_fAniInterpolationTime;
        if (fRatio >= 1.f)
        {
            fRatio = 1.f;
        }

        for (size_t i = 0; i < m_Bones.size(); ++i)
        {
            _vector vLeftScale = XMLoadFloat3(&m_PreAniFrames[i].vScale);
            _vector vRightScale = XMLoadFloat3(&m_NextAniFrames[i].vScale);

            _vector vLeftRotation = XMLoadFloat4(&m_PreAniFrames[i].vRotation);
            _vector vRightRotation = XMLoadFloat4(&m_NextAniFrames[i].vRotation);

            _vector vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_PreAniFrames[i].vTranslation), 1.f);
            _vector vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_NextAniFrames[i].vTranslation), 1.f);

            // 보간
            _vector vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);
            _vector vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);
            _vector vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);

            _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

            // 뼈(node)의 행렬(TransformationMatrix) 업데이트
            m_Bones[i]->Set_TransformationMatrix(TransformationMatrix);
        }


        if (m_fAccAniInterpolationTime >= m_fAniInterpolationTime)
        {
            m_bInterpolationAni = false;
            m_PreAniFrames.clear();
            m_NextAniFrames.clear();
            m_iPreviousAnimationIndex = m_iCurrentAnimationIndex;
        }

        for (auto& pBone : m_Bones)
        {
            pBone->Update_CombinedTransformMatrices(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        }

        return false;
    }


    // KeyFrame To KeyFrame 보간

    /* 현재 애니메이션 이용하고 있는 뼈들의 TransformationMatrix를 갱신해준다.  */
    // 현재 애니메이션으로 가서 뼈들의 행렬을 업데이트 해준다.
    m_bIsFinished = m_Animations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_Bones, fTimeDelta, m_isAnimLoop);

    /* 위의 갱신이 끝났다면, 모든 뼈의 CombinedTransformationMatrix갱신한다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformMatrices(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    return m_bIsFinished;
}

HRESULT CMyModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iIndex)
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

HRESULT CMyModel::Render(_uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMyModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    // mesh한테 자기가 필요한 뼈(node)의 인덱스를 들고 있어서 mesh에서 처리. 뼈 배열도 넘김
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

const vector<_float3>& CMyModel::Get_VtxData(_uint iMeshIdx)
{
    return m_Meshes[iMeshIdx]->Get_VtxData();
}

const vector<_uint>& CMyModel::Get_IdxData(_uint iMeshIdx)
{
    return m_Meshes[iMeshIdx]->Get_IdxData();
}

const string CMyModel::Get_MeshName(_uint iMeshIdx)
{
    return string(m_Meshes[iMeshIdx]->Get_MeshName(iMeshIdx));
}

HRESULT XM_CALLCONV CMyModel::Ready_Meshes(_fmatrix PreTransformMatrix)
{
    m_iNumMeshes = m_pMyScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; ++i)
    {
        CMyMesh* pMesh = CMyMesh::Create(m_pDevice, m_pContext, m_eType, this, &(m_pMyScene->mMeshes[i]), PreTransformMatrix, m_bStoreVtxIdx);
        if (pMesh == nullptr)
            return E_FAIL;
        
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CMyModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pMyScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; ++i)
    {
        CMyMaterial* pMaterial = CMyMaterial::Create(m_pDevice, m_pContext, &(m_pMyScene->mMaterials[i]), pModelFilePath);
        if (pMaterial == nullptr)
            return E_FAIL;
        
        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}

HRESULT CMyModel::Ready_Bones(const myNode* pMyNode, _int iParentIndex)
{
    // 최상위 부모로 CBone 객체 하나 생성
    CMyBone* pBone = CMyBone::Create(pMyNode, iParentIndex);
    if (pBone == nullptr)
        return E_FAIL;

    m_Bones.push_back(pBone);

    // 현재 내 위치가 자식한테는 부모 위치이니 size - 1
    _int iParent = static_cast<_int>(m_Bones.size()) - 1;

    // pMyNode 자식이 몇 개인지 알 수 있음
    // 재귀로 모두 순회
    // 전위 순회(부모 -> 왼쪽 -> 오른쪽)
    // 전위 순회해야 vector를 처음부터 갱신할 때 부모부터 알아서 쭉 갱신됨.
    for (_uint i = 0; i < pMyNode->mNumChildren; ++i)
    {
        Ready_Bones(&(pMyNode->mChildren[i]), iParent);
    }

    return S_OK;
}

HRESULT CMyModel::Ready_Animations()
{
    m_iNumAnimations = m_pMyScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; ++i)
    {
        CMyAnimation* pAnimation = CMyAnimation::Create(&(m_pMyScene->mAnimations[i]), this);
        if (pAnimation == nullptr)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

CMyModel* XM_CALLCONV CMyModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX)
{
    CMyModel* pInstance = new CMyModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix, bStoreVTXIDX)))
    {
        MSG_BOX("Failed to Created: CMyModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMyModel::Clone(void* pArg)
{
    CMyModel* pInstance = new CMyModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMyModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyModel::Free()
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
