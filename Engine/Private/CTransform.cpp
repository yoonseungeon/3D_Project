#include "CTransform.h"
#include "CShader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& Prototype)
    : CComponent{ Prototype }
    , m_WorldMatrix{ Prototype.m_WorldMatrix }
{
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    XMStoreFloat4(&m_RotQuat, XMQuaternionIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return S_OK;

    TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

    m_fRotationPerSec = pDesc->fRotationPerSec;
    m_fSpeedPerSec = pDesc->fSpeedPerSec;
   
    Set_State(STATE::POSITION,
        XMVectorSetW(XMLoadFloat3(&(pDesc->vStartPos)), 1.f)
    );

    return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

_vector CTransform::Get_State(STATE eState)
{
    return XMLoadFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOUI(eState)]));
}

void XM_CALLCONV CTransform::Set_State(STATE eState, _fvector vState)
{
    XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOUI(eState)]), vState);
}

_float3 CTransform::Get_Scaled()
{
    return _float3(
        XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
        XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
        XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
    );
}

void CTransform::Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
    Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
    Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
    Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
    Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);
}

void XM_CALLCONV CTransform::Rotation(_fvector vAxis, _float fRadian)
{
    _vector vNewRot = XMQuaternionRotationAxis(vAxis, fRadian);

    vNewRot = XMQuaternionNormalize(vNewRot);

    XMStoreFloat4(&m_RotQuat, vNewRot);

    Reset_Rotation();
}

void XM_CALLCONV CTransform::Turn(_fvector vAxis, _float fTimeDelta, _float fRotSpeed)
{
    _vector vRotQuat = XMLoadFloat4(&m_RotQuat);
    // 회전축을 기준으로 회전량을 얻어옴.
    _vector vDq = XMQuaternionRotationAxis(vAxis, fRotSpeed * fTimeDelta);

    // vRotQuat 회전후 vDq 회전
    vRotQuat = XMQuaternionMultiply(vRotQuat, vDq);
    vRotQuat = XMQuaternionNormalize(vRotQuat);

    XMStoreFloat4(&m_RotQuat, vRotQuat);

    Reset_Rotation();
}

void XM_CALLCONV CTransform::TurnDirDefaultY(_fvector vDir, _float fTimeDelta, _float fRotSpeed)
{
    _vector vLook = XMVector3Normalize(Get_State(STATE::LOOK));
    _vector vNormalizedDir = XMVector3Normalize(vDir);

    // 안하면 XMQuaternionRotationAxis 여기서 assert
    if (XMVector3Equal(vNormalizedDir, XMVectorZero()))
    {
        return;
    }

    // 남은 각도
    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormalizedDir));   
    MyHelper::FloatClamp(fDot, -1.f, 1.f);  // 이거 안하면 nan뜸
    _float fSeta = acosf(fDot);
    
    if (fSeta <= MyHelper::fEpsilon)
    {
        return;
    }
    
    // 축
    _vector vAxis{};
    // 180인 경우 회전할 수 있는 축이 무한대이기 때문에 Y로 고정
    if (fDot <= -1.f + MyHelper::fEpsilon)
    {
        vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    }
    else
    {
        vAxis = XMVector3Normalize(XMVector3Cross(vLook, vNormalizedDir));
    }

    // 오버 슈팅 방지
    _float fDRadian = (std::min)(fSeta, fTimeDelta * fRotSpeed);

    // Turn
    _vector vRotQuat = XMLoadFloat4(&m_RotQuat);
    // 회전축을 기준으로 회전량을 얻어옴.
    _vector vDq = XMQuaternionRotationAxis(vAxis, fDRadian);

    // vRotQuat 회전후 vDq 회전
    vRotQuat = XMQuaternionMultiply(vRotQuat, vDq);
    vRotQuat = XMQuaternionNormalize(vRotQuat);

    XMStoreFloat4(&m_RotQuat, vRotQuat);

    Reset_Rotation();
}

void XM_CALLCONV CTransform::LookAt(_fvector vAt)
{
    _vector vLook = XMVectorSetW(XMVector3Normalize(vAt - Get_State(STATE::POSITION)), 0.f);
    _vector vRight = XMVectorSetW(XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)), 0.f);
    _vector vUp = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vLook, vRight)), 0.f);

    // 회전 행렬 생성
    _matrix matRot = { vRight , vUp , vLook , XMVectorSet(0.f, 0.f, 0.f, 1.f) };

    // 사원수 얻어오기
    _vector vRotQuat = XMQuaternionRotationMatrix(matRot);
    vRotQuat = XMQuaternionNormalize(vRotQuat);

    XMStoreFloat4(&m_RotQuat, vRotQuat);

    Reset_Rotation();
}

void XM_CALLCONV CTransform::Set_Pos(_fvector vPos)
{
    Set_State(STATE::POSITION, vPos);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
    _vector vPosition = Get_State(STATE::POSITION);
    _vector vLook = Get_State(STATE::LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
    _vector vPosition = Get_State(STATE::POSITION);
    _vector vLook = Get_State(STATE::LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
    _vector vPosition = Get_State(STATE::POSITION);
    _vector vRight = Get_State(STATE::RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
    _vector vPosition = Get_State(STATE::POSITION);
    _vector vRight = Get_State(STATE::RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void XM_CALLCONV CTransform::Reset_Rotation()
{
    _matrix matWorld = XMLoadFloat4x4(&m_WorldMatrix);

    _vector vS{}, vR{}, vT{};
    XMMatrixDecompose(&vS, &vR, &vT, matWorld);

    _vector vNewRot = XMLoadFloat4(&m_RotQuat);

    matWorld = XMMatrixAffineTransformation(vS, XMVectorSet(0.f, 0.f, 0.f, 1.f), vNewRot, vT);

    XMStoreFloat4x4(&m_WorldMatrix, matWorld);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTransform* pInstance = new CTransform(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CTransform");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
    MSG_BOX("Failed to Cloned: CTransform");

    return nullptr;
}

void CTransform::Free()
{
    __super::Free();
}
