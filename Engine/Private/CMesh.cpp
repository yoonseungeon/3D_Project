#include "CMesh.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
    : CVIBuffer{ Prototype }
{
}

HRESULT XM_CALLCONV CMesh::Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    // 이 Mesh가 어떤 머테리얼을 사용하는지.(메쉬 하나 당 머테리얼 하나) 머테리얼 인덱스
    // 여러 Mesh가 하나의 머테리얼을 사용하는 것은 가능하다.
    m_iMaterialIndex = pAIMesh->mMaterialIndex;

    m_iNumVertexBuffers = 1;
    m_iNumVertices = pAIMesh->mNumVertices;
    // 정점 구조체는 내가 쓰고자 하는 정보로만 구성하면 된다.
    m_iVertexStride = sizeof(VTXMESH);

    // 삼각형 단위로 로드. 따라서 인덱스는 삼각형 * 3
    m_iNumIndices = pAIMesh->mNumFaces * 3;
    m_iIndexStride = 4;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;

    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        // aiVector3D는 float 3개임. 따라서 memcpy로 float3 개만큼 복사.(자료형 달라서 memcpy)
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition,
            XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

        // texcoord 주소 배열임.(8개까지 가능)
        // 2D UV라서 float2 개만 복사
        // 한 모델의 텍스처 여러 개 쓰면 uv 여러 개 일 수 있음.
        // diffuse, normal, specular 텍스처 이런 거는 모양이 같으면 같은 UV 사용.
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

        memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vBinormal,
            XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));
    }

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);


    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint       iNumIndices = {};

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {
        // 면을 구성하는 인덱스
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

CMesh* XM_CALLCONV CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pAIMesh, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created: CMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    return nullptr;
}

void CMesh::Free()
{
    __super::Free();
}
