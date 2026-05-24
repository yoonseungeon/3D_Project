#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4x4 g_ShadowLightViewMatrix, g_ShadowLightProjMatrix;
Texture2D g_LightDepthTexture;

float4x4 g_ViewMatrixInverse, g_ProjMatrixInverse;
Texture2D g_Texture;
Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_DepthTexture;
Texture2D g_SpecularTexture;

Texture2D g_VisionMaskTexture;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlDiffuse = 1.f;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

float fPower = 50.f;

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




struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer : SV_TARGET0;
};
    
// Debug
PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}


// + 픽셀 별로 다르게 스페큘러 표현할려면 스페큘러 타겟을 만들어야 함.

// Shade, Specular
struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};
    
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
    // vDepthDesc.y: 뷰스페이스 상의 z 범위 near ~ far임 -> 변경 -> 안 쓰고도 가능
    //float fViewZ = vDepthDesc.y * 500.f;
        
    /* 투영공간상의 위치 */ // (NDC)
    // 텍스처 (0, 0) ~ (1, 1)에서 (-1, 1) ~ (1, -1)로 변경
    // z(0 ~ 1)만 잘 구해주면 NDC를 구할 수 있다.
    // w는 1 w 나누기 했기 때문에
    // 이후 원근 투영의 역행렬, 뷰스페이스 행렬의 역행렬을 곱하면 월드를 얻을 수 있다.
    vector vNDCPos;
    vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
    vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
    // x는 w 나누기까지 한 z(범위 0 ~ 1)
    vNDCPos.z = vDepthDesc.x;
    vNDCPos.w = 1.f;
    
    /* 뷰스페이스 상의 위치 */
    // XMVector3TransformCoord는 w 나누기 알아서 해준다.
    // 뷰스페이스에서 투영 행렬을 곱하면 뷰스페이스에서 clip space를 거친 뒤 NDC로 바뀐다.(결과 NDC)
    // 반대로 NDC에서 투영 행렬의 역행렬을 곱할 때 w 나누기가 돼서 바로 NDC에서 뷰스페이스가 된다.(e.g. 피킹)    
    // 근데 mul은 단순히 행렬의 곱셈만한다. 그래서 NDC에서 뷰스페이스로 갈 때 clip space로 직접 만들어주고 역행렬을 곱해야 한다.
    // -> 변경 -> mul을 XMVector3TransformCoord과 동일하게 처리하면 NDC에서 바로 뷰스페이스를 얻는다.
    vector vViewPos;
    vViewPos = mul(vNDCPos, g_ProjMatrixInverse);
    vViewPos /= vViewPos.w;
    
    vector vWorldPos = mul(vViewPos, g_ViewMatrixInverse);
            
    // N: 법선 벡터, L: 빛을 향하는 벡터, R: 빛의 반사 벡터, V: 카메라를 향하는 벡터
    float4 N = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f)); // 0.0 ~ 1.0 -> -1.0 ~ 1.0
    float4 L = normalize(-g_vLightDir);               
    float4 R = normalize(reflect(-L, N)); // 정규화해서 넣어줘야 함.
    vector V = normalize(g_vCamPosition - vWorldPos);
    
    // 실수 + vector -> 실수가 vector가 됨.
    vector vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * max(0.f, dot(N, L));
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    
    // 밝기 값 저장
    Out.vShade = saturate(vDiffuse + vAmbient);
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(0.f, dot(R, V)), fPower);
        
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    
       // vDepthDesc.y: 뷰스페이스 상의 z 범위 near ~ far임 -> 변경 -> 안 쓰고도 가능
    //float fViewZ = vDepthDesc.y * 500.f;
        
    /* 투영공간상의 위치 */ // (NDC)
    // 텍스처 (0, 0) ~ (1, 1)에서 (-1, 1) ~ (1, -1)로 변경
    // z(0 ~ 1)만 잘 구해주면 NDC를 구할 수 있다.
    // w는 1 w 나누기 했기 때문에
    // 이후 원근 투영의 역행렬, 뷰스페이스 행렬의 역행렬을 곱하면 월드를 얻을 수 있다.
    vector vNDCPos;
    vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
    vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
    // x는 w 나누기까지 한 z(범위 0 ~ 1)
    vNDCPos.z = vDepthDesc.x;
    vNDCPos.w = 1.f;
    
    /* 뷰스페이스 상의 위치 */
    // XMVector3TransformCoord는 w 나누기 알아서 해준다.
    // 뷰스페이스에서 투영 행렬을 곱하면 뷰스페이스에서 clip space를 거친 뒤 NDC로 바뀐다.(결과 NDC)
    // 반대로 NDC에서 투영 행렬의 역행렬을 곱할 때 w 나누기가 돼서 바로 NDC에서 뷰스페이스가 된다.(e.g. 피킹)    
    // 근데 mul은 단순히 행렬의 곱셈만한다. 그래서 NDC에서 뷰스페이스로 갈 때 clip space로 직접 만들어주고 역행렬을 곱해야 한다.
    // -> 변경 -> mul을 XMVector3TransformCoord과 동일하게 처리하면 NDC에서 바로 뷰스페이스를 얻는다.
    vector vViewPos;
    vViewPos = mul(vNDCPos, g_ProjMatrixInverse);
    vViewPos /= vViewPos.w;
    
    vector vWorldPos = mul(vViewPos, g_ViewMatrixInverse);
    
    
    // 점 광원은 빛의 방향을 구해줘야 함.
    vector vLightDir = vWorldPos - g_vLightPos;    
    float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

    
    // N: 법선 벡터, L: 빛을 향하는 벡터, R: 빛의 반사 벡터, V: 카메라를 향하는 벡터
    float4 N = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f)); // 0.0 ~ 1.0 -> -1.0 ~ 1.0
    float4 L = normalize(-vLightDir);
    float4 R = normalize(reflect(-L, N)); // 정규화해서 넣어줘야 함.
    vector V = normalize(g_vCamPosition - vWorldPos);
    
    // 실수 + vector -> 실수가 vector가 됨.
    vector vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * max(0.f, dot(N, L)) * fAtt;
    vector vAmbient = g_vLightAmbient * g_vMtrlAmbient;
    
    // 밝기 값 저장
    Out.vShade = saturate(vDiffuse + vAmbient);
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(0.f, dot(R, V)), fPower) * fAtt;
        
    return Out;
}



// 백버퍼
PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    // Diffuse와 Shade를 곱해서 백버퍼에 그림

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    // 스페큘러 받아와서 던짐
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vBackBuffer = vDiffuse * vShade + vSpecular;
    
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNDCPos;
    vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
    vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
    vNDCPos.z = vDepthDesc.x;
    vNDCPos.w = 1.f;
    
    vector vViewPos;
    vViewPos = mul(vNDCPos, g_ProjMatrixInverse);
    vViewPos /= vViewPos.w;
    
    // 픽셀의 월드 좌표
    vector vWorldPos = mul(vViewPos, g_ViewMatrixInverse);
    
    vector vLightViewPos = mul(vWorldPos, g_ShadowLightViewMatrix);
    vector vLightNDC = mul(vLightViewPos, g_ShadowLightProjMatrix);
    // 직교 투영 w 어차피 1이지만 정확하게
    vLightNDC = vLightNDC / vLightNDC.w;
    
    /* -1 ~ 1 => 0 ~ 1 */
    /* 1 ~ -1 => 0 ~ 1 */
    // 텍스처 좌표로 변환해서 물체의 월드에 맞는 픽셀을 가지고 옴    
    float2 vTexcoord;
    vTexcoord.x = vLightNDC.x * 0.5f + 0.5f;
    vTexcoord.y = vLightNDC.y * -0.5f + 0.5f;

    vector vLightDepthDesc = g_LightDepthTexture.Sample(ClampSampler, vTexcoord);
    
    // NDC의 z 좌표(범위 0 ~ 1)를 받아왔다.
    float fShadowObjectNDCDepth = vLightDepthDesc.x;
        
    // if (픽셀의 광원 기준 깊이 > 이미 광원기준으로 기록되어있던 깊이보다.)
    if (vLightNDC.z - 0.0001f > fShadowObjectNDCDepth)
        Out.vBackBuffer = Out.vBackBuffer * 0.8f; // 어둡게
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_DEBUG()));
    } 

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_DIRECTIONAL()));
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_POINT()));
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_COMBINED()));
    }
}