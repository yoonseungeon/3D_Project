# 이터널 리턴 모작

## 프로젝트 소개

C++과 DirectX 11 기반 프레임워크를 활용해 제작한 1인 3D 쿼터뷰 액션 게임 프로젝트입니다. 이터널 리턴의 캐릭터 전투와 아이템 수집·제작을 중심으로 게임 흐름을 구현했습니다.

## 개발 정보

| 항목 | 내용 |
| --- | --- |
| 개발 기간 | 2026.02.19 ~ 2026.05.29 |
| 개발 인원 | 1명 |
| 개발 환경 | Windows x64, Visual Studio 2022, v143 |
| 언어 | C++, HLSL |
| 그래픽 | DirectX 11, Effects11 |
| 오디오 | FMOD |
| 도구 및 라이브러리 | oneTBB, Dear ImGui, DirectXTK, DirectXTex, nlohmann/json |
| 버전 관리 | Git, GitHub |

## 프로젝트 개요

- 로고, 로비, 캐릭터·스킨 선택, 맵 선택, 게임 플레이, 엔딩으로 이어지는 진행 구조
- 리 다이린의 이동, 기본 공격, 스킬, 애니메이션 및 전투 이펙트
- AI 피오라와 야생 동물을 대상으로 한 전투
- 아이템 상자, 자원 채집, 인벤토리, 장비 및 제작 시스템
- 내비게이션, 충돌, 오브젝트 피킹을 활용한 쿼터뷰 조작
- 낮과 밤, 시야 범위, 미니맵 및 게임 플레이 UI

## 개발 및 담당

| 담당자 | 주요 역할 |
| --- | --- |
| yoonseungeon | 엔진 및 게임 콘텐츠 전체 개발 |

## 저장소 구조

```text
3D_Project/
├─ Engine/          # 공용 엔진 및 렌더링 시스템
├─ Client/          # 캐릭터, 전투, 레벨, 아이템 및 UI
├─ External/        # 외부 SDK 연동 경로
├─ vcpkg/           # 패키지 관리자 서브모듈
├─ Setup.bat        # vcpkg 초기 설정
├─ UpdateLib.bat    # 엔진 빌드 결과 복사
└─ 3DProject.sln    # Visual Studio 솔루션
```

## 빌드 방법

1. FMOD SDK를 `External/FMOD` 경로에 구성합니다.
2. `Setup.bat`을 실행해 vcpkg 서브모듈과 개발 환경을 설정합니다.
3. Visual Studio 2022에서 `3DProject.sln`을 열고 x64 환경으로 빌드합니다.
4. Engine → Client 순서로 빌드합니다.

원작 리소스와 FMOD SDK는 저장소에 포함되어 있지 않습니다.
