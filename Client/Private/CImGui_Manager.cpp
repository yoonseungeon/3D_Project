#ifdef _DEBUG

#include "CImGui_Manager.h"

#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CInGame_Manager.h"

#include <typeinfo>

#include "CForkLift.h"

CGameObject* pGameObject = { nullptr };
string strGameObjectName;
_float3 vClickPos{};
wstring wstrLayer = L"Layer_ImGui";

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

    static _bool show_begin_window = FALSE;
    static _bool bIsF6Pressed = TRUE;
    static _bool bAll_UI_Off = TRUE;

    static _bool show_demo_window = FALSE;
    static _bool show_gameobjects = TRUE;
    static _bool show_components = TRUE;
    static _bool show_prototype = TRUE;
    static _bool show_gameObject_setting_window = FALSE;





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
        ImGui::Begin("Prototype", &show_prototype);

        Map_Picking();
        Show_Object_Prototype();

        ImGui::End();
    }

    if (show_gameObject_setting_window)
    {
        ImGui::Begin("Setting Window", &show_gameObject_setting_window);
        ImGui::Text("Setting Area");
        ImGui::End();
    }

    if (m_pGameInstance->Key_Down(DIK_S) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
    {
        GamePlayLevel_Save();
    }

    if (m_pGameInstance->Key_Down(DIK_L) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
    {
        GamePlayLevel_Load();
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

        ImGui::Separator();
        const _float fSizeRatioX = (vScale.x / g_iWinSizeX);
        const _float fSizeRatioY = (vScale.y / g_iWinSizeY);

        ImGui::Text("fSizeRatio X %f", fSizeRatioX);
        ImGui::Text("fSizeRatio Y %f", fSizeRatioY);
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


        ImGui::Separator();
        const _float fPosRatioX = (vFloat4.x / g_iWinSizeX);
        const _float fPosRatioY = (vFloat4.y / g_iWinSizeY);

        ImGui::Text("fPosRatio X %f", fPosRatioX);
        ImGui::Text("fPosRatio Y %f", fPosRatioY);
    }
}

void CImGui_Manager::Map_Picking()
{
    if (m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
    {
        m_pInGame_Manager->Picking_SplitGround(vClickPos);
    }
    ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f", vClickPos.x, vClickPos.y, vClickPos.z);
}

void CImGui_Manager::Show_Object_Prototype()
{
    ImGui::Separator();

    m_iCurPrototypeCnt =  m_pGameInstance->Get_PrototypeCnt(static_cast<_uint>(LEVEL::GAMEPLAY));

    if (m_iCurPrototypeCnt != m_iPrePrototypeCnt) {
        m_PrototypeTags.clear();
        m_pGameInstance->Get_PrototypeTags(static_cast<_uint>(LEVEL::GAMEPLAY), m_PrototypeTags);

        for (size_t i = 0; i < m_PrototypeTags.size();)
        {
            size_t Idx = m_PrototypeTags[i].find(L"Component");
            if (Idx != wstring::npos) {
                m_PrototypeTags.erase(m_PrototypeTags.begin() + i);
            }
            else {
                ++i;
            }
        }

        std::sort(m_PrototypeTags.begin(), m_PrototypeTags.end());
    }

    wstring wstrObjName;
    for (auto wstr : m_PrototypeTags) {

        wstrObjName = wstr;
        size_t Idx = wstrObjName.rfind(L"Prototype_GameObject_");
        if (Idx != wstring::npos) {
           wstrObjName.erase(wstrObjName.begin(), wstrObjName.begin() + 21);
        }

        if (ImGui::Button(WStringToUTF8(wstrObjName).c_str()))
        {
            CGameObject::GAMEOBJECT_DESC Desc{};

            Desc.tTransformDesc.vStartPos = vClickPos;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), wstr,
                ETOUI(LEVEL::GAMEPLAY), wstrLayer, &Desc)))
            {
                MSG_BOX("Failed to Created: Object");
            }
        }
    }
}

string CImGui_Manager::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty())
        return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    string strTo(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

wstring CImGui_Manager::UTF8ToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

    wstring wstrTo(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}

void CImGui_Manager::GamePlayLevel_Save()
{
    const size_t NumLevels = m_pGameInstance->Get_NumLevels();

    const _uint iGamePlayLevel = static_cast<_uint>(LEVEL::GAMEPLAY);

    if (iGamePlayLevel >= NumLevels)
    {
        MSG_BOX("Save Failed");
        return;
    }

    auto pMapLevelLayers = m_pGameInstance->Get_MapLevelLayers();

    auto iter1 = pMapLevelLayers[iGamePlayLevel].find(wstrLayer);
    if(iter1 == pMapLevelLayers[iGamePlayLevel].end())
    {
        MSG_BOX("Save Failed");
        return;
    }

    const CLayer* pLayer = iter1->second;
    auto objList = pLayer->Get_ObjList();

    nlohmann::json root = nlohmann::json::array();

    for (auto& pObj : objList) {
        string strClassName = typeid(*pObj).name();

        if (strClassName.size() >= 14) {
            strClassName.erase(0, 14);
        }

        auto& mapComponent = pObj->Get_Componets();
        auto iter2 = mapComponent.find(g_strTransformTag);
        if (iter2 == mapComponent.end()) {
            MSG_BOX("Transform error 1");
            continue;
        }

        CTransform* pTransform = dynamic_cast<CTransform*>(iter2->second);
        if (pTransform == nullptr) {
            MSG_BOX("Transform error 2");
            continue;
        }

        _float3 vScale = pTransform->Get_Scaled();
        _float3 vPos;
        XMStoreFloat3(&vPos, pTransform->Get_State(STATE::POSITION));

        nlohmann::json jsonObj;
        jsonObj["Class_name"] = strClassName;
        jsonObj["Scale"] = { vScale.x, vScale.y, vScale.z };
        jsonObj["Position"] = { vPos.x, vPos.y, vPos.z };

        root.push_back(jsonObj);
    }

    std::ofstream ofs("../Bin/Data/GamePlay_Obj.json");
    if (!ofs.is_open())
    {
        MSG_BOX("File Open Failed");
        return;
    }   

    ofs << root.dump(4);
    ofs.close();

    MSG_BOX("Saved");
}

void CImGui_Manager::GamePlayLevel_Load()
{
    std::ifstream ifs("../Bin/Data/GamePlay_Obj.json");
    if (!ifs.is_open())
    {
        MSG_BOX("File Open Failed");
        return;
    }

    nlohmann::json root;
    ifs >> root;
    ifs.close();

    if (!root.is_array())
    {
        MSG_BOX("Json Format Error");
        return;
    }

    for (auto& jsonObj : root)
    {
        LoadedObj tLoadedObj{};
         
        string strClassName = jsonObj["Class_name"].get<std::string>();
        // 영어, 숫자, 기본 아스키만 안전
        tLoadedObj.wstrClassName = wstring(strClassName.begin(), strClassName.end());

        tLoadedObj.vSacle.x = jsonObj["Scale"][0].get<_float>();
        tLoadedObj.vSacle.y = jsonObj["Scale"][1].get<_float>();
        tLoadedObj.vSacle.z = jsonObj["Scale"][2].get<_float>();

        tLoadedObj.vPos.x = jsonObj["Position"][0].get<_float>();
        tLoadedObj.vPos.y = jsonObj["Position"][1].get<_float>();
        tLoadedObj.vPos.z = jsonObj["Position"][2].get<_float>();

        Create_Object(tLoadedObj);
    }

    MSG_BOX("Loaded");
}

void CImGui_Manager::Create_Object(LoadedObj& tLoadedObj)
{
    if (tLoadedObj.wstrClassName == L"CForkLift") {

        CForkLift::FORKLIFT_DESC Desc{};

        Desc.tTransformDesc.vStartPos = tLoadedObj.vPos;

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_ForkLift"),
            ETOUI(LEVEL::GAMEPLAY), wstrLayer, &Desc)))
        {
            MSG_BOX("Failed to Created: Prototype_GameObject_ForkLift");
        }
    }
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