SamplerState DefaultSampler
{
    Filter = min_mag_mip_linear;

    AddressU = clamp;
    AddressV = clamp;
};

/////////////////////////////////////////////

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = WireFrame;
};

/////////////////////////////////////////////

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Z_Disable
{
    DepthEnable = false;
    DepthWriteMask = Zero;  
};

/////////////////////////////////////////////

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;
};