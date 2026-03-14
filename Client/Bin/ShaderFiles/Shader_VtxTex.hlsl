float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
    float3 vPosition;
    float2 vTexcoord;
};

struct VS_OUT
{
    float4 vPosition;
    float2 vTexcoord;
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

/* 픽셀셰이더: 픽셀의 최종적인 색을 결정하는 과정 */
PS_MAIN()
{


}