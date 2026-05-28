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
    1.f, 1.3f, 1.5f, 2.0f, 2.5f, 3.f
};

static float ItemCollectTime[static_cast<int>(ITEM_GRADE::ITEM_GRADE_END)]
{
    1.5, 1.5, 1.5, 1.5, 1.5, 1.5
};

enum SPAWN_MAP {
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
    unsigned int    iItemID{};
    unsigned int    iTextureIdx{};
    const wchar_t*  ItemName{};
    const wchar_t*  Instruction{};
    SPAWN_MAP        eSpawnMap{};

    ITEM_TYPE   eType{};
    ITEM_GRADE  eGrade{};

    int     iATKPower{};

    int     iHP{};
    float   fHPRecoveryPercent{};

    int     iPenetrationDefense{};
    int     iDefense{};

    float   fATKSpeed{};
    int     iCritical{};
    float   fSpeed{};

    unsigned int iConsumableHP{};

    unsigned int iCraftCnt{};

    int     iMaxCnt{1};
    int     materials[2]{ -1, -1 };
};

static ITEM_DESC ItemInfos[89] = {
    // NORMAL
    { 0,  0,  L"가위",          L"None", static_cast<SPAWN_MAP>(ALLEY | ARCHERY | SCHOOL | HOSPITAL | UPTOWN | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 1,  1,  L"만년필",        L"None", static_cast<SPAWN_MAP>(ALLEY | SCHOOL | FIRE_STATION | HOTEL | UPTOWN | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 2,  2,  L"곡괭이",        L"None", static_cast<SPAWN_MAP>(ALLEY | TEMPLE | CEMETERY | FOREST | SANDY_BEACH | WAREHOUSE), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 3,  3,  L"나뭇가지",      L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 4,  4,  L"목장갑",        L"None", (SPAWN_MAP)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::NORMAL,  7,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 5,  5,  L"돌멩이",        L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 6,  6,  L"쇠구슬",        L"None", static_cast<SPAWN_MAP>(POLICE_STATION | STREAM | FIRE_STATION | ARCHERY | FACTORY | UPTOWN), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 7,  7,  L"유리병",        L"None", static_cast<SPAWN_MAP>(POLICE_STATION | STREAM | FIRE_STATION | ARCHERY | FACTORY | UPTOWN), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 8,  8,  L"트럼프 카드",   L"None", static_cast<SPAWN_MAP>(FIRE_STATION | STREAM | HOTEL | CHURCH | UPTOWN), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 9,  9,  L"분필",          L"None", static_cast<SPAWN_MAP>(POLICE_STATION | STREAM | SCHOOL | CHURCH | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 10, 10, L"쇠사슬",        L"None", static_cast<SPAWN_MAP>(POLICE_STATION | CEMETERY | FOREST | HARBOR), ITEM_TYPE::NUNCHAKU, ITEM_GRADE::NORMAL, 13,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 11, 11, L"머리띠",        L"None", static_cast<SPAWN_MAP>(TEMPLE | STREAM | POND | CHURCH | HOTEL | WAREHOUSE), ITEM_TYPE::HEAD, ITEM_GRADE::NORMAL,      0,  0,    0.f, 0,  1,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 12, 12, L"모자",          L"None", static_cast<SPAWN_MAP>(ALLEY | ARCHERY | FOREST | CHURCH | UPTOWN | WAREHOUSE | HOSPITAL), ITEM_TYPE::HEAD, ITEM_GRADE::NORMAL,      0,  0,    0.f, 0,  2,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 13, 13, L"자전거 헬멧",   L"None", static_cast<SPAWN_MAP>(GAS_STATION | FIRE_STATION | FOREST | UPTOWN | FACTORY), ITEM_TYPE::HEAD, ITEM_GRADE::NORMAL,      0, 30,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 14, 14, L"가면",          L"None", static_cast<SPAWN_MAP>(SCHOOL | CEMETERY | FACTORY | UPTOWN), ITEM_TYPE::HEAD, ITEM_GRADE::NORMAL,      0, 20,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 15, 15, L"바람막이",      L"None", static_cast<SPAWN_MAP>(SCHOOL | SANDY_BEACH | HARBOR | POND), ITEM_TYPE::CLOTH, ITEM_GRADE::NORMAL,     4,  0,    0.f, 0,  3,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 16, 16, L"승복",          L"None", static_cast<SPAWN_MAP>(ARCHERY | FOREST | CEMETERY | TEMPLE), ITEM_TYPE::CLOTH, ITEM_GRADE::NORMAL,     0,  0,    0.f, 0,  9,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 17, 17, L"전신 수영복",   L"None", static_cast<SPAWN_MAP>(STREAM | POND | SANDY_BEACH | HARBOR), ITEM_TYPE::CLOTH, ITEM_GRADE::NORMAL,     0,  0,    0.f, 0,  9,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 18, 18, L"셔츠",          L"None", static_cast<SPAWN_MAP>(GAS_STATION | STREAM | HOTEL | CHURCH), ITEM_TYPE::CLOTH, ITEM_GRADE::NORMAL,     0,  0,    0.f, 0,  9,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 19, 19, L"손목시계",      L"None", static_cast<SPAWN_MAP>(GAS_STATION | POLICE_STATION | HOSPITAL | FACTORY | HARBOR | UPTOWN), ITEM_TYPE::ARM, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  0,   8.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 20, 20, L"붕대",          L"None", static_cast<SPAWN_MAP>(SCHOOL | FIRE_STATION | FOREST | HOSPITAL | FACTORY | HARBOR), ITEM_TYPE::ARM, ITEM_GRADE::NORMAL,       0,  0,   20.f, 0,  0,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 21, 21, L"팔찌",          L"None", static_cast<SPAWN_MAP>(ALLEY | HOTEL | UPTOWN | WAREHOUSE | CEMETERY | HOSPITAL), ITEM_TYPE::ARM, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  2,   0.f, 0,   0.f,     0, 1, 1, {-1, -1} },
    { 22, 22, L"슬리퍼",        L"None", static_cast<SPAWN_MAP>(SCHOOL | HOTEL | STREAM | CHURCH), ITEM_TYPE::LEG, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  0,   0.f, 0, 0.12f,     0, 1, 1, {-1, -1} },
    { 23, 23, L"운동화",        L"None", static_cast<SPAWN_MAP>(ARCHERY | SANDY_BEACH | TEMPLE | FACTORY), ITEM_TYPE::LEG, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  0,   0.f, 0, 0.12f,     0, 1, 1, {-1, -1} },
    { 24, 24, L"타이즈",        L"None", static_cast<SPAWN_MAP>(TEMPLE | HOSPITAL | FOREST | UPTOWN), ITEM_TYPE::LEG, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  0,   0.f, 0, 0.12f,     0, 1, 1, {-1, -1} },
    { 25, 25, L"나막신",        L"None", static_cast<SPAWN_MAP>(SANDY_BEACH | FOREST | POND | TEMPLE), ITEM_TYPE::LEG, ITEM_GRADE::NORMAL,       0,  0,    0.f, 0,  0,   0.f, 0, 0.12f,     0, 1, 1, {-1, -1} },
    { 26, 26, L"깃털",          L"None", static_cast<SPAWN_MAP>(ARCHERY | TEMPLE | STREAM | POND | FOREST | WAREHOUSE | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 27, 27, L"꽃",            L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 28, 28, L"리본",          L"None", static_cast<SPAWN_MAP>(ALLEY | POLICE_STATION | SCHOOL | POND | CEMETERY | UPTOWN | WAREHOUSE), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 29, 29, L"십자가",        L"None", static_cast<SPAWN_MAP>(ALLEY | FIRE_STATION | SCHOOL | CEMETERY | CHURCH), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 30, 30, L"쌍안경",        L"None", static_cast<SPAWN_MAP>(POLICE_STATION | ARCHERY | FOREST | SANDY_BEACH), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 31, 31, L"탄창",          L"None", static_cast<SPAWN_MAP>(POLICE_STATION | TEMPLE | HOTEL | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 32, 32, L"얼음",          L"None", static_cast<SPAWN_MAP>(SCHOOL | HOTEL | FOREST | STREAM | HOSPITAL), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 33, 33, L"못",            L"None", static_cast<SPAWN_MAP>(ALLEY | ARCHERY | FIRE_STATION | HOTEL | CHURCH | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 34, 34, L"가죽",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 35, 35, L"거북이 등딱지", L"None", static_cast<SPAWN_MAP>(TEMPLE | STREAM | POND | FOREST | SANDY_BEACH | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 36, 36, L"고무",          L"None", static_cast<SPAWN_MAP>(GAS_STATION | FIRE_STATION | HOSPITAL | FACTORY | SANDY_BEACH | WAREHOUSE), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 37, 37, L"고철",          L"None", static_cast<SPAWN_MAP>(GAS_STATION | FIRE_STATION | HOSPITAL | FACTORY | SANDY_BEACH | WAREHOUSE), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 38, 38, L"라이터",        L"None", static_cast<SPAWN_MAP>(GAS_STATION | ALLEY | POLICE_STATION | SCHOOL | FOREST | CEMETERY | UPTOWN | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 39, 39, L"레이저 포인터", L"None", static_cast<SPAWN_MAP>(GAS_STATION | POLICE_STATION | SCHOOL | FOREST | HOSPITAL), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 40, 40, L"마패",          L"None", static_cast<SPAWN_MAP>(GAS_STATION | POLICE_STATION | TEMPLE | CEMETERY | FACTORY | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 41, 41, L"배터리",        L"None", static_cast<SPAWN_MAP>(GAS_STATION | FIRE_STATION | HOTEL | TEMPLE | HOSPITAL | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 42, 42, L"오일",          L"None", static_cast<SPAWN_MAP>(POND | HOSPITAL | FACTORY | SANDY_BEACH | HOTEL | GAS_STATION), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 43, 43, L"옷감",          L"None", static_cast<SPAWN_MAP>(GAS_STATION | TEMPLE | HOTEL | UPTOWN | CHURCH | CEMETERY | HOSPITAL), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 44, 44, L"원석",          L"None", static_cast<SPAWN_MAP>(ALLEY | TEMPLE | STREAM | POND | CEMETERY | FOREST | SANDY_BEACH | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 45, 45, L"종이",          L"None", static_cast<SPAWN_MAP>(ALLEY | TEMPLE | STREAM | POND | CEMETERY | FOREST | SANDY_BEACH | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 46, 46, L"화약",          L"None", static_cast<SPAWN_MAP>(ARCHERY | FIRE_STATION | POLICE_STATION | STREAM | FOREST | CHURCH), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 47, 47, L"화약품",        L"None", static_cast<SPAWN_MAP>(GAS_STATION | SCHOOL | FIRE_STATION | CEMETERY | HOSPITAL | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 48, 48, L"흑연",          L"None", static_cast<SPAWN_MAP>(POLICE_STATION | POND | SCHOOL | ARCHERY | CEMETERY | SANDY_BEACH | WAREHOUSE), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 49, 49, L"플라스틱",      L"None", static_cast<SPAWN_MAP>(ALLEY | FIRE_STATION | STREAM | CEMETERY | SANDY_BEACH | WAREHOUSE | FACTORY), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 50, 50, L"피아노선",      L"None", static_cast<SPAWN_MAP>(ALLEY | POLICE_STATION | STREAM | ARCHERY | HOTEL | CHURCH | UPTOWN), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 51, 51, L"실",            L"None", static_cast<SPAWN_MAP>(ARCHERY | SCHOOL | FIRE_STATION | POND | CHURCH | SANDY_BEACH | HARBOR), ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,     0, 1, 2, {-1, -1} },
    { 52, 52, L"고기",          L"None", (SPAWN_MAP)0, ITEM_TYPE::FOOD,     ITEM_GRADE::NORMAL,  0,  0,    0.f, 0,  0,   0.f, 0,   0.f,   300, 1, 6, {-1, -1} },
    // HIGH                                                                                                                       
    { 53, 53, L"글러브",        L"None", (SPAWN_MAP)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   18, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {4 , 34} },
    { 54, 54, L"아이언 너클",   L"None", (SPAWN_MAP)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   20, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {4 , 37} },
    { 55, 55, L"눈차크",        L"None", (SPAWN_MAP)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HIGH,   25, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {10, 49} },
    { 56, 56, L"사슬 코이프",   L"None", (SPAWN_MAP)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  3,   0.f, 0,    0.f,     0, 1, 1, {12, 51} },
    { 57, 57, L"잠수복",        L"None", (SPAWN_MAP)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HIGH,    0, 0,    0.f, 0, 10,   0.f, 0,    0.f,     0, 1, 1, {18, 36} },
    { 58, 58, L"분대장 완장",   L"None", (SPAWN_MAP)0, ITEM_TYPE::ARM,      ITEM_GRADE::HIGH,    0, 0,   25.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {20, 46} },
    { 59, 59, L"하이힐",        L"None", (SPAWN_MAP)0, ITEM_TYPE::LEG,      ITEM_GRADE::HIGH,    2, 0,    0.f, 0,  0,   0.f, 0,  0.16f,     0, 1, 1, {23, 33} },
    { 60, 60, L"운명의 꽃",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {27, 44} },
    { 61, 61, L"유리 조각",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {7 ,  5} },
    { 62, 62, L"화살통",        L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {26,  3} },
    { 63, 63, L"비파단도",      L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {44, 51} },
    { 64, 64, L"기름먹인 천",   L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {42, 20} },
    { 65, 65, L"재",            L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {45, 38} },
    { 66, 66, L"전자 부품",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {41, 50} },
    { 67, 67, L"정교한 도면",   L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {1 , 45} },
    { 68, 68, L"철판",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {37, 47} },
    { 69, 69, L"황금",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {2 , 44} },
    { 70, 70, L"철사",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {50, 49} },
    { 71, 71, L"루비",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {38, 44} },
    { 72, 72, L"하드커버",      L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {45, 34} },
    { 73, 73, L"다이아몬드",    L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {48, 38} },
    { 74, 74, L"독약",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {47,  7} },
    { 75, 75, L"이온 전지",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {41, 48} },
    { 76, 76, L"덧댄 가죽",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {34, 43} },
    // HERO                                                                                                                        
    { 77, 77, L"생명의 나무",   L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HERO,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {-1, -1} },
    { 78, 78, L"운석",          L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HERO,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {-1, -1} },
    { 79, 79, L"미스릴",        L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HERO,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {-1, -1} },
    { 80, 80, L"VF 혈액 샘플",  L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HERO,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {-1, -1} },
    { 81, 81, L"포스 코어",     L"None", (SPAWN_MAP)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HERO,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 2, {-1, -1} },
    // RARE                                                                                                    
    { 82, 82, L"월던 스테이크", L"None", (SPAWN_MAP)0, ITEM_TYPE::FOOD,     ITEM_GRADE::RARE,    0, 0,    0.f, 0,  0,   0.f, 0,    0.f,   550, 3, 6, {52, -1} },
    { 83, 83, L"블리더",        L"None", (SPAWN_MAP)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::RARE,   30, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {55, 33} },
    // HERO                                                                                                                        
    { 84, 84, L"초진동눈차크",  L"None", (SPAWN_MAP)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HERO,   63, 0,    0.f, 0,  0,   0.f, 0,    0.f,     0, 1, 1, {83, 41} },
    { 85, 85, L"플라즈마 투구", L"None", (SPAWN_MAP)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HERO,   35, 0,    0.f, 8,  0,   0.f, 0,  0.06f,     0, 1, 1, {56, 75} }, // 이속
    { 86, 86, L"고스트",        L"None", (SPAWN_MAP)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HERO,   14, 0,    0.f, 0, 18,   0.f, 34,   0.f,     0, 1, 1, {57, 61} },
    { 87, 87, L"미닛맨의 표식", L"None", (SPAWN_MAP)0, ITEM_TYPE::ARM,      ITEM_GRADE::HERO,   25, 0,    0.f, 0,  0, 0.15f, 24,   0.f,     0, 1, 1, {58, 61} }, // 공속
    { 88, 88, L"부케팔로스",    L"None", (SPAWN_MAP)0, ITEM_TYPE::LEG,      ITEM_GRADE::HERO,    8, 0,    0.f, 0,  0,   0.f, 25, 0.22f,     0, 1, 1, {59, 61} }
};

struct ITEMBOX_META
{
    const char* MODEL_PATH;
    const wchar_t* PROTYPE_TAG;
};

static const ITEMBOX_META ItemBoxMeta[]{
    {"ItemBox_AltarBig_Re.mymodel",         L"Prototype_Component_Model_ItemBox_AltarBig_Re"},
    {"ItemBox_AltarTable.mymodel",          L"Prototype_Component_Model_ItemBox_AltarTable"},
    {"ItemBox_ATM_01.mymodel",              L"Prototype_Component_Model_ItemBox_ATM_01"},
    {"ItemBox_Bag_01.mymodel",              L"Prototype_Component_Model_ItemBox_Bag_01"},
    {"ItemBox_Bag_02.mymodel",              L"Prototype_Component_Model_ItemBox_Bag_02"},
    {"ItemBox_BarbequeGrill_01.mymodel",    L"Prototype_Component_Model_ItemBox_BarbequeGrill_01"},
    {"ItemBox_BeachChair.mymodel",          L"Prototype_Component_Model_ItemBox_BeachChair"},
    {"ItemBox_Biotoilet_01.mymodel",        L"Prototype_Component_Model_ItemBox_Biotoilet_01"},
    {"ItemBox_Boat_01.mymodel",             L"Prototype_Component_Model_ItemBox_Boat_01"},
    {"ItemBox_Box_01.mymodel",              L"Prototype_Component_Model_ItemBox_Box_01"},
    {"ItemBox_Box_02.mymodel",              L"Prototype_Component_Model_ItemBox_Box_02"},
    {"ItemBox_CartonBox_01.mymodel",        L"Prototype_Component_Model_ItemBox_CartonBox_01"},
    {"ItemBox_CementMixer.mymodel",         L"Prototype_Component_Model_ItemBox_CementMixer"},
    {"ItemBox_CoffeeMachine_01.mymodel",    L"Prototype_Component_Model_ItemBox_CoffeeMachine_01"},
    {"ItemBox_Coffin_Large.mymodel",        L"Prototype_Component_Model_ItemBox_Coffin_Large"},
    {"ItemBox_Coffin_Middle.mymodel",       L"Prototype_Component_Model_ItemBox_Coffin_Middle"},
    {"ItemBox_ConcreteBag_Set_01.mymodel",  L"Prototype_Component_Model_ItemBox_ConcreteBag_Set_01"},
    {"ItemBox_ConcreteBag_Set_02.mymodel",  L"Prototype_Component_Model_ItemBox_ConcreteBag_Set_02"},
    {"ItemBox_Confessional_01.mymodel",     L"Prototype_Component_Model_ItemBox_Confessional_01"},
    {"ItemBox_Drum_W.mymodel",              L"Prototype_Component_Model_ItemBox_Drum_W"},
    {"ItemBox_Dumpster_01.mymodel",         L"Prototype_Component_Model_ItemBox_Dumpster_01"},
    {"ItemBox_EraserCleaner_01.mymodel",    L"Prototype_Component_Model_ItemBox_EraserCleaner_01"},
    {"ItemBox_FilingCabinets_01.mymodel",   L"Prototype_Component_Model_ItemBox_FilingCabinets_01"},
    {"ItemBox_FilingCabinets_02.mymodel",   L"Prototype_Component_Model_ItemBox_FilingCabinets_02"},
    {"ItemBox_GarbageBag_Set.mymodel",      L"Prototype_Component_Model_ItemBox_GarbageBag_Set"},
    {"ItemBox_Hospital_Cabinet_01.mymodel", L"Prototype_Component_Model_ItemBox_Hospital_Cabinet_01"},
    {"ItemBox_Hospital_Cart_01.mymodel",    L"Prototype_Component_Model_ItemBox_Hospital_Cart_01"},
    {"ItemBox_IceBox_B.mymodel",            L"Prototype_Component_Model_ItemBox_IceBox_B"},
    {"ItemBox_IceBox_R.mymodel",            L"Prototype_Component_Model_ItemBox_IceBox_R"},
    {"ItemBox_Jar_01.mymodel",              L"Prototype_Component_Model_ItemBox_Jar_01"},
    {"ItemBox_Jar_Big_01.mymodel",          L"Prototype_Component_Model_ItemBox_Jar_Big_01"},
    {"ItemBox_Locker_01.mymodel",           L"Prototype_Component_Model_ItemBox_Locker_01"},
    {"ItemBox_Organ_01.mymodel",            L"Prototype_Component_Model_ItemBox_Organ_01"},
    {"ItemBox_Pallet_Loaded_01.mymodel",    L"Prototype_Component_Model_ItemBox_Pallet_Loaded_01"},
    {"ItemBox_Quiver_01.mymodel",           L"Prototype_Component_Model_ItemBox_Quiver_01"},
    {"ItemBox_Sedan_Brown_01.mymodel",      L"Prototype_Component_Model_ItemBox_Sedan_Brown_01"},
    {"ItemBox_Sedan_Police_01.mymodel",     L"Prototype_Component_Model_ItemBox_Sedan_Police_01"},
    {"ItemBox_Sedan_Taxi_01.mymodel",       L"Prototype_Component_Model_ItemBox_Sedan_Taxi_01"},
    {"ItemBox_SteelBox_02.mymodel",         L"Prototype_Component_Model_ItemBox_SteelBox_02"},
    {"ItemBox_SteelBox_03.mymodel",         L"Prototype_Component_Model_ItemBox_SteelBox_03"},
    {"ItemBox_Suitcase_01.mymodel",         L"Prototype_Component_Model_ItemBox_Suitcase_01"},
    {"ItemBox_Suitcase_02.mymodel",         L"Prototype_Component_Model_ItemBox_Suitcase_02"},
    {"ItemBox_Suitcase_03.mymodel",         L"Prototype_Component_Model_ItemBox_Suitcase_03"},
    {"ItemBox_Suitcase_04.mymodel",         L"Prototype_Component_Model_ItemBox_Suitcase_04"},
    {"ItemBox_Switchboard_01.mymodel",      L"Prototype_Component_Model_ItemBox_Switchboard_01"},
    {"ItemBox_Temple_Box_01.mymodel",       L"Prototype_Component_Model_ItemBox_Temple_Box_01"},
    {"ItemBox_Temple_Box_02.mymodel",       L"Prototype_Component_Model_ItemBox_Temple_Box_02"},
    {"ItemBox_Temple_Lamp_01.mymodel",      L"Prototype_Component_Model_ItemBox_Temple_Lamp_01"},
    {"ItemBox_Temple_Thurible_01.mymodel",  L"Prototype_Component_Model_ItemBox_Temple_Thurible_01"},
    {"ItemBox_Tomb.mymodel",                L"Prototype_Component_Model_ItemBox_Tomb"},
    {"ItemBox_TrashCan_01.mymodel",         L"Prototype_Component_Model_ItemBox_TrashCan_01"},
    {"ItemBox_TrashCan_02.mymodel",         L"Prototype_Component_Model_ItemBox_TrashCan_02"},
    {"ItemBox_TrashCan_03.mymodel",         L"Prototype_Component_Model_ItemBox_TrashCan_03"},
    {"ItemBox_TreeofLife.mymodel",          L"Prototype_Component_Model_ItemBox_TreeofLife"},
    {"ItemBox_TreeofLife_Branch.mymodel",   L"Prototype_Component_Model_ItemBox_TreeofLife_Branch"},
    {"ItemBox_TreeStump_01.mymodel",        L"Prototype_Component_Model_ItemBox_TreeStump_01"},
    {"ItemBox_TreeStump_02.mymodel",        L"Prototype_Component_Model_ItemBox_TreeStump_02"},
    {"ItemBox_Uptown_BookCase_01.mymodel",  L"Prototype_Component_Model_ItemBox_Uptown_BookCase_01"},
    {"ItemBox_Uptown_Drawer_01.mymodel",    L"Prototype_Component_Model_ItemBox_Uptown_Drawer_01"},
    {"ItemBox_Uptown_FirePlace_01.mymodel", L"Prototype_Component_Model_ItemBox_Uptown_FirePlace_01"},
    {"ItemBox_VendingMachine_01.mymodel",   L"Prototype_Component_Model_ItemBox_VendingMachine_01"},
    {"ItemBox_VendingMachine_02.mymodel",   L"Prototype_Component_Model_ItemBox_VendingMachine_02"},
    {"ItemBox_Water_Dispenser_01.mymodel",  L"Prototype_Component_Model_ItemBox_Water_Dispenser_01"},
    {"ItemBox_WaterTank_01.mymodel",        L"Prototype_Component_Model_ItemBox_WaterTank_01"},
    {"ItemBox_Wheelbarrow.mymodel",         L"Prototype_Component_Model_ItemBox_Wheelbarrow"},
    {"ItemBox_WreckCar_Blue_01.mymodel",    L"Prototype_Component_Model_ItemBox_WreckCar_Blue_01"},
    {"ItemBox_WreckCar_White_01.mymodel",   L"Prototype_Component_Model_ItemBox_WreckCar_White_01"},
    {"ItemBox_WreckCar_Yellow_01.mymodel",  L"Prototype_Component_Model_ItemBox_WreckCar_Yellow_01"}
};

static const ITEMBOX_META ItemBoxCollectibleMeta[]{
    {"ItemBox_Branch.mymodel",         L"Prototype_Component_Model_ItemBox_Branch_01"},
    {"ItemBox_Pebbles.mymodel",        L"Prototype_Component_Model_ItemBox_Pebbles_01"},
    {"ItemBox_Flower.mymodel",         L"Prototype_Component_Model_ItemBox_Flower_01_active"},
};