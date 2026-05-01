#include "CMyMesh.h"

#include "CMyModel.h"
#include "CShader.h"
#include "CMyBone.h"

CMyMesh::CMyMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CMyMesh::CMyMesh(const CMyMesh& Prototype)
    : CVIBuffer{ Prototype }
{
}

HRESULT XM_CALLCONV CMyMesh::Initialize_Prototype(MODEL eType, CMyModel* pModel, const myMesh* pMyMesh, MODEL_LOCAL_MIN_MAX& m_tLocalXYZ, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX)
{
    strcpy_s(m_szName, pMyMesh->mName.c_str());

    // 이 Mesh가 어떤 머테리얼을 사용하는지.(메쉬 하나 당 머테리얼 하나) 머테리얼 인덱스
    // 여러 Mesh가 하나의 머테리얼을 사용하는 것은 가능하다.
    m_iMaterialIndex = pMyMesh->mMaterialIndex;

    m_iNumVertexBuffers = 1;
    m_iNumVertices = pMyMesh->mNumVertices;

    // 삼각형 단위로 로드. 따라서 인덱스는 삼각형 * 3
    m_iNumIndices = pMyMesh->mNumIndices;
    m_iIndexStride = 4;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;

    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    HRESULT hr{};

    if (eType == MODEL::NONANIM)
    {
        hr = Ready_NonAnimMesh(pMyMesh, m_tLocalXYZ, PreTransformMatrix, bStoreVTXIDX);
    }
    else
    {
        if (bStoreVTXIDX == true)
            return E_FAIL;

        hr = Ready_AnimMesh(pModel, pMyMesh, m_tLocalXYZ);
    }

    if (FAILED(hr))
        return E_FAIL;

    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iNumIndices = pMyMesh->mNumIndices;

    if(bStoreVTXIDX == true)
    {
        vecIndices.reserve(iNumIndices);
    }

    for (size_t i = 0; i < iNumIndices; ++i)
    {
        pIndices[i] = pMyMesh->mIndices[i];

        if (bStoreVTXIDX == true)
        {
            vecIndices.push_back(pIndices[i]);
        }
    }

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
    {
        Safe_Delete_Array(pIndices);
        vecIndices.clear();
        return E_FAIL;
    }

    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CMyMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMyMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, vector<CMyBone*>& Bones)
{
    ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iNumMeshBones);

    for (size_t i = 0; i < m_iNumBones; ++i)
    {
        // 이 mesh가 필요로하는 뼈(node)들의 행렬 저장
        XMStoreFloat4x4(&m_BoneMatrices[i],
            // 정점을 bone 기준 좌표로 바꾸는 것을 뼈(node) 곱하기 전에 적용 시켜준다.
            XMLoadFloat4x4(&m_OffsetMatrices[i]) *
            // 이 mesh가 필요로하는 뼈(node)를 꺼내와서 부모 행렬이 적용된 최종 행렬을 꺼내옴.
            XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr()));

        // 반복문을 돌며
        // m_BoneIndices는 현재 aiBone의 이름으로 검색에서 aiNode의 행렬을 순서대로 저장했고,
        // m_OffsetMatrices는 현재 aiBone이 갖고 있는 OffsetMatrix를 순서대로 저장했다.
        // 따라서 둘이 곱하면 이름에 대응되는 bone의 OffsetMatrix와 node의 행렬이 곱해진 것이다.
    }

    return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

const _char* CMyMesh::Get_MeshName(_uint iMeshIdx)
{
    return m_szName;
}

HRESULT XM_CALLCONV CMyMesh::Ready_NonAnimMesh(const myMesh* pMyMesh, MODEL_LOCAL_MIN_MAX& m_tLocalXYZ, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX)
{
    // 정점 구조체는 내가 쓰고자 하는 정보로만 구성하면 된다.
    m_iVertexStride = sizeof(VTXMESH);

    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    if(bStoreVTXIDX)
    {
        vecVertices.reserve(m_iNumVertices);
    }

    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        // aiVector3D는 float 3개임. 따라서 memcpy로 float3 개만큼 복사.(자료형 달라서 memcpy)
        memcpy(&pVertices[i].vPosition, &pMyMesh->mVerticesInfo[i].mVertex, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition,
            XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        m_tLocalXYZ.vMin.x = (std::min)(m_tLocalXYZ.vMin.x, pVertices[i].vPosition.x);
        m_tLocalXYZ.vMin.y = (std::min)(m_tLocalXYZ.vMin.y, pVertices[i].vPosition.y);
        m_tLocalXYZ.vMin.z = (std::min)(m_tLocalXYZ.vMin.z, pVertices[i].vPosition.z);

        m_tLocalXYZ.vMax.x = (std::max)(m_tLocalXYZ.vMax.x, pVertices[i].vPosition.x);
        m_tLocalXYZ.vMax.y = (std::max)(m_tLocalXYZ.vMax.y, pVertices[i].vPosition.y);
        m_tLocalXYZ.vMax.z = (std::max)(m_tLocalXYZ.vMax.z, pVertices[i].vPosition.z);


        if (bStoreVTXIDX)
        {
            vecVertices.push_back(pVertices[i].vPosition);
        }

        memcpy(&pVertices[i].vNormal, &pMyMesh->mVerticesInfo[i].mNormal, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

        // texcoord 주소 배열임.(8개까지 가능)
        // 2D UV라서 float2 개만 복사
        // 한 모델의 텍스처 여러 개 쓰면 uv 여러 개 일 수 있음.
        // diffuse, normal, specular 텍스처 이런 거는 모양이 같으면 같은 UV 사용.
        memcpy(&pVertices[i].vTexcoord, &pMyMesh->mVerticesInfo[i].mTexcoord, sizeof(_float2));

        memcpy(&pVertices[i].vTangent, &pMyMesh->mVerticesInfo[i].mTangent, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

        memcpy(&pVertices[i].vBinormal, &pMyMesh->mVerticesInfo[i].mBitangent, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vBinormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
    {
        Safe_Delete_Array(pVertices);
        vecVertices.clear();
        return E_FAIL;
    }

    Safe_Delete_Array(pVertices);


    return S_OK;
}

HRESULT CMyMesh::Ready_AnimMesh(CMyModel* pModel, const myMesh* pMyMesh, MODEL_LOCAL_MIN_MAX& m_tLocalXYZ)
{
    m_iVertexStride = sizeof(VTXANIMMESH);

    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        // 사전 변환 행렬 생략
        memcpy(&pVertices[i].vPosition, &pMyMesh->mVerticesInfo[i].mVertex, sizeof(_float3));

        m_tLocalXYZ.vMin.x = (std::min)(m_tLocalXYZ.vMin.x, pVertices[i].vPosition.x);
        m_tLocalXYZ.vMin.y = (std::min)(m_tLocalXYZ.vMin.y, pVertices[i].vPosition.y);
        m_tLocalXYZ.vMin.z = (std::min)(m_tLocalXYZ.vMin.z, pVertices[i].vPosition.z);

        m_tLocalXYZ.vMax.x = (std::max)(m_tLocalXYZ.vMax.x, pVertices[i].vPosition.x);
        m_tLocalXYZ.vMax.y = (std::max)(m_tLocalXYZ.vMax.y, pVertices[i].vPosition.y);
        m_tLocalXYZ.vMax.z = (std::max)(m_tLocalXYZ.vMax.z, pVertices[i].vPosition.z);

        memcpy(&pVertices[i].vNormal, &pMyMesh->mVerticesInfo[i].mNormal, sizeof(_float3));
        memcpy(&pVertices[i].vTexcoord, &pMyMesh->mVerticesInfo[i].mTexcoord, sizeof(_float2));
        memcpy(&pVertices[i].vTangent, &pMyMesh->mVerticesInfo[i].mTangent, sizeof(_float3));
        memcpy(&pVertices[i].vBinormal, &pMyMesh->mVerticesInfo[i].mBitangent, sizeof(_float3));

        // vBlendIndex와 vBlendWeight는 여기서 못 채움.
        // loop 정점 기준
        // vBlendIndex와 vBlendWeight는 뼈 기준으로 돌려야 한다.(이 뼈는 어떤 정점들에게 영향을 줘야 하는가)
    }

    // 그리는 단위 mesh -> 셰이더에게 Model 전체 bone을 주지 않고 이 mesh에게 영향을 주는 bone만 줄 거임.
    // 이 mesh에 영향을 주는 bone의 개수
    m_iNumBones = pMyMesh->mNumBones;

    m_BoneIndices.reserve(m_iNumBones);
    m_OffsetMatrices.reserve(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        // 이 mesh의 들어있는 bone 정보
        // 실제 움직이는 건 node이고, node의 움직임이 어떤 정점에게, 어떤 가중치로
        // 영향을 주는지에 대한 정보
        const myBone* pMyBone = &(pMyMesh->mBones[i]);

        // mesh에만 사용되는 뼈(node) 찾아내기
        _int iBoneIndex = pModel->Get_BoneIndex(pMyBone->mName.c_str());
        if (iBoneIndex == -1)
            return E_FAIL;

        m_BoneIndices.push_back(iBoneIndex);

        // mOffsetMatrix는 mesh의 bone에 포함된 행렬이다.
        // 정점은 원래 model의 로컬 좌표에 있다. 하지만
        // 애니메이션을 적용하려면 bone 기준 좌표로 바꿔야 해서
        // OffsetMatrix를 곱해줘야 한다.
        // OffsetMatrix가 bone 기준 좌표로 바꿔준다.
        _float4x4 OffsetMatrix = {};

        // assimp에서 가져와서 전치해줘야 함.
        memcpy(&OffsetMatrix, &pMyBone->mOffsetMatrix, sizeof OffsetMatrix);
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        m_OffsetMatrices.push_back(OffsetMatrix);

        // bone이 영향을 주는 정점의 개수
        for (_uint j = 0; j < pMyBone->mNumWeights; j++)
        {
            // 이 mesh에서 몇 번째 정점인가, 가중치를 가져옴.
            myVertexWeight MyWeight = pMyBone->mWeights[j];

            if (0.f == pVertices[MyWeight.mVertexId].vBlendWeight.x)
            {
                // 가져온 정점에 bone의 index를 주고
                pVertices[MyWeight.mVertexId].vBlendIndex.x = i;
                // 가중치도 줌
                pVertices[MyWeight.mVertexId].vBlendWeight.x = MyWeight.mWeight;
            }
            else if (0.f == pVertices[MyWeight.mVertexId].vBlendWeight.y)
            {
                pVertices[MyWeight.mVertexId].vBlendIndex.y = i;
                pVertices[MyWeight.mVertexId].vBlendWeight.y = MyWeight.mWeight;
            }
            else if (0.f == pVertices[MyWeight.mVertexId].vBlendWeight.z)
            {
                pVertices[MyWeight.mVertexId].vBlendIndex.z = i;
                pVertices[MyWeight.mVertexId].vBlendWeight.z = MyWeight.mWeight;
            }
            else
            {
                pVertices[MyWeight.mVertexId].vBlendIndex.w = i;
                pVertices[MyWeight.mVertexId].vBlendWeight.w = MyWeight.mWeight;
            }
        }
    }

    // 뼈(애니메이션) 없는 부착물
    if (m_iNumBones == 0)
    {
        // 강제로 늘리고
        m_iNumBones = 1;

        // 이름같은 뼈 인덱스 가져옴
        _uint iBoneIndex = pModel->Get_BoneIndex(m_szName);
        if (iBoneIndex == -1)
            return E_FAIL;

        m_BoneIndices.push_back(iBoneIndex);

        // OffsetMatrix 있을리가 없음. bone이 없으니 그냥 항등 넣어줌
        _float4x4 OffsetMatrix = {};
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        m_OffsetMatrices.push_back(OffsetMatrix);

        // 이 mesh의 모든 정점한테
        for (_uint i = 0; i < m_iNumVertices; ++i)
        {
            // 뼈(bone)는 한 개임. vector 0번에 넣음.
            pVertices[i].vBlendIndex.x = 0;
            // 한 개니깐 가중치 
            pVertices[i].vBlendWeight.x = 1.f;
        }
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
    {
        Safe_Delete_Array(pVertices);
        return E_FAIL;
    }

    Safe_Delete_Array(pVertices);

    return S_OK;
}

CMyMesh* XM_CALLCONV CMyMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, CMyModel* pModel, const myMesh* pMyMesh, MODEL_LOCAL_MIN_MAX& m_tLocalXYZ, _fmatrix PreTransformMatrix, _bool bStoreVTXIDX)
{
    CMyMesh* pInstance = new CMyMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModel, pMyMesh, m_tLocalXYZ, PreTransformMatrix, bStoreVTXIDX)))
    {
        MSG_BOX("Failed to Created: CMyMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMyMesh::Clone(void* pArg)
{
    return nullptr;
}

void CMyMesh::Free()
{
    vecVertices.clear();
    vecIndices.clear();

    __super::Free();
}
