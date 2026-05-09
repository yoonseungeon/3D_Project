#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;

// 512개 넘는 게임 거의 없다고 한다.
float4x4 g_BoneMatrices[512];

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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
    
    // 로컬 스페이스에서 뼈를 따라 한 번 움직여주고, 월드 행렬을 곱해야 한다.
    float4x4 BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    //법선도 마찬가지이다.
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(vPosition.xyz, 1.f), g_WorldMatrix);
    // w 나누기 안된 Clip space 좌표
    // Out.vPosition는 w 나누기 -> NDC -> 뷰포트 변환 -> 윈도우 좌표의 픽셀
    // 근데 z만 필요한 것이 아니라 w값도 필요해서 변환안된 좌표(Clip space)가 필요
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
    // 타겟 하나 추가
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
     // clip space 좌표라서 w 나누기 해줘야 함.
    // x: z의 범위는 0 ~ far -> w 나누기 하면 0 ~ 1이됨.
    // y: 뷰스페이스 상의 z 범위 near ~ far임
    // UNORM이라 0.0 ~ 1.0으로 저장해야 해서 far로 나눠서 저장
    Out.vDepth = In.vPosition.z;
    
    return Out;
}





struct PS_OUT_SHADOW
{
    float vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;
    
    // NDC의 z 좌표(범위 0 ~ 1)을 넘겨준다.
    Out.vLightDepth = In.vProjPos.z / In.vProjPos.w;
    
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

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_SHADOW()));
    }
}