#include "CInGame_Manager.h"

#include "CLumia_NavMesh.h"
#include "CAbstractPlayer.h"
#include "CUI_GameResult.h"

#include "CSplitGround.h"
#include "CUI_Timer.h"

IMPLEMENT_SINGLETON(CInGame_Manager)

CInGame_Manager::CInGame_Manager()
{
    m_fMaxWaitGameEndTime = 3.f;

    m_fAccDayTimer = 5.f;

    m_fVisionRange = 9.5f;
}

void CInGame_Manager::Update_InGameManager(_float fTimeDelta)
{
    // SplitGround Resets
    for (auto pSplitGround : m_SplitGrounds)
        Safe_Release(pSplitGround);
    m_SplitGrounds.clear();

    // Timer
    m_fAccDayTimer -= fTimeDelta;
    if (m_fAccDayTimer <= 0.f)
    {
        m_fAccDayTimer = 5.f;
        ++m_iDay;

        if (m_iDay % 2 == 1)
            m_fVisionRange = 9.5f;
        else
            m_fVisionRange = 7.5f;
    }

    if(m_pTimer != nullptr)
        m_pTimer->Set_Timer(m_iDay, m_fAccDayTimer);


    // 종료 체크
    if (m_iEnemyCount != 0)
        return;

    if (m_pGameResultUI == nullptr)
        return;

    m_fAccWaitGameEndTime += fTimeDelta;

    if (m_fAccWaitGameEndTime >= m_fMaxWaitGameEndTime)
    {
        m_pGameResultUI->GameResultStart();

        if (m_pGameResultUI->Get_ResultEnd() == true)
        {
            if (m_pGameResultUI->Get_ResultEnd() == true)
            {
                m_bGameEnd = true;
                m_pGameResultUI->GameResultReset();
            }
        }
    }

}

void CInGame_Manager::Set_Player(CAbstractPlayer* pPlayer)
{
    if (m_pPlayer == nullptr) {
        m_pPlayer = pPlayer;
        Safe_AddRef(m_pPlayer);
    }
}

void CInGame_Manager::Release_Player()
{
    Safe_Release(m_pPlayer);
    m_pPlayer = nullptr;
}

_float3 CInGame_Manager::Get_PlayerPos()
{
    _float3 vPos{};

    if (m_pPlayer == nullptr)
    {
        return vPos;
    }

    const _float4x4* pWorldMatrix = m_pPlayer->Get_TransformCom()->Get_WorldMatrixPtr();

    memcpy(&vPos, (*pWorldMatrix).m[3], sizeof(_float3));

    return vPos;
}

void CInGame_Manager::Set_GameResultUI(CUI_GameResult* pGameResult)
{
    if (m_pGameResultUI == nullptr) {
        m_pGameResultUI = pGameResult;
        Safe_AddRef(m_pGameResultUI);
    }
}

void CInGame_Manager::Release_GameResultUI()
{
    Safe_Release(m_pGameResultUI);
    m_pGameResultUI = nullptr;
}

_bool CInGame_Manager::Picking_SplitGround(_float3& vOutPos)
{
    vOutPos = { 0.f,0.f,0.f };

    if (m_SplitGrounds.empty() == true)
        return false;

    for (auto pSplitGround : m_SplitGrounds)
    {
        _float3 vPos{};

        // 지형이 겹치지 않는다고 가정
        if (pSplitGround->Picking(vPos) == true)
        {
            vOutPos = vPos;
            return true;
        }
    }

    return false;
}

void CInGame_Manager::Add_SplitGround(CSplitGround* pSplitGround)
{
    m_SplitGrounds.push_back(pSplitGround);
    Safe_AddRef(pSplitGround);
}

void CInGame_Manager::Set_DayTimer(CUI_Timer* pTimer)
{
    if (m_pTimer == nullptr) {
        m_pTimer = pTimer;
        Safe_AddRef(m_pTimer);
    }
}

void CInGame_Manager::Release_DayTimer()
{
    Safe_Release(m_pTimer);
    m_pTimer = nullptr;
}

_bool XM_CALLCONV CInGame_Manager::IsInVisionRange(_fvector vPos)
{
    if (m_pPlayer == nullptr)
        return true;

    _vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(STATE::POSITION);

    _float fLengthSq = XMVectorGetX(XMVector3LengthSq(XMVectorSetY(vPlayerPos, 0.f) - XMVectorSetY(vPos, 0.f)));

    if (fLengthSq > m_fVisionRange * m_fVisionRange)
        return false;

    return true;
}

void CInGame_Manager::Free()
{
    Safe_Release(m_pGameResultUI);
    Safe_Release(m_pPlayer);

    __super::Free();
}