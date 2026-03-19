#include "CLevel_Ending.h"

CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Ending::Initialize()
{
    return S_OK;
}

void CLevel_Ending::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Ending::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Ending 레벨입니다."));
#endif

    return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Ending* pInstance = new CLevel_Ending(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Ending");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Ending::Free()
{
    __super::Free();
}
