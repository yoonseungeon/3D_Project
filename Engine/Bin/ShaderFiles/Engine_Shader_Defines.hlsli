SamplerState LinearSampler
{
    Filter = min_mag_mip_linear;

    AddressU = clamp;
    AddressV = clamp;
};


SamplerState PointSampler
{
    Filter = min_mag_mip_point;

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

DepthStencilState DSS_Test_NoWrite
{
    DepthEnable = true;
    DepthWriteMask = Zero;  
    DepthFunc = less_equal;
};

/////////////////////////////////////////////

BlendState BS_Default
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};