#include "CEffect.h"

#include "CGameInstance.h"

#include "CLiDailin.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CEffect::CEffect(const CEffect& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

    m_iTexIdx = pDesc->iTexIdx;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
    #ifdef _DEBUG
        // 위치 조정
        {
            bool bChanged = false;
    
            float fMoveSpeed = 1.f;
    
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                fMoveSpeed = 0.1f;
    
            if (GetAsyncKeyState(VK_MENU) & 0x8000)
                fMoveSpeed = 5.f;
    
            float fMove = fMoveSpeed * fTimeDelta;
    
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
                fMove *= -1.f;
    
            if (GetAsyncKeyState('B') & 0x8000)
            {
                m_vDebugPos.x += fMove;
                bChanged = true;
            }
    
            if (GetAsyncKeyState('N') & 0x8000)
            {
                m_vDebugPos.y += fMove;
                bChanged = true;
            }
    
            if (GetAsyncKeyState('M') & 0x8000)
            {
                m_vDebugPos.z += fMove;
                bChanged = true;
            }
    
            if (bChanged == true)
            {
                m_pTransformCom->Set_State(
                    STATE::POSITION,
                    XMLoadFloat4(&m_vDebugPos)
                );
    
                wchar_t szDebug[256]{};
                swprintf_s(
                    szDebug,
                    L"Pos = { %.6ff, %.6ff, %.6ff, 1.f }\n",
                    m_vDebugPos.x,
                    m_vDebugPos.y,
                    m_vDebugPos.z
                );
    
                OutputDebugStringW(szDebug);
            }
        }
    
        // 회전 조정
        {
            bool bChanged = false;
    
            float fSpeedDegree = 60.f;
    
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                fSpeedDegree = 10.f;
    
            float fAngle = XMConvertToRadians(fSpeedDegree) * fTimeDelta;
    
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
                fAngle *= -1.f;
    
            XMVECTOR qCur = XMLoadFloat4(&m_vDebugQuat);
            qCur = XMQuaternionNormalize(qCur);
    
            auto Apply_WorldRotation = [&](FXMVECTOR vAxis)
                {
                    XMVECTOR qDelta = XMQuaternionRotationAxis(vAxis, fAngle);
    
                    // 월드축 기준
                    qCur = XMQuaternionMultiply(qCur, qDelta);
                    qCur = XMQuaternionNormalize(qCur);
    
                    bChanged = true;
                };
    
            if (GetAsyncKeyState('J') & 0x8000)
            {
                Apply_WorldRotation(XMVectorSet(1.f, 0.f, 0.f, 0.f));
            }
    
            if (GetAsyncKeyState('K') & 0x8000)
            {
                Apply_WorldRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f));
            }
    
            if (GetAsyncKeyState('L') & 0x8000)
            {
                Apply_WorldRotation(XMVectorSet(0.f, 0.f, 1.f, 0.f));
            }
    
            if (bChanged == true)
            {
                XMStoreFloat4(&m_vDebugQuat, qCur);
    
                m_pTransformCom->Set_Rotation(m_vDebugQuat);
    
                wchar_t szDebug[256]{};
                swprintf_s(
                    szDebug,
                    L"Quat = { %.6ff, %.6ff, %.6ff, %.6ff }\n",
                    m_vDebugQuat.x,
                    m_vDebugQuat.y,
                    m_vDebugQuat.z,
                    m_vDebugQuat.w
                );
    
                OutputDebugStringW(szDebug);
            }
        }
    #endif
}
void CEffect::Parallel_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{

}

void CEffect::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CEffect::Render()
{
    return S_OK;
}

void CEffect::Compute_CombinedMatrix(const _float4x4* vParentMatrix)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())
        * XMLoadFloat4x4(vParentMatrix));
}

void CEffect::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
}

void CEffect::Set_EffectTransform(_uint iCurAni, _uint iATKCount)
{
    if (m_iCurAni == iCurAni && m_iATKCount == iATKCount)
        return;

    m_iCurAni = iCurAni;
    m_iATKCount = iATKCount;

    switch (m_iCurAni) {
    case ETOUI(Nunchaku_Ani::ATK_1_WP):
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.102898f, 1.047245f, 0.430798f, 1.f));
        _float4 vQuat = _float4(0.370416f, 0.631760f, 0.383929f, -0.562379f);
        m_pTransformCom->Set_Rotation(vQuat);
        break;
    }
    case ETOUI(Nunchaku_Ani::ATK_2_WP):
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.113678f, 1.445772f, 0.451558f, 1.f));
        _float4 vQuat = _float4(0.483466f, 0.536214f, 0.441775f, 0.532514f);
        m_pTransformCom->Set_Rotation(vQuat);
        break;
    }
    case ETOUI(Nunchaku_Ani::ATK_1P_WP):
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.102898f, 1.047245f, 0.430798f, 1.f));
        _float4 vQuat = _float4(0.370416f, 0.631760f, 0.383929f, -0.562379f);
        m_pTransformCom->Set_Rotation(vQuat);
        break;
    }
    case ETOUI(Nunchaku_Ani::ATK_2P_WP):
    {
        if (iATKCount == 1)
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.113678f, 1.445772f, 0.451558f, 1.f));
            _float4 vQuat = _float4(0.483466f, 0.536214f, 0.441775f, 0.532514f);
            m_pTransformCom->Set_Rotation(vQuat);
        }
        else
        {
            //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-0.806056f, 0.088632f, 0.680687f, 1.f));
            //_float4 vQuat = _float4(0.344840f, -0.588919f, 0.478363f, 0.552656f);
            //m_pTransformCom->Set_Rotation(vQuat);
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.102898f, 1.047245f, 0.430798f, 1.f));
            _float4 vQuat = _float4(0.370416f, 0.631760f, 0.383929f, -0.562379f);
            m_pTransformCom->Set_Rotation(vQuat);
        }
        break;
    }
    default:
    {
        return;
    }
    }

}

void CEffect::Free()
{
    __super::Free();
}