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
    LABORATORY,
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
};

constexpr MAP_META MAPS[]
{
    { L"Map_Alley_Over.png",          L"Map_Alley_Select.png",          L"Prototype_Tex_Alley_Over",          L"Prototype_Tex_Alley_Select"          },
    { L"Map_Archery_Over.png",        L"Map_Archery_Select.png",        L"Prototype_Tex_Archery_Over",        L"Prototype_Tex_Archery_Select"        },
    { L"Map_Cemetery_Over.png",       L"Map_Cemetery_Select.png",       L"Prototype_Tex_Cemetery_Over",       L"Prototype_Tex_Cemetery_Select"       },
    { L"Map_Church_Over.png",         L"Map_Church_Select.png",         L"Prototype_Tex_Church_Over",         L"Prototype_Tex_Church_Select"         },
    { L"Map_Factory_Over.png",        L"Map_Factory_Select.png",        L"Prototype_Tex_Factory_Over",        L"Prototype_Tex_Factory_Select"        },
    { L"Map_FireStation_Over.png",    L"Map_FireStation_Select.png",    L"Prototype_Tex_FireStation_Over",    L"Prototype_Tex_FireStation_Select"    },
    { L"Map_Forest_Over.png",         L"Map_Forest_Select.png",         L"Prototype_Tex_Forest_Over",         L"Prototype_Tex_Forest_Select"         },
    { L"Map_GasStation_Over.png",     L"Map_GasStation_Select.png",     L"Prototype_Tex_GasStation_Over",     L"Prototype_Tex_GasStation_Select"     },
    { L"Map_Harbor_Over.png",         L"Map_Harbor_Select.png",         L"Prototype_Tex_Harbor_Over",         L"Prototype_Tex_Harbor_Select"         },
    { L"Map_Hospital_Over.png",       L"Map_Hospital_Select.png",       L"Prototype_Tex_Hospital_Over",       L"Prototype_Tex_Hospital_Select"       },
    { L"Map_Hotel_Over.png",          L"Map_Hotel_Select.png",          L"Prototype_Tex_Hotel_Over",          L"Prototype_Tex_Hotel_Select"          },
    { L"Map_Laboratory_Over.png",     L"Map_Laboratory_Select.png",     L"Prototype_Tex_Laboratory_Over",     L"Prototype_Tex_Laboratory_Select"     },
    { L"Map_PoliceStation_Over.png",  L"Map_PoliceStation_Select.png",  L"Prototype_Tex_PoliceStation_Over",  L"Prototype_Tex_PoliceStation_Select"  },
    { L"Map_Pond_Over.png",           L"Map_Pond_Select.png",           L"Prototype_Tex_Pond_Over",           L"Prototype_Tex_Pond_Select"           },
    { L"Map_SandyBeach_Over.png",     L"Map_SandyBeach_Select.png",     L"Prototype_Tex_SandyBeach_Over",     L"Prototype_Tex_SandyBeach_Select"     },
    { L"Map_School_Over.png",         L"Map_School_Select.png",         L"Prototype_Tex_School_Over",         L"Prototype_Tex_School_Select"         },
    { L"Map_Stream_Over.png",         L"Map_Stream_Select.png",         L"Prototype_Tex_Stream_Over",         L"Prototype_Tex_Stream_Select"         },
    { L"Map_Temple_Over.png",         L"Map_Temple_Select.png",         L"Prototype_Tex_Temple_Over",         L"Prototype_Tex_Temple_Select"         },
    { L"Map_Uptown_Over.png",         L"Map_Uptown_Select.png",         L"Prototype_Tex_Uptown_Over",         L"Prototype_Tex_Uptown_Select"         },
    { L"Map_Warehouse_Over.png",      L"Map_Warehouse_Select.png",      L"Prototype_Tex_Warehouse_Over",      L"Prototype_Tex_Warehouse_Select"      }
};