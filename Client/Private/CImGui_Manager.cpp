#ifdef _DEBUG

#include "CImGui_Manager.h"

#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameObject.h"

#include <typeinfo>

CGameObject* pGameObject = { nullptr };
string strGameObjectName;

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
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
    static _bool show_component_prototype_window = FALSE;
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
        ImGui::Checkbox("Component Proto", &show_component_prototype_window);
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

    if (show_component_prototype_window)
    {
        ImGui::Begin("Component Window", &show_component_prototype_window);
        ImGui::Text("Component Area");
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
        const _float fFastSpeed = 1.f;

        ImGui::Text("Input");
        ImGui::Text("X ");
        ImGui::InputFloat("##XX", &vScale.x, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Y ");
        ImGui::InputFloat("##YY", &vScale.y, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Z ");
        ImGui::InputFloat("##ZZ", &vScale.z, fSpeed, fFastSpeed, "%.2f");

        pTransform->Set_Scale(vScale.x, vScale.y, vScale.z);
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
        const _float fFastSpeed = 1.f;

        ImGui::Text("Input");
        ImGui::Text("X ");
        ImGui::InputFloat("##X", &vFloat4.x, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Y ");
        ImGui::InputFloat("##Y", &vFloat4.y, fSpeed, fFastSpeed, "%.2f");

        ImGui::Text("Z ");
        ImGui::InputFloat("##Z", &vFloat4.z, fSpeed, fFastSpeed, "%.2f");

        pTransform->Set_State(STATE::POSITION, XMLoadFloat4(&vFloat4));
    }
}

void CImGui_Manager::Free()
{
    Safe_Release(pGameObject);

    Safe_Release(m_pGameInstance);

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}

#endif