#include "CLevel_Lobby.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"

#include "CStage_Lobby.h"
#include "CStage_Select.h"

CLevel_Lobby::CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Lobby::Initialize()
{
    m_pGameInstance->Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE::PARALLEL);

    if(FAILED(Ready_Lobby()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Lobby::Update(_float fTimeDelta)
{
    Update_Stage();

    if (m_pGameInstance->Key_Down(DIK_RETURN))
    {
        CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY);

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
            return;
    }
}

HRESULT CLevel_Lobby::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Lobby 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Lobby::Change_Stage(STAGE eNewStage)
{
    if (eNewStage >= STAGE::STAGE_END) {
        return E_FAIL;
    }

    m_eCurStage = eNewStage;

    return S_OK;
}

void CLevel_Lobby::Update_Stage()
{
    if (m_eCurStage != m_ePreStage) {

        const _uint iCurStage = ETOUI(m_eCurStage);
        const _uint iPreStage = ETOUI(m_ePreStage);

        m_Stages[iPreStage]->Disable_Stage();

        if (m_Stages[iCurStage] == nullptr) {

            switch (m_eCurStage) {
            case STAGE::LOBBY: {
                m_Stages[iCurStage] = CStage_Lobby::Create(m_pDevice, m_pContext,
                    [this](STAGE eStage)->void {
                    if (FAILED(Change_Stage(eStage)))
                    {
                        MSG_BOX("Failed to Changed: Stage");
                    }
                });
                break;
            }
            case STAGE::SELECT: {
                m_Stages[iCurStage] = CStage_Select::Create(m_pDevice, m_pContext,
                    [this](STAGE eStage)->void {
                    if (FAILED(Change_Stage(eStage)))
                    {
                        MSG_BOX("Failed to Changed: Stage");
                    }
                });



                break;
            }
            }
        }
        if(m_Stages[iCurStage] != nullptr)
        {
            m_Stages[iCurStage]->Enable_Stage();
        }

        m_ePreStage = m_eCurStage;
    }
}

HRESULT CLevel_Lobby::Ready_Lobby()
{
    m_Stages[ETOUI(STAGE::LOBBY)] = CStage_Lobby::Create(m_pDevice, m_pContext,
        [this](STAGE eStage)->void {
        if (FAILED(Change_Stage(eStage)))
        {
            MSG_BOX("Failed to Changed: Stage");
        }
    });
    m_eCurStage = STAGE::LOBBY;
    m_ePreStage = STAGE::LOBBY;

    return S_OK;
}

CLevel_Lobby* CLevel_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Lobby* pInstance = new CLevel_Lobby(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Lobby");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Lobby::Free()
{
    for (auto pStage : m_Stages) {
        Safe_Release(pStage);
    }

    __super::Free();
}
