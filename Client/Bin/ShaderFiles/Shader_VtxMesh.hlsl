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

SamplerState DefaultSampler
{
    Filter = min_mag_mip_linear;

    AddressU = wrap;
    AddressV = wrap;
};

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
};
    
/* 정점셰이더 : 정점 데이터의 변환 과정을 수행한다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    // 빛의 방향 벡터(월드 좌표)와 법선 벡터 연산함(로컬 좌표) -> 법선 벡터를 월드로 맞춰야 함.
    // 빛을 로컬로 가져오는 건 역행렬 필요.(빛 위치, 방향, 카메라 위치 다 로컬로 가져와야 함.)
    // 비균일 스케일일 경우 정확 x
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
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

    vector vTextureDiffuse = /*g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord)*/ 1.f;
    if (vTextureDiffuse.a < 0.1f)
        discard;
        
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
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }
}