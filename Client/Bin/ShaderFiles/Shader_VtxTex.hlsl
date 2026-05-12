#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_Mask;

int g_FlipX = { false }, g_FlipY = { false };
float g_Alpha = { 1.0f };
float3 g_Color;

float g_UVFillX = { 1.f };
float g_UVFillY = { 1.f };

float g_UVFillCenterY = { 0.5f };

float2 g_ClipYRatio;



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
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}

PS_OUT PS_MAIN_ALPHATEST(PS_IN In)
{
    PS_OUT Out;
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_LOBBY_TAB_BTN(PS_IN In)
{
    PS_OUT Out;
            
    if (g_UVFillX < In.vTexcoord.x)
    {
        discard;
    }
    
    if (g_UVFillCenterY < abs(In.vTexcoord.y - 0.5f))
    {
        discard;
    }
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}

PS_OUT PS_MAIN_MASK(PS_IN In)
{
    PS_OUT Out;
    
    //Out.vColor = g_Mask.Sample(LinearSampler, In.vTexcoord);
    //if (Out.vColor.a <= 0.5f)
    //    discard;    
    
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    
    float4 vMask = g_Mask.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= vMask.a;
    
    if (Out.vColor.a <= 0.1f)
        discard;
        
    return Out;
}

PS_OUT PS_MAIN_COLORALPHA(PS_IN In)
{
    PS_OUT Out;
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    Out.vColor.a *= g_Alpha;
    
    Out.vColor.xyz = Out.vColor.xyz * g_Color.xyz;
    
    return Out;
}

PS_OUT PS_MAIN_CHARPROFILE(PS_IN In)
{
    PS_OUT Out;
        
    float4 vMask = g_Mask.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.y = g_ClipYRatio.x + In.vTexcoord.y * (g_ClipYRatio.y - g_ClipYRatio.x);
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= vMask.a;
    
    if (Out.vColor.a <= 0.1f)
        discard;
        
    return Out;
}

PS_OUT PS_MAIN_GAUGE(PS_IN In)
{
    PS_OUT Out;
            
    if (g_UVFillX < In.vTexcoord.x)
    {
        discard;
    }
    
    if (1.f - g_UVFillY > In.vTexcoord.y)
    {
        discard;
    }
    
    if (g_FlipX == 1)
    {
        In.vTexcoord.x = -In.vTexcoord.x + 1.f;
    }
    
    if (g_FlipY == 1)
    {
        In.vTexcoord.y = -In.vTexcoord.y + 1.f;
    }

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    Out.vColor.xyz = Out.vColor.xyz * g_Color.xyz;

    return Out;
}

float g_fMaxHp = { 1000 };
float g_fCurHp = { 500 };
float g_fMpRatio = { 0.5f };

bool g_NoMp = { false };
bool g_DrawSmallLine = { false };

float g_GaugeStartX = 0.03f;
float g_GaugeEndX = 0.97f;

float g_HpStartY = 0.17f;
float g_HpEndY = 0.59f;
float g_MpStartY = 0.67f;
float g_MpEndY = 0.82f;

float3 g_HpColor = { 0.196f, 0.843f, 0.f };
float3 g_MpColor = { 0.196f, 0.843f, 0.f };

PS_OUT PS_MAIN_INGAMEHPBAR(PS_IN In)
{
    PS_OUT Out;
    
    float fHPUVX = (In.vTexcoord.x - g_GaugeStartX) / (g_GaugeEndX - g_GaugeStartX);
    float fHPUVY = (In.vTexcoord.y - g_HpStartY) / (g_HpEndY - g_HpStartY);

    float fHPRatio = saturate(g_fCurHp / g_fMaxHp);
    
    float fTexcoordHp = fHPUVX * g_fMaxHp;
    // 2x2 단위로 실행해서 다음 픽셀과의 HP 차를 알 수 있음. -> 픽셀 간의 HP 차
    float fHPPerPixel = fwidth(fTexcoordHp);
    bool bDrawLine = fTexcoordHp >= 100.f;
    
    float fModLine = fmod(fTexcoordHp, 100.f);
    float fModLongLine = fmod(fTexcoordHp, 1000.f);

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (In.vTexcoord.x >= g_GaugeStartX && In.vTexcoord.x <= g_GaugeEndX &&
        In.vTexcoord.y >= g_HpStartY && In.vTexcoord.y < g_HpEndY)
    {
        if (fHPUVX <= fHPRatio)
        {        
            if (bDrawLine && fModLongLine <= fHPPerPixel)
            {
                Out.vColor.xyz = float3(0.f, 0.f, 0.f);
            }
            else if (g_DrawSmallLine && bDrawLine && fModLine <= fHPPerPixel && fHPUVY <= 0.5f)
            {
                Out.vColor.xyz = float3(0.f, 0.f, 0.f);
            }
            else
            {
                Out.vColor.xyz = g_HpColor;
            }
        }
    }
        
    if (g_NoMp == false)
    {   
        if (In.vTexcoord.x >= g_GaugeStartX && In.vTexcoord.x <= g_GaugeEndX &&
        In.vTexcoord.y >= g_MpStartY && In.vTexcoord.y < g_MpEndY)
        {
            if (fHPUVX <= g_fMpRatio)
            {
                Out.vColor.xyz = g_MpColor;
            }
        }
    }
    
    return Out;
}

float fExtendSize = 1.15f;
int g_MaxLevel = 5;
int g_CurLevel = 3;

PS_OUT PS_MAIN_SKILLICON(PS_IN In)
{
    PS_OUT Out;        
    
    float2 vTexcoord = In.vTexcoord;
    vTexcoord.y *= fExtendSize;
    
    if (vTexcoord.y <=1.f)
    {
        Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        float fFmodX = fmod(vTexcoord.x, 0.2f);
        
        int iCurSlotIndex = floor(vTexcoord.x / 0.2f);
        int iStartSlotIndex = (5 - g_MaxLevel) / 2;
        int iEndSlotIndex = iStartSlotIndex + g_MaxLevel - 1;

        int iCurLevelSlot = iCurSlotIndex - iStartSlotIndex + 1;
        
        Out.vColor = float4(0.f, 0.f, 0.f, 0.5f);
        
        // 레벨 나타낼 수 있는 범위 들어오면(5칸)
        if (fFmodX >= 0.04f && fFmodX <= 0.16f &&
            vTexcoord.y >= 1.07f && vTexcoord.y <= 1.1f)
        {
            // MaxLevel기반으로 칸 수 제한
            if (iCurSlotIndex >= iStartSlotIndex && iCurSlotIndex <= iEndSlotIndex)
            {                
                // 현재 레벨만큼 올라칸 만큼 칠하고
                if (iCurLevelSlot >= 1 && iCurLevelSlot <= g_CurLevel)
                {
                    Out.vColor = float4(0.909f, 0.69f, 0.247f, 1.f);
                }
                else // 나머지는 레벨 안 올라간 부분 어둡게 칠함
                {
                    Out.vColor = float4(0.325f, 0.325f, 0.325f, 1.f);
                }
            }              
        }
    }
    
    return Out;
}





float g_SkillCoolRatio = { 0.3f };

float PI = { 3.141592f };

PS_OUT PS_MAIN_SKILLCOOL(PS_IN In)
{
    PS_OUT Out;

    // 좌표계로
    float2 vTexcoord = In.vTexcoord - float2(0.5f, 0.5f);
    
    // 각도 리턴
    float fRadian = atan2(vTexcoord.x, -vTexcoord.y);
    
    if(fRadian < 0.f)
        fRadian += 2 * PI;
    
    float fAngleRatio = fRadian / (2 * PI);
    
    if (fAngleRatio >= g_SkillCoolRatio)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_Alpha;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_ALPHATEST()));
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }

    pass AlphaBlend_Lobby_Tab_Btn
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_LOBBY_TAB_BTN()));
    }

    pass Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_MASK()));
    }

    pass Color_AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_COLORALPHA()));
    }

    pass CharProfile
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_CHARPROFILE()));
    }

    pass AlphaBlend_Gauge
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_GAUGE()));
    }

    pass InGameHPBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_INGAMEHPBAR()));
    }

    pass SkillIcon
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_SKILLICON()));
    }

    pass SkillCool
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_SKILLCOOL()));
    }
}