float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

SamplerState DefaultSampler
{
    Filter = min_mag_mip_linear;

    AddressU = wrap;
    AddressV = wrap;
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
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
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