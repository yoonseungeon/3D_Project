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
    NORMAL, HIGH, RARE, HERO
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

    float fHPRecovery{};

    unsigned int iPenetrationDefense{};

    unsigned int iDefense{};

    float fATKSpeed{};
    unsigned int iCritical{};
    float fSpeed{};

    //
    unsigned int iMaxCnt{1};
    int materials[2]{ -1, -1 };
};

static ITEM_DESC ItemInfos[89] = {
    // NORMAL
    { 0,  0,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 1,  1,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 2,  2,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 3,  3,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 4,  4,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 5,  5,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 6,  6,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 7,  7,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 8,  8,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 9,  9,  L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 10, 10, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 11, 11, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 12, 12, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 13, 13, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 14, 14, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 15, 15, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 16, 16, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 17, 17, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 18, 18, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 19, 19, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 20, 20, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 21, 21, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 22, 22, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 23, 23, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 24, 24, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 25, 25, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 26, 26, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 27, 27, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 28, 28, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 29, 29, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 30, 30, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 31, 31, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 32, 32, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 33, 33, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 34, 34, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 35, 35, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 36, 36, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 37, 37, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 38, 38, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 39, 39, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 40, 40, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 41, 41, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 42, 42, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 43, 43, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 44, 44, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 45, 45, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 46, 46, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 47, 47, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 48, 48, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 49, 49, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 50, 50, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 51, 51, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 52, 52, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::FOOD,     ITEM_GRADE::NORMAL, 0, 0.f, 0, 0, 0.f, 0, 0.f, 6, {-1, -1} },
    // HIGH
    { 53, 53, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {4 , 34} },
    { 54, 54, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::GLOVE,    ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {4 , 37} },
    { 55, 55, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {10, 49} },
    { 56, 56, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {12, 51} },
    { 57, 57, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {18, 36} },
    { 58, 58, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::ARM,      ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {20, 46} },
    { 59, 59, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::LEG,      ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {23, 33} },
    { 60, 60, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {27, 44} },
    { 61, 61, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {7 ,  5} },
    { 62, 62, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {26,  3} },
    { 63, 63, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {44, 51} },
    { 64, 64, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {42, 20} },
    { 65, 65, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {45, 38} },
    { 66, 66, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {41, 50} },
    { 67, 67, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {1 , 45} },
    { 68, 68, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {37, 47} },
    { 69, 69, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {2 , 44} },
    { 70, 70, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {50, 49} },
    { 71, 71, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {38, 44} },
    { 72, 72, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {45, 34} },
    { 73, 73, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {48, 38} },
    { 74, 74, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {47,  7} },
    { 75, 75, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {41, 48} },
    { 76, 76, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {34, 43} },
    { 77, 77, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 78, 78, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 79, 79, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 80, 80, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 81, 81, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::MATERIAL, ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 2, {-1, -1} },
    { 82, 82, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::FOOD,     ITEM_GRADE::HIGH,   0, 0.f, 0, 0, 0.f, 0, 0.f, 6, {52, -1} },
    // RARE
    { 83, 83, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::RARE,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {55, 33} },
    // HERO                                                                        
    { 84, 84, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::NUNCHAKU, ITEM_GRADE::HERO,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {83, 41} },
    { 85, 85, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::HEAD,     ITEM_GRADE::HERO,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {56, 75} },
    { 86, 86, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::CLOTH,    ITEM_GRADE::HERO,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {57, 61} },
    { 87, 87, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::ARM,      ITEM_GRADE::HERO,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {58, 60} },
    { 88, 88, L"Empty", L"None", (MAP_NAME)0, ITEM_TYPE::LEG,      ITEM_GRADE::HERO,   0, 0.f, 0, 0, 0.f, 0, 0.f, 1, {59, 61} }
};