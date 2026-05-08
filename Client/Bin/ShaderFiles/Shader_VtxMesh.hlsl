#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

float g_Alpha = { 0.5f };
float3 g_Color = { 0.157f, 0.314f, 0.392f };

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
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
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
     // w 나누기 안된 Clip space 좌표
    // Out.vPosition는 w 나누기 -> NDC -> 뷰포트 변환 -> 윈도우 좌표의 픽셀
    // 근데 z만 필요한 것이 아니라 w값도 필요해서 변환안된 좌표(Clip space)가 필요
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    
    return Out;
}





struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};





struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};
    
/* 픽셀셰이더 : 픽셀의 최종적인 색을 결정해준다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vTextureDiffuse.a < 0.01f)
        discard;
        
    Out.vDiffuse = vTextureDiffuse;
    /* -1 ~ 1 -> 0 ~ 1 */
    float3 vNormal = normalize(In.vNormal.xyz);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);

    return Out;
}


PS_OUT PS_MAIN_NORMAL_MAP(PS_IN In)
{
    PS_OUT Out;

    vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vTextureDiffuse.a < 0.01f)
        discard;
        
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    // -1 ~ 1 사이로 변경
    // Linear로 꺼내와서(보간돼서) 길이가 1이 아닐 수 있음
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    // UV의 V방향이 아래라서 -1 곱해서 위로 바꿔줘야 함.
    float3x3 TBNMatrix = float3x3(
                                    normalize(In.vTangent.xyz),
                                    normalize(In.vBinormal.xyz) * -1.f,
                                    normalize(In.vNormal.xyz)
                                 );
    vNormal = normalize(mul(vNormal, TBNMatrix));
  
    
    Out.vDiffuse = vTextureDiffuse;
    /* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);

    return Out;
}





struct PS_OUT_WATER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_WATER PS_MAIN_WATER(PS_IN In)
{
    PS_OUT_WATER Out;

    Out.vColor.rgb = g_Color;
    Out.vColor.a = g_Alpha;
    
    return Out;
}

technique11 DefaultTechnique
{
// 지붕 반드시 Wrap이어야 함.
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }


    pass NormalMap
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_NORMAL_MAP()));
    }

    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Test_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_WATER()));
    }
}