#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

float4 g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;      
    // 행렬로 한 번에 받는데 WORLD 1, 2, 3을 row로 넣어야 할지 col로 넣어야 할지 알려줘야 함.
    row_major float4x4 TransformMatrix : WORLD;    
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    // SV_POSITION은 래스터라이저가 위치로 해석하는 값
    // SV_POSITION 레스터라이저 넘어가기 전에 붙여야 한다.
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
    // MS 예제는 GS Input에서도 SV_POSITION 사용
    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-output-stream-stage-getting-started
    float4 vPosition : POSITION;
    // 점으로 만들 도형 크기
    float2 vPSize : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};
    
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;      
  
    float4 vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);   
    
    // 월드까지만 변환해서 넣어준다.(뷰, 투영 x) 셰이더에서 빌보드 하기 위해서
    vPosition = mul(vPosition, g_WorldMatrix);
       
    Out.vPosition = vPosition;
    // 너비, 높이 설정
    Out.vPSize = float2(length(In.TransformMatrix._11_12_13), length(In.TransformMatrix._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

// 지오메트리 셰이더: 정점을 생성하기 위한 셰이더
// 셰이더 내에서 너무 많이 만들면 프레임 드랍 생김

struct GS_IN
{
    float4 vPosition : POSITION;
    // PSIZE 타입이 float이라고 한다. *수정*
    float2 vPSize : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

// 입력
// 지오메트리 셰이더는 도형(primitive 단위)마다 호출된다.
// 프리미티브 타입에 의존. TriangleList이면 점 3개씩 받고, PointList이면 점 1개씩 받는다.

// 출력
// 셰이더 포인터 리턴 없음. 여러개 리턴 안돼서 파라미터로 전달.
// TriangleStream, PointStream, LineStream 이런 거 있음

// TriangleStream에 넣은 정점들이 triangle strip 토폴로지로 해석되어 래스터라이저로 간다.
// https://learn.microsoft.com/en-us/windows/win32/direct3d11/geometry-shader-stage

// maxvertexcount: GS 한 번 호출에서 최대 몇 개 정점을 출력할 수 있는지 선언하는 값
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-geometry-shader

// 6개가 아닌 4개로 사각형 그리는 방법
// https://learn.microsoft.com/en-us/windows/uwp/graphics-concepts/triangle-strips
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    // 빌보드
    // 카메라에서 내 월드 Pos 뺌
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    // 네모의 길이
    // 외적 해결 필요
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    // 뷰 투영 행렬
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime; // 받아온 거 그대로
    
    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    // 마지막은 굳이 없어도 된다고 함. 한 지오메트리 셰이더 내에서 strip을 끊는 거임.
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-so-restartstrip
    OutStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
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
    
    Out.vColor.rgb += 1.f - saturate(In.vLifeTime.x - In.vLifeTime.y);
    Out.vColor.a = saturate(In.vLifeTime.x - In.vLifeTime.y);   
    
    return Out;
}

technique11 DefaultTechnique
{
    // 지오메트리 셰이더 세팅 한 번 설정하면 계속 유지돼서 안 쓴다고 다른 셰이더에서 명시해야 함.
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(CompileShader(gs_5_0, GS_MAIN()));
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }
}