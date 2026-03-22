#ifdef _DEBUG

#include "CImGui_Manager.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (pDevice == nullptr || pContext == nullptr) {
        return E_FAIL;
    }

    //현재 프로젝트에 포함된 imgui.h의 버전과, 링크된 ImGui 라이브러리(컴파일된 바이너리)의 버전이 일치 검사
    IMGUI_CHECKVERSION();

    //ImGui가 내부적으로 사용할 폰트 아틀라스, 드로우 리스트, 윈도우 상태 등을 저장할 메모리 공간(Context)을 할당
    if (!ImGui::CreateContext()) {
        return E_FAIL;
    }

    //ImGui 시스템의 설정과 입출력을 담당하는 구조체 가져옴
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 도킹 활성화
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // ImGui 테마
    ImGui::StyleColorsClassic();

    //윈도우 핸들을 넘겨주어, ImGui가 마우스 클릭, 키보드 입력, 윈도우 리사이징 등의 이벤트를 감지할 수 있게
    if (!ImGui_ImplWin32_Init(g_hWnd)) {
        ImGui::DestroyContext();
        return E_FAIL;
    }

    //ImGui가 UI를 그리기 위한 텍스처를 생성하고, 버텍스 버퍼를 만들 수 있게 함
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
    // 1. 프레임 시작 (필수)
 //DirectX 9 관련 리소스를 업데이트 (e.g. 창 크기가 변했다면 렌더 타겟, 크기 갱신 등)
    ImGui_ImplDX11_NewFrame();
    //Windows 입력 장치 상태를 업데이트
    //마우스 커서의 위치, 클릭 여부, 키보드 입력 값, 델타 타임(프레임 간 시간) 등을 ImGui io 구조체에 채워 넣음
    ImGui_ImplWin32_NewFrame();
    //이번 프레임에 추가할 UI 위젯(버튼, 창, 텍스트 등) 정의 시작
    ImGui::NewFrame();

    // 뷰포트 영역 전체를 덮는 DockSpace용 부모 영역을 하나 만든다.
    ImGui::DockSpaceOverViewport(
        0,                                                              // DockSpace ID
        ImGui::GetMainViewport(),                              // 이미 존재하는 메인 뷰포트를 가지고 온다.
        ImGuiDockNodeFlags_PassthruCentralNode        // 창이 도킹되지 않은 영역은 투명하게 하라.
    );

    //가장 기본이 되는 메인 메뉴 창을 띄울지 여부
    static _bool show_begin_window = FALSE;           // 기본 제어 창

    //ImGui가 제공하는 예제 창
    static _bool show_demo_window = FALSE;           // ImGui 데모 창

    //개발자가 직접 만든 툴 창들의 가시성 여부
    static _bool show_map_editor_window = FALSE;
    static _bool show_gameObject_list_window = FALSE;
    static _bool show_component_prototype_window = FALSE;

    //왼쪽에 뭔 이상한 창
    static _bool show_gameObject_setting_window = FALSE;

    //키를 꾹 누르고 있는 상태와 한 번만 딱 눌린 순간을 구분하기 위한 용도
    static _bool bIsF6Pressed = FALSE;
    //현재 UI 전체가 숨겨진 상태인지 아닌지를 저장하는 최종 상태 값(아직 쓸모없음(구현x). 메인 창 닫았을 때 기존 창 복구용)
    static _bool bAll_UI_Off = TRUE;

    // 2. F6 키 입력 처리 (한 번만 눌리게 처리)
    if (GetAsyncKeyState(VK_F6) & 0x8000)
    {
        if (!bIsF6Pressed) // 눌린 순간 한 번만 실행
        {
            bIsF6Pressed = TRUE;
            bAll_UI_Off = !bAll_UI_Off; // 상태 반전

            // 모든 창 끄기/켜기
            _bool bNextState = !bAll_UI_Off;
            show_begin_window = bNextState;

            // 필요하다면 다른 창들도 같이 끄게 설정 가능
            //if (bAll_UI_Off) {
            //   show_demo_window = FALSE;
            //   show_map_editor_window = FALSE;
            //}
        }
    }
    else
    {
        // 키 떼면 초기화
        bIsF6Pressed = FALSE;
    }

    // 3. Basic Window (메뉴판) 그리기
    if (show_begin_window)
    {
        //화면에 Basic Window라는 제목을 가진 윈도우 창
        ImGui::Begin("Basic Window", &show_begin_window);

        //단순히 하얀색 글씨를 출력
        ImGui::Text("F6 : Toggle UI On/Off");
        //가로로 긴 구분선(줄)
        ImGui::Separator();

        // 버튼으로도 전체 끄기/켜기 가능하게 On/Off F6 (F6) 버튼
        if (ImGui::Button("On/Off F6"))
        {
            bAll_UI_Off = !bAll_UI_Off;
            show_begin_window = !bAll_UI_Off;
        }

        //눈에 보이지 않는 투명한 박스
        //너비 0(자동), 높이 5 픽셀
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        //네모난 체크박스를 그림
        //변수의 주소를 넘겨주었기 때문에, 사용자가 체크박스를 클릭하면 ImGui가 알아서 해당 bool 변수 값을 true ↔ false로 바꿈
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Checkbox("Map Editor", &show_map_editor_window);
        ImGui::Checkbox("Object List", &show_gameObject_list_window);
        ImGui::Checkbox("Component Proto", &show_component_prototype_window);
        ImGui::Checkbox("Object Setting", &show_gameObject_setting_window);

        //ImGui::Begin("Basic Window")으로 열었던 창 정의 마무리
        ImGui::End();
    }

    // 4. 서브 윈도우 그리기 (체크된 것만 그림)

    if (bAll_UI_Off)
        return;

    // (1) 데모 창
    if (show_demo_window)
        //ImGui 라이브러리가 제공하는 설명서
        //주소값을 넘겨주면, 데모 창 우측 상단에 X (닫기) 버튼이 생김
        ImGui::ShowDemoWindow(&show_demo_window);

    // (2) 맵 에디터
    if (show_map_editor_window)
    {
        //창 만들기 시작
        //Map Editor Window: 창의 제목줄(Title Bar)에 표시될 이름
        //show_map_editor_window:: X 버튼
        ImGui::Begin("Map Editor Window", &show_map_editor_window);

        // Edit_Map(); // 구현부 있으면 주석 해제

        //내용 채우기
        ImGui::Text("Map Editor Area");
        //창 만들기 끝
        ImGui::End();
    }

    // (3) 오브젝트 리스트
    if (show_gameObject_list_window)
    {
        ImGui::Begin("Object List Window", &show_gameObject_list_window);
        ImGui::Text("Object List Area");
        ImGui::End();
    }

    // (4) 컴포넌트 프로토타입
    if (show_component_prototype_window)
    {
        ImGui::Begin("Component Window", &show_component_prototype_window);
        // Show_Component_Prototype(); 
        ImGui::Text("Component Area");
        ImGui::End();
    }

    // (5) 오브젝트 세팅
    if (show_gameObject_setting_window)
    {
        ImGui::Begin("Setting Window", &show_gameObject_setting_window);
        // Set_GameObject();
        ImGui::Text("Setting Area");
        ImGui::End();
    }
}

HRESULT CImGui_Manager::Render()
{
    //이번 프레임에 추가할 UI 위젯(버튼, 창, 텍스트 등) 정의가 모두 끝났다고 알림
    ImGui::EndFrame();
    //그릴 준비를 하는 단계
    //ImGui::NewFrame() 이후부터 지금까지 코드로 작성한 모든 UI(창 위치, 버튼 색상, 글자 등)를 분석
    //그래픽 카드가 이해할 수 있는 정점(Vertex) 데이터와 인덱스 버퍼 리스트를 계산해서 메모리에 만듬
    //이 결과물은 ImDrawData라는 구조체에 저장
    ImGui::Render();

    //ImGui::GetDrawData() : 방금 Render() 함수가 만들어낸 렌더링 데이터(좌표, 색상 정보 등)를 가져옴
    //데이터를 DirectX 9 명령어(DrawIndexedPrimitive 등)로 변환하여 GPU로 쏨
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

bool CImGui_Manager::IsInputCapturedByUI()
{
    // 마우스 입력(클릭, 이동, 휠 등)이 UI에 의해 캡처되었는지 확인
    return ImGui::GetIO().WantCaptureMouse;
}

void CImGui_Manager::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}

#endif