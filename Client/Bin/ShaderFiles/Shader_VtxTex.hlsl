#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_Mask;

int g_FlipX = { false }, g_FlipY = { false };
float g_Alpha = { 1.0f };
float3 g_Color;

float g_UVFillX = { 1.f };
float g_UVFillY = { 1.f };

float g_UVFillCenterY = { 0.5f };

float2 g_ClipYRatio;

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
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
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
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_LOBBY_TAB_BTN(PS_IN In)
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

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}

PS_OUT PS_MAIN_MASK(PS_IN In)
{
    PS_OUT Out;
    
    //Out.vColor = g_Mask.Sample(LinearSampler, In.vTexcoord);
    //if (Out.vColor.a <= 0.5f)
    //    discard;    
    
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    
    float4 vMask = g_Mask.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= vMask.a;
    
    if (Out.vColor.a <= 0.1f)
        discard;
        
    return Out;
}

PS_OUT PS_MAIN_COLORALPHA(PS_IN In)
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
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    Out.vColor.a *= g_Alpha;
    
    Out.vColor.xyz = Out.vColor.xyz * g_Color.xyz;
    
    return Out;
}

PS_OUT PS_MAIN_CHARPROFILE(PS_IN In)
{
    PS_OUT Out;
        
    float4 vMask = g_Mask.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.y = g_ClipYRatio.x + In.vTexcoord.y * (g_ClipYRatio.y - g_ClipYRatio.x);
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= vMask.a;
    
    if (Out.vColor.a <= 0.1f)
        discard;
        
    return Out;
}

PS_OUT PS_MAIN_GAUGE(PS_IN In)
{
    PS_OUT Out;
            
    if (g_UVFillX < In.vTexcoord.x)
    {
        discard;
    }
    
    if (1.f - g_UVFillY > In.vTexcoord.y)
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

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    Out.vColor.xyz = Out.vColor.xyz * g_Color.xyz;

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_ALPHATEST()));
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaBlend_Lobby_Tab_Btn
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_LOBBY_TAB_BTN()));
    }

    pass Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_MASK()));
    }

    pass Color_AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_COLORALPHA()));
    }

    pass CharProfile
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_CHARPROFILE()));
    }

    pass AlphaBlend_Gauge
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_GAUGE()));
    }
}