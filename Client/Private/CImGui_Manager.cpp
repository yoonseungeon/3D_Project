#ifdef _DEBUG

#include "CImGui_Manager.h"

#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CInGame_Manager.h"

#include <typeinfo>

CGameObject* pGameObject = { nullptr };
string strGameObjectName;
_float3 vClickPos{};

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pInGame_Manager{ CInGame_Manager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pInGame_Manager);
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (pDevice == nullptr || pContext == nullptr) {
        return E_FAIL;
    }

    IMGUI_CHECKVERSION();

    if (!ImGui::CreateContext()) {
        return E_FAIL;
    }

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsClassic();

    if (!ImGui_ImplWin32_Init(g_hWnd)) {
        ImGui::DestroyContext();
        return E_FAIL;
    }

    if (!ImGui_ImplDX11_Init(pDevice, pContext)) {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        return E_FAIL;
    }

    m_pDevice = pDevice;
    pDevice->AddRef();

    m_pContext = pContext;
    pContext->AddRef();

	return S_OK;
}

void CImGui_Manager::Update(_float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(
        0,                                     
        ImGui::GetMainViewport(),              
        ImGuiDockNodeFlags_PassthruCentralNode 
    );

    static _bool show_begin_window = TRUE;


    static _bool show_demo_window = FALSE;
    static _bool show_gameobjects = TRUE;
    static _bool show_components = TRUE;
    static _bool show_prototype = TRUE;
    static _bool show_gameObject_setting_window = FALSE;


    static _bool bIsF6Pressed = FALSE;
    static _bool bAll_UI_Off = FALSE;


    // F6 키 입력 처리 (한 번만 눌리게 처리)
    if (GetAsyncKeyState(VK_F6) & 0x8000)
    {
        if (!bIsF6Pressed)
        {
            bIsF6Pressed = TRUE;
            bAll_UI_Off = !bAll_UI_Off;

            _bool bNextState = !bAll_UI_Off;
            show_begin_window = bNextState;
        }
    }
    else
    {
        bIsF6Pressed = FALSE;
    }

    if (show_begin_window)
    {
        ImGui::Begin("Basic Window", &show_begin_window);
        ImGui::Text("F6 : Toggle UI On/Off");
        ImGui::Separator();

        if (ImGui::Button("On/Off F6"))
        {
            bAll_UI_Off = !bAll_UI_Off;
            show_begin_window = !bAll_UI_Off;
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
       
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Checkbox("GameObjects", &show_gameobjects);
        ImGui::Checkbox("Components", &show_components);
        ImGui::Checkbox("Prototype", &show_prototype);
        ImGui::Checkbox("Object Setting", &show_gameObject_setting_window);

        ImGui::End();
    }

    if (bAll_UI_Off)
        return;

    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    if (show_gameobjects)
    {
        ImGui::Begin("GameObjects", &show_gameobjects);
        Show_GameObjects();
        ImGui::End();
    }

    if (show_components)
    {
        ImGui::Begin("Components", &show_components);
        Show_Transform();
        ImGui::End();
    }

    if (show_prototype)
    {
        ImGui::Begin("Component Window", &show_prototype);

        if (m_pGameInstance->Mouse_Down(DIMB::LBUTTON)) {
            vClickPos = m_pInGame_Manager->MapPIcking();
        }
        ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f", vClickPos.x, vClickPos.y, vClickPos.z);


        ImGui::End();
    }

    if (show_gameObject_setting_window)
    {
        ImGui::Begin("Setting Window", &show_gameObject_setting_window);
        ImGui::Text("Setting Area");
        ImGui::End();
    }
}

HRESULT CImGui_Manager::Render()
{
    ImGui::EndFrame();

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

bool CImGui_Manager::IsInputCapturedByUI()
{
    // 마우스 입력(클릭, 이동, 휠 등)이 UI에 의해 캡처되었는지 확인
    return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

void CImGui_Manager::Show_GameObjects()
{
    const size_t NumLevels = m_pGameInstance->Get_NumLevels();

    auto mapLevelLayers = m_pGameInstance->Get_MapLevelLayers();

    _uint iIndex = { 0 };

    for (size_t i = 0; i < NumLevels; ++i)
    {
        for (auto pair : mapLevelLayers[i]) {

            ImGui::Text(WStringToUTF8(pair.first).c_str());
            ImGui::Separator();

            auto& ObjList = pair.second->Get_ObjList();

            for (auto pObj : ObjList) {
                
                string Text = typeid(*pObj).name();

                if (Text.size() >= 14) {
                    Text.erase(0, 14);
                }

                Text += " " + to_string(iIndex);

                if (ImGui::Button(Text.c_str()))
                {
                    if(pGameObject == nullptr)
                    {
                        pGameObject = pObj;
                        strGameObjectName = Text;

                        Safe_AddRef(pGameObject);
                    }
                    else if (pGameObject != pObj) {
                        Safe_Release(pGameObject);
                        pGameObject = pObj;
                        strGameObjectName = Text;
                        Safe_AddRef(pGameObject);
                    }
                }

                ++iIndex;
            }

            ImGui::Separator();
        }
    }
}

void CImGui_Manager::Show_Transform()
{
    if (pGameObject == nullptr)
        return;

    auto& map = pGameObject->Get_Componets();
    auto iter = map.find(g_strTransformTag);

    if (iter == map.end()) {
        return;
    }

    CTransform* pTransform = dynamic_cast<CTransform*>(iter->second);

    if (pTransform == nullptr)
        return;

    ImGui::Text(strGameObjectName.c_str());
    ImGui::Separator();
    ImGui::Separator();

    ImGui::Text("Com_Transform");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen))
    {
        _float3 vScale = pTransform->Get_Scaled();

        ImGui::Text("X: %.2f", vScale.x);
        ImGui::SameLine(80.f, 0.f);
        ImGui::Text("Y: %.2f", vScale.y);
        ImGui::SameLine(160.f, 0.f);
        ImGui::Text("Z: %.2f", vScale.z);
        ImGui::Separator();

        const _float fSpeed = 0.1f;
        const _float fFastSpeed = 10.f;

        ImGui::Text("Input");
        ImGui::Text("X ");
        ImGui::InputFloat("##XX", &vScale.x, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Y ");
        ImGui::InputFloat("##YY", &vScale.y, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Z ");
        ImGui::InputFloat("##ZZ", &vScale.z, fSpeed, fFastSpeed, "%.2f");

        pTransform->Set_Scale(vScale.x, vScale.y, vScale.z);


        /////////////////////////////////////////////////////////
        ImGui::Separator();
        const _float fPSizeRatioX = (vScale.x / g_iWinSizeX) / 0.32096f;
        const _float fPSizeRatioY = (vScale.y / g_iWinSizeY) / 0.56828f;
        
        ImGui::Text("Parent Size X %f", fPSizeRatioX);
        ImGui::Text("Parent Size Y %f", fPSizeRatioY);
    }    

    if (ImGui::CollapsingHeader("Pos", ImGuiTreeNodeFlags_DefaultOpen))
    {
        _vector vectorPos = pTransform->Get_State(STATE::POSITION);

        XMFLOAT4 vFloat4{};
        XMStoreFloat4(&vFloat4, vectorPos);

        ImGui::Text("X: %.2f", vFloat4.x);
        ImGui::SameLine(80.f, 0.f);
        ImGui::Text("Y: %.2f", vFloat4.y);
        ImGui::SameLine(160.f, 0.f);
        ImGui::Text("Z: %.2f", vFloat4.z);
        ImGui::Separator();

        const _float fSpeed = 0.1f;
        const _float fFastSpeed = 5.f;

        ImGui::Text("Input");
        ImGui::Text("X ");
        ImGui::InputFloat("##X", &vFloat4.x, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Y ");
        ImGui::InputFloat("##Y", &vFloat4.y, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Z ");
        ImGui::InputFloat("##Z", &vFloat4.z, fSpeed, fFastSpeed, "%.2f");

        pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&vFloat4));


        /////////////////////////////////////////////////////////
        ImGui::Separator();
        const _float fParentScaleRatioX = 0.32096f;
        const _float fParentScaleRatioY = 0.56828f;

        const _float ChildPosRatioX = (vFloat4.x / g_iWinSizeX);
        const _float ChildPosRatioY = (vFloat4.y / g_iWinSizeY);

        const _float ParentPosRatioX = -0.23f;
        const _float ParentPosRatioY = 0.1f;

        ImGui::Text("Parent Pos X %f", (ChildPosRatioX - ParentPosRatioX) / fParentScaleRatioX);
        ImGui::Text("Parent Pos Y %f", (ChildPosRatioY - ParentPosRatioY) / fParentScaleRatioY);
    }
}

std::string CImGui_Manager::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty())
        return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    std::string strTo(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

std::wstring CImGui_Manager::UTF8ToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    std::wstring wstrTo(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}

void CImGui_Manager::Free()
{
    Safe_Release(pGameObject);

    Safe_Release(m_pInGame_Manager);
    Safe_Release(m_pGameInstance);

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}

#endif