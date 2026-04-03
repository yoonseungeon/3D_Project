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
    const wchar_t* TEX_PATH_OVER;
    const wchar_t* TEX_PATH_SELECT;

    const wchar_t* TEX_OVER_TAG;
    const wchar_t* TEX_SELECT_TAG;

    float fScaleRatioX{};
    float fScaleRatioY{};

    float fPosRatioX{};
    float fPosRatioY{};

};

constexpr MAP_META MAPS[]
{
    { L"Map_Alley_Over.png",          L"Map_Alley_Select.png",          L"Prototype_Tex_Alley_Over",          L"Prototype_Tex_Alley_Select"          , 0.298908f, 0.198211f, 0.056958f, 0.397398f},
    { L"Map_Archery_Over.png",        L"Map_Archery_Select.png",        L"Prototype_Tex_Archery_Over",        L"Prototype_Tex_Archery_Select"        , 0.213715f, 0.211653f, -0.257771f,  0.260288f},
    { L"Map_Cemetery_Over.png",       L"Map_Cemetery_Select.png",       L"Prototype_Tex_Cemetery_Over",       L"Prototype_Tex_Cemetery_Select"       , 0.336149f, 0.256868f, 0.147019f, -0.109248f},
    { L"Map_Church_Over.png",         L"Map_Church_Select.png",         L"Prototype_Tex_Church_Over",         L"Prototype_Tex_Church_Select"         , 0.307913f, 0.239270f, 0.067425f, -0.231938f},
    { L"Map_Factory_Over.png",        L"Map_Factory_Select.png",        L"Prototype_Tex_Factory_Over",        L"Prototype_Tex_Factory_Select"        , 0.317894f, 0.279108f,  0.309618f, -0.275197f},
    { L"Map_FireStation_Over.png",    L"Map_FireStation_Select.png",    L"Prototype_Tex_FireStation_Over",    L"Prototype_Tex_FireStation_Select"    , 0.196188f, 0.253446f,  0.036511f,  0.179880f},
    { L"Map_Forest_Over.png",         L"Map_Forest_Select.png",         L"Prototype_Tex_Forest_Over",         L"Prototype_Tex_Forest_Select"         , 0.298908f, 0.261022f, -0.131929f, -0.071366f},
    { L"Map_GasStation_Over.png",     L"Map_GasStation_Select.png",     L"Prototype_Tex_GasStation_Over",     L"Prototype_Tex_GasStation_Select"     , 0.216636f, 0.248069f, -0.110021f,  0.347051f},
    { L"Map_Harbor_Over.png",         L"Map_Harbor_Select.png",         L"Prototype_Tex_Harbor_Over",         L"Prototype_Tex_Harbor_Select"         , 0.277244f, 0.182814f,  0.063774f, -0.407907f},
    { L"Map_Hospital_Over.png",       L"Map_Hospital_Select.png",       L"Prototype_Tex_Hospital_Over",       L"Prototype_Tex_Hospital_Select"       ,0.261179f,  0.246846f,  0.363897f, -0.039838f},
    { L"Map_Hotel_Over.png",          L"Map_Hotel_Select.png",          L"Prototype_Tex_Hotel_Over",          L"Prototype_Tex_Hotel_Select"          ,0.304263f, 0.290351f, -0.316678f, 0.096050f},
    //{ L"Map_Laboratory_Over.png",     L"Map_Laboratory_Select.png",     L"Prototype_Tex_Laboratory_Over",     L"Prototype_Tex_Laboratory_Select"     ,0.f,0.f,0.f,0.f},
    { L"Map_PoliceStation_Over.png",  L"Map_PoliceStation_Select.png",  L"Prototype_Tex_PoliceStation_Over",  L"Prototype_Tex_PoliceStation_Select"  ,0.217123f, 0.229739f, 0.129007f, 0.256378f},
    { L"Map_Pond_Over.png",           L"Map_Pond_Select.png",           L"Prototype_Tex_Pond_Over",           L"Prototype_Tex_Pond_Select"           ,0.221503f, 0.230960f, 0.164059f, 0.037883f},
    { L"Map_SandyBeach_Over.png",     L"Map_SandyBeach_Select.png",     L"Prototype_Tex_SandyBeach_Over",     L"Prototype_Tex_SandyBeach_Select"     ,0.289415f, 0.228028f, -0.350755f, -0.094339f},
    { L"Map_School_Over.png",         L"Map_School_Select.png",         L"Prototype_Tex_School_Over",         L"Prototype_Tex_School_Select"         ,0.241464f, 0.258089f, -0.139231f, 0.165948f},
    { L"Map_Stream_Over.png",         L"Map_Stream_Select.png",         L"Prototype_Tex_Stream_Over",         L"Prototype_Tex_Stream_Select"         ,0.219557f, 0.227539f, 0.289902f, 0.078942f},
    { L"Map_Temple_Over.png",         L"Map_Temple_Select.png",         L"Prototype_Tex_Temple_Over",         L"Prototype_Tex_Temple_Select"         ,0.318137f, 0.270065f, 0.302802f, 0.240247f},
    { L"Map_Uptown_Over.png",         L"Map_Uptown_Select.png",         L"Prototype_Tex_Uptown_Over",         L"Prototype_Tex_Uptown_Select"         ,0.316190f, 0.243670f, -0.176474f, -0.228272f},
    { L"Map_Warehouse_Over.png",      L"Map_Warehouse_Select.png",      L"Prototype_Tex_Warehouse_Over",      L"Prototype_Tex_Warehouse_Select"      ,0.247548f, 0.201632f, -0.047221f, -0.351450f}
};