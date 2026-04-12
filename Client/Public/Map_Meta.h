#pragma once

enum class MAP_NAME {
    ALLEY,
    ARCHERY,
    CEMETERY,
    CHURCH,
    FACTORY,
    FIRE_STATION,
    FOREST,
    GAS_STATION,
    HARBOR,
    HOSPITAL,
    HOTEL,
  //LABORATORY,
    POLICE_STATION,
    POND,
    SANDY_BEACH,
    SCHOOL,
    STREAM,
    TEMPLE,
    UPTOWN,
    WAREHOUSE,
    MAP_END
};

struct MAP_META
{
    const wchar_t* KR_MAP_NAME;

    const wchar_t* TEX_PATH_OVER;
    const wchar_t* TEX_PATH_SELECT;

    const wchar_t* TEX_OVER_TAG;
    const wchar_t* TEX_SELECT_TAG;

    const wchar_t* IMAGE_OVER_TAG;

    float fScaleRatioX{};
    float fScaleRatioY{};

    float fPosRatioX{};
    float fPosRatioY{};

};

constexpr MAP_META MAPS[]
{
    { L"골목길",      L"Map_Alley_Over.png",          L"Map_Alley_Select.png",          L"Prototype_Tex_Alley_Over",          L"Prototype_Tex_Alley_Select"          , L"Prototype_Image_Alley_Over",           0.298908f, 0.198211f,  0.056958f,  0.397398f},
    { L"양궁장",      L"Map_Archery_Over.png",        L"Map_Archery_Select.png",        L"Prototype_Tex_Archery_Over",        L"Prototype_Tex_Archery_Select"        , L"Prototype_Image_Archery_Over",         0.213715f, 0.211653f, -0.257771f,  0.260288f},
    { L"묘지",        L"Map_Cemetery_Over.png",       L"Map_Cemetery_Select.png",       L"Prototype_Tex_Cemetery_Over",       L"Prototype_Tex_Cemetery_Select"       , L"Prototype_Image_Cemetery_Over",        0.336149f, 0.256868f,  0.147019f, -0.109248f},
    { L"성당",        L"Map_Church_Over.png",         L"Map_Church_Select.png",         L"Prototype_Tex_Church_Over",         L"Prototype_Tex_Church_Select"         , L"Prototype_Image_Church_Over",          0.307913f, 0.239270f,  0.067425f, -0.231938f},
    { L"공장",        L"Map_Factory_Over.png",        L"Map_Factory_Select.png",        L"Prototype_Tex_Factory_Over",        L"Prototype_Tex_Factory_Select"        , L"Prototype_Image_Factory_Over",         0.317894f, 0.279108f,  0.309618f, -0.275197f},
    { L"소방서",      L"Map_FireStation_Over.png",    L"Map_FireStation_Select.png",    L"Prototype_Tex_FireStation_Over",    L"Prototype_Tex_FireStation_Select"    , L"Prototype_Image_FireStation_Over",     0.196188f, 0.253446f,  0.036511f,  0.179880f},
    { L"숲",          L"Map_Forest_Over.png",         L"Map_Forest_Select.png",         L"Prototype_Tex_Forest_Over",         L"Prototype_Tex_Forest_Select"         , L"Prototype_Image_Forest_Over",          0.298908f, 0.261022f, -0.131929f, -0.071366f},
    { L"주유소",      L"Map_GasStation_Over.png",     L"Map_GasStation_Select.png",     L"Prototype_Tex_GasStation_Over",     L"Prototype_Tex_GasStation_Select"     , L"Prototype_Image_GasStation_Over",      0.216636f, 0.248069f, -0.110021f,  0.347051f},
    { L"항구",        L"Map_Harbor_Over.png",         L"Map_Harbor_Select.png",         L"Prototype_Tex_Harbor_Over",         L"Prototype_Tex_Harbor_Select"         , L"Prototype_Image_Harbor_Over",          0.277244f, 0.182814f,  0.063774f, -0.407907f},
    { L"병원",        L"Map_Hospital_Over.png",       L"Map_Hospital_Select.png",       L"Prototype_Tex_Hospital_Over",       L"Prototype_Tex_Hospital_Select"       , L"Prototype_Image_Hospital_Over",        0.261179f, 0.246846f,  0.363897f, -0.039838f},
    { L"호텔",        L"Map_Hotel_Over.png",          L"Map_Hotel_Select.png",          L"Prototype_Tex_Hotel_Over",          L"Prototype_Tex_Hotel_Select"          , L"Prototype_Image_Hotel_Over",           0.304263f, 0.290351f, -0.316678f,  0.096050f},
  //{ L"연구소",      L"Map_Laboratory_Over.png",     L"Map_Laboratory_Select.png",     L"Prototype_Tex_Laboratory_Over",     L"Prototype_Tex_Laboratory_Select"     , L"Prototype_Image_Laboratory_Over",      0.f,       0.f,        0.f,        0.f      },
    { L"경찰서",      L"Map_PoliceStation_Over.png",  L"Map_PoliceStation_Select.png",  L"Prototype_Tex_PoliceStation_Over",  L"Prototype_Tex_PoliceStation_Select"  , L"Prototype_Image_PoliceStation_Over",   0.217123f, 0.229739f,  0.129007f,  0.256378f},
    { L"연못",        L"Map_Pond_Over.png",           L"Map_Pond_Select.png",           L"Prototype_Tex_Pond_Over",           L"Prototype_Tex_Pond_Select"           , L"Prototype_Image_Pond_Over",            0.221503f, 0.230960f,  0.164059f,  0.037883f},
    { L"모래사장",    L"Map_SandyBeach_Over.png",     L"Map_SandyBeach_Select.png",     L"Prototype_Tex_SandyBeach_Over",     L"Prototype_Tex_SandyBeach_Select"     , L"Prototype_Image_SandyBeach_Over",      0.289415f, 0.228028f, -0.350755f, -0.094339f},
    { L"학교",        L"Map_School_Over.png",         L"Map_School_Select.png",         L"Prototype_Tex_School_Over",         L"Prototype_Tex_School_Select"         , L"Prototype_Image_School_Over",          0.241464f, 0.258089f, -0.139231f,  0.165948f},
    { L"개울",        L"Map_Stream_Over.png",         L"Map_Stream_Select.png",         L"Prototype_Tex_Stream_Over",         L"Prototype_Tex_Stream_Select"         , L"Prototype_Image_Stream_Over",          0.219557f, 0.227539f,  0.289902f,  0.078942f},
    { L"절",          L"Map_Temple_Over.png",         L"Map_Temple_Select.png",         L"Prototype_Tex_Temple_Over",         L"Prototype_Tex_Temple_Select"         , L"Prototype_Image_Temple_Over",          0.318137f, 0.270065f,  0.302802f,  0.240247f},
    { L"고급 주택가", L"Map_Uptown_Over.png",         L"Map_Uptown_Select.png",         L"Prototype_Tex_Uptown_Over",         L"Prototype_Tex_Uptown_Select"         , L"Prototype_Image_Uptown_Over",          0.316190f, 0.243670f, -0.176474f, -0.228272f},
    { L"창고",        L"Map_Warehouse_Over.png",      L"Map_Warehouse_Select.png",      L"Prototype_Tex_Warehouse_Over",      L"Prototype_Tex_Warehouse_Select"      , L"Prototype_Image_Warehouse_Over",       0.247548f, 0.201632f, -0.047221f, -0.351450f}
};

constexpr unsigned int iRoofCnt = { 31 };

struct ROOF_META
{
    const char* MODEL_PATH;
    const wchar_t* PROTYPE_TAG;
};

constexpr ROOF_META ROOFS[]
{
    { "Alley_Roof1.mymodel",         L"Prototype_Component_Model_Alley_Roof1" },
    { "Alley_Roof2.mymodel",         L"Prototype_Component_Model_Alley_Roof2" },
    { "Archery_Roof1.mymodel",       L"Prototype_Component_Model_Archery_Roof1" },
    { "Archery_Roof2.mymodel",       L"Prototype_Component_Model_Archery_Roof2" },
    { "Church_Roof1.mymodel",        L"Prototype_Component_Model_Church_Roof1" },
    { "Church_Roof2.mymodel",        L"Prototype_Component_Model_Church_Roof2" },
    { "Church_Roof3.mymodel",        L"Prototype_Component_Model_Church_Roof3" },
    { "Factory_Roof1.mymodel",       L"Prototype_Component_Model_Factory_Roof1" },
    { "Factory_Roof2.mymodel",       L"Prototype_Component_Model_Factory_Roof2" },
    { "Factory_Roof3.mymodel",       L"Prototype_Component_Model_Factory_Roof3" },
    { "FireStation_Roof1.mymodel",   L"Prototype_Component_Model_FireStation_Roof1" },
    { "FireStation_Roof2.mymodel",   L"Prototype_Component_Model_FireStation_Roof2" },
    { "GasStation_Roof1.mymodel",    L"Prototype_Component_Model_GasStation_Roof1" },
    { "GasStation_Roof2.mymodel",    L"Prototype_Component_Model_GasStation_Roof2" },
    { "Harbor_Roof1.mymodel",        L"Prototype_Component_Model_Harbor_Roof1" },
    { "Harbor_Roof2.mymodel",        L"Prototype_Component_Model_Harbor_Roof2" },
    { "Hospital_Roof1.mymodel",      L"Prototype_Component_Model_Hospital_Roof1" },
    { "Hospital_Roof2.mymodel",      L"Prototype_Component_Model_Hospital_Roof2" },
    { "Hospital_Roof3.mymodel",      L"Prototype_Component_Model_Hospital_Roof3" },
    { "Hotel_Roof1.mymodel",         L"Prototype_Component_Model_Hotel_Roof1" },
    { "PoliceStation_Roof1.mymodel", L"Prototype_Component_Model_PoliceStation_Roof1" },
    { "PoliceStation_Roof2.mymodel", L"Prototype_Component_Model_PoliceStation_Roof2" },
    { "School_Roof1.mymodel",        L"Prototype_Component_Model_School_Roof1" },
    { "School_Roof2.mymodel",        L"Prototype_Component_Model_School_Roof2" },
    { "Temple_Roof1.mymodel",        L"Prototype_Component_Model_Temple_Roof1" },
    { "Uptown_Roof1.mymodel",        L"Prototype_Component_Model_Uptown_Roof1" },
    { "Uptown_Roof2.mymodel",        L"Prototype_Component_Model_Uptown_Roof2" },
    { "Warehouse_Roof1.mymodel",     L"Prototype_Component_Model_Warehouse_Roof1" },
    { "Warehouse_Roof2.mymodel",     L"Prototype_Component_Model_Warehouse_Roof2" },
    { "Warehouse_Roof3.mymodel",     L"Prototype_Component_Model_Warehouse_Roof3" },
    { "Warehouse_Roof4.mymodel",     L"Prototype_Component_Model_Warehouse_Roof4" }
};