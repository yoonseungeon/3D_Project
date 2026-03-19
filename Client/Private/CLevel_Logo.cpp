#include "CLevel_Logo.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOBBY);

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
            return;
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Logo 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
        ETOUI(LEVEL::LOGO), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Logo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();
}
