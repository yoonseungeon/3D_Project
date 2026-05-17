#include "CPartEffect.h"

#include "CGameInstance.h"
#include "CLiDailin.h"

CPartEffect::CPartEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CPartEffect::CPartEffect(const CPartEffect& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CPartEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartEffect::Initialize(void* pArg)
{
    PARTEFFECT_DESC* pDesc = static_cast<PARTEFFECT_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;
    m_iTexIdx = pDesc->iTexIdx;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CPartEffect::Priority_Update(_float fTimeDelta)
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

void CPartEffect::Parallel_Update(_float fTimeDelta)
{
}

void CPartEffect::Update(_float fTimeDelta)
{
}

void CPartEffect::Late_Update(_float fTimeDelta)
{

}

HRESULT CPartEffect::Render()
{
    return S_OK;
}

void CPartEffect::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
}

void CPartEffect::Set_EffectTransform(_uint iCurAni, _uint iATKCount)
{
}

void CPartEffect::Free()
{
    __super::Free();
}