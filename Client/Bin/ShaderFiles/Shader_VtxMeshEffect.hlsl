#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NoiseTexture;

float3 g_Color = { 0.f, 0.f, 0.f };
float g_Alpha = { 0.5f };
bool g_FlipX = { false };

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
    
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
    
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}





float g_ProgressRatio;

PS_OUT PS_MAIN_Q_SPIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord;
    
    // 0.3 ~ 0.8
    float fStartTop = 0.3f;
    vTexcoord.y = vTexcoord.y * 0.5f + fStartTop;
    
    // 0.3 ~ 0.8 -> -0.5 ~ 0
    vTexcoord.y = vTexcoord.y - (g_ProgressRatio * (fStartTop + 0.5f));
    
    if (vTexcoord.y < 0.f)
        discard;
    
    float4 vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    float fBrightness = (vColor.r + vColor.g + vColor.b) / 3.f;
    
    Out.vColor.xyz = float3(0.f, 0.f, 0.f);
    Out.vColor.a = g_Alpha * fBrightness;

    return Out;
}




float g_DiscardRatio;
float3 g_StartColor;
float3 g_EndColor;

PS_OUT PS_MAIN_Q_DRAGON(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord;
    
    if(g_FlipX == true)
        vTexcoord.x = 1.f - vTexcoord.x;
    
    if (g_Alpha >= 1.f)
    {
        float fNoise = (g_NoiseTexture.Sample(LinearSampler, vTexcoord).r * -0.5f) * 0.5f;
        if ((1.f - vTexcoord.x) + fNoise < g_DiscardRatio)
            discard;
    }
            
    float t = saturate(pow(g_DiscardRatio, 3.f));
    float3 vEffectColor = lerp(g_StartColor, g_EndColor, t);
        
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    Out.vColor.xyz *= vEffectColor;
    Out.vColor.a *= g_Alpha;
    
    return Out;
}





PS_OUT PS_MAIN_WIND(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord;
    vTexcoord.x -= g_ProgressRatio;
    
    if (vTexcoord.x < 0.f)
        discard;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    float fBrightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
    
    Out.vColor.rgb *= g_Color;
    Out.vColor.a = g_Alpha * fBrightness;       
    
    return Out;
}





PS_OUT PS_MAIN_CONVER_ALPHA(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fBrightness = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
    
    Out.vColor.xyz = g_Color;
    Out.vColor.a = g_Alpha * fBrightness;

    return Out;
}





PS_OUT PS_MAIN_COLOR_ADD(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor.xyz = g_Color;
    Out.vColor.a = g_Alpha;

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass Blend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AddAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass ConvertBlack
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_Q_SPIN()));
    }

    pass Q_Dragon
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AddAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_Q_DRAGON()));
    }

    pass Wind
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_WIND()));
    }

    pass Convert_AlphaBlend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_CONVER_ALPHA()));
    }

    pass Color_Add
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AddAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_COLOR_ADD()));
    }
}