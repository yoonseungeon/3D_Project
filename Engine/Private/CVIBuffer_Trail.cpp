#include "CVIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
    : CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
#pragma region 정점 버퍼
    m_iNumVertexBuffers = 1;      //정점 버퍼의 개수

    // test
    _uint iMaxTrailPoints = 32;
    m_iNumVertices = iMaxTrailPoints * 2;
    m_iVertexStride = sizeof(VTXTEX);

    D3D11_BUFFER_DESC VertexBufferDesc{};

    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;  // 버퍼 전체 크기
    VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                   // 이 버퍼를 CPU/GPU가 어떤 방식으로 사용할지 정하는 옵션
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;          // 이 버퍼를 파이프라인에서 어떤 용도로 바인딩할 것인가
    VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;       // CPU가 이 버퍼에 어떤 접근 권한을 가질지
    VertexBufferDesc.MiscFlags = 0;                                 // 추가적인 특수 버퍼 기능
    VertexBufferDesc.StructureByteStride = 0;                       // 버퍼가 structured buffer를 나타낼 때, 버퍼 구조의 각 원소 크기

    // 1. 만들 버퍼의 설정 정보 2. 버퍼 생성 시 넣어줄 초기 데이터 3. 버퍼를 저장할 주소(리턴)
    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, nullptr, &m_pVB)))
        return E_FAIL;
#pragma endregion

#pragma region 인덱스 버퍼
    // 사각형 개수 * 2 = 삼각형 개수, 삼각형 개수 * 2 = 인덱스 개수
    m_iNumIndices = (iMaxTrailPoints - 1) * 2 * 3;
    m_iIndexStride = 2;

    m_eIndexFormat = DXGI_FORMAT_R16_UINT;

    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    D3D11_BUFFER_DESC           IndexBufferDesc{};

    IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = 0;


    D3D11_SUBRESOURCE_DATA      IndexInitialData{};

    // m_iIndexStride = 2, m_eIndexFormat = DXGI_FORMAT_R16_UINT -> 2  bytes로 잡아서 _ushort
    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    _int iIndex = -1;

    // 사각형 개수만큼
    for (_uint i = 0; i < iMaxTrailPoints - 1; ++i)
    {
        _uint iVTXIndices[4] =
        {
            i * 2,
            i * 2 + 1,
            i * 2 + 2,
            i * 2 + 3,
        };

        pIndices[++iIndex] = iVTXIndices[0];
        pIndices[++iIndex] = iVTXIndices[1];
        pIndices[++iIndex] = iVTXIndices[2];

        pIndices[++iIndex] = iVTXIndices[2];
        pIndices[++iIndex] = iVTXIndices[1];
        pIndices[++iIndex] = iVTXIndices[3];
    }

    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Trail::Bind_Resources()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
    };

    _uint		iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint		iOffsets[] = {
        0,
    };

    // 1. 정적 버퍼 시작 슬롯 2. 정점 버퍼 개수 3. 정점 버퍼들의 배열 4. 각각의 정점 버퍼들의 정점 하나의 크기 배열 5. 각각의 버퍼의 시작 위치 배열
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    // 1. 인덱스 버퍼의 주소 2. 인덱스의 크기 3. 인덱스 버퍼의 시작 위치
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    // 1. 정점 데이터를 해석하는 방식 TRIANGLELIST, TRIANGLESTRIP 등
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
    // 1. 그릴 인덱스의 개수 2. 인덱스 버퍼 시작 위치 3. 버텍스 버퍼 시작 위치
    m_pContext->DrawIndexed(m_iValidIndices, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer_Trail::Trail_Map_Discard(const VTXTEX* pNewVertices, _uint iVertexCount)
{
    if(pNewVertices == nullptr)
        return E_FAIL;

    if (iVertexCount > m_iNumVertices)
        return E_FAIL;

    // 4개 미만이면 그리기 x
    if(iVertexCount < 4)
    {
        m_iValidIndices = 0;
        return S_OK;
    }

    // 삼각형 개수 * 3
    m_iValidIndices = (iVertexCount - 2) * 3;
 
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

    memcpy(SubResource.pData, pNewVertices, sizeof(VTXTEX) * iVertexCount);

    m_pContext->Unmap(m_pVB, 0);

    return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Trail::Free()
{
    __super::Free();
}
