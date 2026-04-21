#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
    
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;    
    
    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
        
    vPosition = mul(vPosition, g_WorldMatrix);
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
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a < 0.4f)
        discard;      
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }
}