#include "CVIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& Prototype)
    : CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
#pragma region 정점 버퍼
    m_iNumVertexBuffers = 1;      //정점 버퍼의 개수

    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXTEX);

    D3D11_BUFFER_DESC VertexBufferDesc{};

    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;  // 버퍼 전체 크기
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;                   // 이 버퍼를 CPU/GPU가 어떤 방식으로 사용할지 정하는 옵션
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;          // 이 버퍼를 파이프라인에서 어떤 용도로 바인딩할 것인가
    VertexBufferDesc.CPUAccessFlags = 0;                            // CPU가 이 버퍼에 어떤 접근 권한을 가질지
    VertexBufferDesc.MiscFlags = 0;                                 // 추가적인 특수 버퍼 기능
    VertexBufferDesc.StructureByteStride = 0;                       // 버퍼가 structured buffer를 나타낼 때, 버퍼 구조의 각 원소 크기


    D3D11_SUBRESOURCE_DATA      VertexInitialData{};

    VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.f, 1.f);

    VertexInitialData.pSysMem = pVertices; // 초기화할 원본 데이터의 시작 주소

    // 1. 만들 버퍼의 설정 정보 2. 버퍼 생성 시 넣어줄 초기 데이터 3. 버퍼를 저장할 주소(리턴)
    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region 인덱스 버퍼
    m_iNumIndices = 6;
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

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Rect::Free()
{
    __super::Free();
}
