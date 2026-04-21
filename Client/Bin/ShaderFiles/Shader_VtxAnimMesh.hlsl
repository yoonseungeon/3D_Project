#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

Texture2D g_DiffuseTexture;

/* 재질 */
vector g_vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 빛 */
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

float fPower = 50.f;

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
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};
    
/* 픽셀셰이더 : 픽셀의 최종적인 색을 결정해준다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    vector vTextureDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (vTextureDiffuse.a < 0.1f)
    //    discard;
        
    // N: 법선 벡터, L: 빛을 향하는 벡터, R: 빛의 반사 벡터, V: 카메라를 향하는 벡터
    float4 N = normalize(In.vNormal); // 보간된 법선 정규화가 필요하다.
    float4 L = normalize(-g_vLightDir);
    float4 R = normalize(reflect(-L, N)); // 정규화해서 넣어줘야 함.
    vector V = normalize(g_vCamPosition - In.vWorldPos);

    // 실수 + vector -> 실수가 vector가 됨.
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    vector vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * max(0.f, dot(N, L));
    vector vSpecular = g_vLightSpecular * g_vMtrlSpecular * pow(max(0.f, dot(R, V)), fPower);
    
    // saturate: 0 ~ 1 범위 밖 커트(벡터, 실수 다 됨)
    Out.vColor = saturate((vDiffuse + vAmbient) * vTextureDiffuse + vSpecular);
    
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