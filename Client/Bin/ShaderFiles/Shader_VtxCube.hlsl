#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
TextureCube g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};
    
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;    
    
    float4      vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};
    
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();        
    }
}