#include "CCollider.h"

#include "CGameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& Prototype)
    : CComponent{ Prototype }
    , m_eType{ Prototype.m_eType }
#ifdef _DEBUG
    , m_pBatch{ Prototype.m_pBatch }
    , m_pEffect{ Prototype.m_pEffect }
    , m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{

#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
    m_eType = eType;

#ifdef _DEBUG
    // PrimitiveBatch: 정점 버퍼, 인덱스 버퍼 만들고, 토폴로지 설정 알아서 해줌
    // VertexPositionColor: 정점 구조체
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

    // 셰이더
    m_pEffect = new BasicEffect(m_pDevice);
    // 정점마다 들어 있는 color 값을 셰이더에 사용하겠다.
    // InputLayout 생성 전에 해야 한다고 함.
    m_pEffect->SetVertexColorEnabled(true);

    // inputlayout 만들 때 필요한 거 받아오고, InputLayout 생성
    const void* pShaderByteCode = { nullptr };
    size_t      iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    static const D3D11_INPUT_ELEMENT_DESC s_layout[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // InputLayout 생성
    if (FAILED(m_pDevice->CreateInputLayout(s_layout, _countof(s_layout), pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
    auto    pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

    switch (m_eType)
    {
    case COLLIDER::AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case COLLIDER::OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case COLLIDER::SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    }

    return S_OK;
}

void XM_CALLCONV CCollider::Update(_fmatrix TransformMatrix)
{
    m_pBounding->Update(TransformMatrix);
}

_bool CCollider::Intersect(CCollider* pTarget)
{
    // 내 Type은 아는데 상대 어떤 타입 콜라이던지 몰라서 타입도 같이 던져줌.
    return m_pBounding->Intersect(pTarget->m_eType, pTarget->m_pBounding);
}

void CCollider::Set_IsColl(_bool isColl)
{
    m_pBounding->Set_IsColl(isColl);
    m_isColl = isColl;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
    // GS 바인딩 해제
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    // 이미 월드가 곱해진 상태
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW)));
    m_pEffect->SetProjection(XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ)));

    // inputLayout 세팅
    m_pContext->IASetInputLayout(m_pInputLayout);

    // 셰이더 설정
    m_pEffect->Apply(m_pContext);

    // 버퍼 (동적)바인딩
    m_pBatch->Begin();

    // 버퍼에 정점 채움
    if (FAILED(m_pBounding->Render(m_pBatch)))
        return E_FAIL;

    // batch 제출
    m_pBatch->End();

    return S_OK;
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
    CCollider* pInstance = new CCollider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType)))
    {
        MSG_BOX("Failed to Created: CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    Safe_Release(m_pBounding);

#ifdef _DEBUG

    if (false == m_isCloned)
    {
        Safe_Delete(m_pEffect);
        Safe_Delete(m_pBatch);
    }

    Safe_Release(m_pInputLayout);
#endif

    __super::Free();
}