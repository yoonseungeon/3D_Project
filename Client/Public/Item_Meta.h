#pragma once

enum class ITEM_TYPE
{
    NUNCHAKU,
    GLOVE,
    CLOTH,
    HEAD,
    ARM,
    LEG,
    MATERIAL,
    FOOD
};

enum class ITEM_GRADE
{
    NORMAL, HIGH, RARE, HERO, LEGENDARY, TRANSCENDENCE, ITEM_GRADE_END
};

static float ItemCraftTime[static_cast<int>(ITEM_GRADE::ITEM_GRADE_END)]
{
    3.f, 3.f, 3.f, 3.f, 3.f, 3.f
};

enum class SPAWN_MAP {
    ALLEY           = 1 << 0,
    ARCHERY         = 1 << 1,
    CEMETERY        = 1 << 2,
    CHURCH          = 1 << 3,
    FACTORY         = 1 << 4,
    FIRE_STATION    = 1 << 5,
    FOREST          = 1 << 6,
    GAS_STATION     = 1 << 7,
    HARBOR          = 1 << 8,
    HOSPITAL        = 1 << 9,
    HOTEL           = 1 << 10,
  //LABORATORY      = 1 << 11,
    POLICE_STATION  = 1 << 12,
    POND            = 1 << 13,
    SANDY_BEACH     = 1 << 14,
    SCHOOL          = 1 << 15,
    STREAM          = 1 << 16,
    TEMPLE          = 1 << 17,
    UPTOWN          = 1 << 18,
    WAREHOUSE       = 1 << 19,
    MAP_END         = 1 << 20
};

struct ITEM_DESC
{
    unsigned int iItemID{};
    unsigned int iTextureIdx{};
    const wchar_t* ItemName{};
    const wchar_t* Instruction{};
    MAP_NAME eSpawnMap{};

    ITEM_TYPE eType{};
    ITEM_GRADE eGrade{};


    unsigned int iATKPower{};

    unsigned int iHP{};
    float fHPRecoveryPercent{};

    unsigned int iPenetrationDefense{};

    unsigned int iDefense{};

    float fATKSpeed{};
    unsigned int iCritical{};
    float fSpeed{};

    unsigned int iConsumableHP{};

    unsigned int iCraftCnt{};

    //
    unsigned int iMaxCnt{1};
    int materials[2]{ -1, -1 };
};

static ITEM_DESC ItemInfos[89] = {
    // NORMAL
    { 0,  0,  L"가위",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 1,  1,  L"만년필",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 2,  2,  L"곡괭이",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 3,  3,  L"나뭇가지",      L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 4,  4,  L"목장갑",        L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::NORMAL, 7,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 5,  5,  L"돌멩이",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 6,  6,  L"쇠구슬",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 7,  7,  L"유리병",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 8,  8,  L"트럼프 카드",   L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 9,  9,  L"분필",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 10, 10, L"쇠사슬",        L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::NORMAL, 13, 0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 11, 11, L"머리띠",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  1,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 12, 12, L"모자",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  2,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 13, 13, L"자전거 헬멧",   L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  30,   0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 14, 14, L"가면",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  20,   0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 15, 15, L"바람막이",      L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 4,  0,    0.f, 0,  3,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 16, 16, L"승복",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  9,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 17, 17, L"전신 수영복",   L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  9,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 18, 18, L"셔츠",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  9,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 19, 19, L"손목시계",      L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  8.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 20, 20, L"붕대",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,   20.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 21, 21, L"팔찌",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  2,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 22, 22, L"슬리퍼",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.12f,   0, 1, 2, {-1, -1} },
    { 23, 23, L"운동화",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.12f,   0, 1, 2, {-1, -1} },
    { 24, 24, L"타이즈",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.12f,   0, 1, 2, {-1, -1} },
    { 25, 25, L"나막신",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.12f,   0, 1, 2, {-1, -1} },
    { 26, 26, L"깃털",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 27, 27, L"꽃",            L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 28, 28, L"리본",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 29, 29, L"십자가",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 30, 30, L"쌍안경",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 31, 31, L"탄창",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 32, 32, L"얼음",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 33, 33, L"못",            L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 34, 34, L"가죽",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 35, 35, L"거북이 등딱지", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 36, 36, L"고무",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 37, 37, L"고철",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 38, 38, L"라이터",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 39, 39, L"레이저 포인터", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 40, 40, L"마패",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 41, 41, L"배터리",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 42, 42, L"오일",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 43, 43, L"옷감",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 44, 44, L"원석",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 45, 45, L"종이",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 46, 46, L"화약",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 47, 47, L"화약품",        L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 48, 48, L"흑연",          L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 49, 49, L"플라스틱",      L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 50, 50, L"피아노선",      L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 51, 51, L"실",            L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {-1, -1} },
    { 52, 52, L"고기",          L"None", (MAP_NAME)0, ITEM_TYPE::FOOD,     ITEM_GRADE::NORMAL, 0,  0,    0.f, 0,  0,  0.f, 0, 0.f,   300, 1, 6, {-1, -1} },
    // HIGH                                                                                                                          
    { 53, 53, L"글러브",        L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   18, 0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {4 , 34} },
    { 54, 54, L"아이언 너클",   L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   20, 0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {4 , 37} },
    { 55, 55, L"눈차크",        L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HIGH,   25, 0,    0.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {10, 49} },
    { 56, 56, L"사슬 코이프",   L"None", (MAP_NAME)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HIGH,   0,  0,    0.f, 0,  3,  0.f, 0, 0.f,     0, 1, 2, {12, 51} },
    { 57, 57, L"잠수복",        L"None", (MAP_NAME)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HIGH,   0,  0,    0.f, 0,  10, 0.f, 0, 0.f,     0, 1, 2, {18, 36} },
    { 58, 58, L"분대장 완장",   L"None", (MAP_NAME)0, ITEM_TYPE::ARM,      ITEM_GRADE::HIGH,   0,  0,   25.f, 0,  0,  0.f, 0, 0.f,     0, 1, 2, {20, 46} },
    { 59, 59, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::LEG,      ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {23, 33} },
    { 60, 60, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {27, 44} },
    { 61, 61, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {7 ,  5} },
    { 62, 62, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {26,  3} },
    { 63, 63, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {44, 51} },
    { 64, 64, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {42, 20} },
    { 65, 65, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {45, 38} },
    { 66, 66, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {41, 50} },
    { 67, 67, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {1 , 45} },
    { 68, 68, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {37, 47} },
    { 69, 69, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {2 , 44} },
    { 70, 70, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {50, 49} },
    { 71, 71, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {38, 44} },
    { 72, 72, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {45, 34} },
    { 73, 73, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {48, 38} },
    { 74, 74, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {47,  7} },
    { 75, 75, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {41, 48} },
    { 76, 76, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {34, 43} },
    { 77, 77, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {-1, -1} },
    { 78, 78, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {-1, -1} },
    { 79, 79, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {-1, -1} },
    { 80, 80, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {-1, -1} },
    { 81, 81, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 2, {-1, -1} },
    // RARE                                                                                                          
    { 82, 82, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::FOOD,     ITEM_GRADE::RARE,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 3, 6, {52, -1} },
    { 83, 83, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::RARE,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {55, 33} },
    // HERO                                                                                                          
    { 84, 84, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HERO,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {83, 41} },
    { 85, 85, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HERO,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {56, 75} },
    { 86, 86, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HERO,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {57, 61} },
    { 87, 87, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::ARM,      ITEM_GRADE::HERO,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {58, 60} },
    { 88, 88, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::LEG,      ITEM_GRADE::HERO,   0, 0, 0.f, 0, 0, 0.f, 0, 0.f, 0, 1, 1, {59, 61} }
};

struct INVENTORY_SLOT
{
    int iItemId{ -1 };
    unsigned int iItemCnt{};
};