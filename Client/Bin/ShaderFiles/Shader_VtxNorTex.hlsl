#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};
    
/* 정점셰이더 : 정점 데이터의 변환 과정을 수행한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    float4 vViewPos = mul(vWorldPos, g_ViewMatrix);
    float4 vProjPos = mul(vViewPos, g_ProjMatrix);
    
    Out.vPosition = vProjPos;
    // 빛의 방향 벡터(월드 좌표)와 법선 벡터 연산함(로컬 좌표) -> 법선 벡터를 월드로 맞춰야 함.
    // 빛을 로컬로 가져오는 건 역행렬 필요.(빛 위치, 방향, 카메라 위치 다 로컬로 가져와야 함.)
    // 비균일 스케일일 경우 정확 x
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = vWorldPos;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float vDepth : SV_TARGET2;
};
    
/* 픽셀셰이더 : 픽셀의 최종적인 색을 결정해준다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vTextureDiffuse.a < 0.1f)
        discard;
                 
    Out.vDiffuse = vTextureDiffuse;
    /* -1 ~ 1 -> 0 ~ 1 */
    float3 vNormal = normalize(In.vNormal.xyz);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = In.vProjPos.z / In.vProjPos.w;

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
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }
}