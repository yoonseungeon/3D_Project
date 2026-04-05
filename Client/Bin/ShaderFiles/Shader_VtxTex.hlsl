float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
int g_FlipX = { false }, g_FlipY = { false };
float g_Alpha = { 1 };

float g_UVFillX = { 1.f };
float g_UVFillCenterY = { 0.5f };

SamplerState DefaultSampler
{
    Filter = min_mag_mip_linear;

    AddressU = clamp;
    AddressV = clamp;
};

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

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
    
/* 정점셰이더: 정점 데이터의 변환 과정을 수행한다. */
// 변환 과정: 월드, 뷰 투영 변환
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};
    
/* 픽셀셰이더: 픽셀의 최종적인 색을 결정하는 과정 */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}

PS_OUT PS_MAIN_ALPHATEST(PS_IN In)
{
    PS_OUT Out;
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_UIGAUGE(PS_IN In)
{
    PS_OUT Out;
            
    if (g_UVFillX < In.vTexcoord.x)
    {
        discard;
    }
    
    if (g_UVFillCenterY < abs(In.vTexcoord.y - 0.5f))
    {
        discard;
    }
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }

    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        //SetDepthStencilState(DSS_None, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaTest
    {
        //SetDepthStencilState(DSS_None, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_ALPHATEST()));
    }

    pass AlphaBlend
    {
        //SetDepthStencilState(DSS_None, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaBlend_Gauge
    {
        //SetDepthStencilState(DSS_None, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_UIGAUGE()));
    }
}