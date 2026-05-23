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

float4 g_vCamPosition;

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

VS_OUT VS_MAIN_BILLBORAD(VS_IN In)
{
    VS_OUT Out;
    
    float4 vCenter = g_WorldMatrix._41_42_43_44;
    
    float fScaleX = length(g_WorldMatrix._11_12_13);
    float fScaleY = length(g_WorldMatrix._21_22_23);
    float fScaleZ = length(g_WorldMatrix._31_32_33);
        
    float4 vViewPos = mul(vCenter, g_ViewMatrix);

    vViewPos.x += In.vPosition.x * fScaleX;
    vViewPos.y += In.vPosition.y * fScaleY;
    vViewPos.z += In.vPosition.z * fScaleZ;
    
    float4 vPosition = mul(vViewPos, g_ProjMatrix);
    
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
        Out.vColor = g_Texture.Sample(PointSampler, vTexcoord);
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





// Ratio가 1이면 전부 흰색. Ratio가 0 이면 투명
// 시간이 갈수록 감소
float g_SkillCoolRatio;
float3 g_SkillCoolColor = { 1.f, 1.f, 1.f };

float PI = { 3.141592f };

PS_OUT PS_MAIN_SKILLCOOL(PS_IN In)
{
    PS_OUT Out;

    // 좌표계로
    float2 vTexcoord = In.vTexcoord - float2(0.5f, 0.5f);
    
    // ratio 1일 때는 전부 출력해야 함.
    // 부등호를 바꾸는게 아니라 좌우를 뒤집어야 함.
    vTexcoord.x = -vTexcoord.x;
    
    // 각도 리턴
    float fRadian = atan2(vTexcoord.x, -vTexcoord.y);
    
    if(fRadian < 0.f)
        fRadian += 2 * PI;
    
    float fAngleRatio = fRadian / (2 * PI);
    
    if (fAngleRatio >= g_SkillCoolRatio)
        discard;
    
    Out.vColor.xyz = g_SkillCoolColor;
    Out.vColor.a = g_Alpha;
    
    return Out;
}





float g_AppearRatio = { 1.f };

PS_OUT PS_MAIN_SHOCKWAVE(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord - float2(0.5f, 0.5f);
    float fDistance = length(vTexcoord);
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float fDark = (1.f - Out.vColor.r) * 0.3f;
    
    if (fDistance - fDark > g_AppearRatio)
        discard;
        
    float3 vColor = { 0.407f, 0.4f, 0.392f };
        
    Out.vColor.a = saturate(Out.vColor.r * 2.f);
    if (Out.vColor.a > 0.5f)
        Out.vColor.a *= 0.7f;
    
    Out.vColor.rgb *= vColor;

    Out.vColor.a *= g_Alpha;
    
    return Out;
}





PS_OUT PS_MAIN_LAVA(PS_IN In)
{
    PS_OUT Out;

    float fMask = g_Texture.Sample(LinearSampler, In.vTexcoord).r;

    float3 vDarkRed = float3(0.12f, 0.01f, 0.0f);
    float3 vRed = float3(0.75f, 0.05f, 0.0f);
    float3 vOrange = float3(1.0f, 0.32f, 0.02f);
    float3 vYellow = float3(1.0f, 0.85f, 0.18f);

 
    float3 vColor = vDarkRed;

    if (fMask < 0.15f)
    {
        vColor = vDarkRed;
    }
    else if (fMask < 0.45f)
    {
        float t = smoothstep(0.15f, 0.45f, fMask);
        vColor = lerp(vDarkRed, vRed, t);
    }
    else if (fMask < 0.75f)
    {
        float t = smoothstep(0.45f, 0.75f, fMask);
        vColor = lerp(vRed, vOrange, t);
    }
    else
    {
        float t = smoothstep(0.75f, 1.0f, fMask);
        vColor = lerp(vOrange, vYellow, t);
    }

    Out.vColor.a = saturate(fMask * 1.5f) * g_Alpha;
    Out.vColor.rgb = vColor;

    return Out;
}





Texture2D g_MaskColor;
Texture2D g_MaskDisappear;

float g_ProgressRatio;

PS_OUT PS_MAIN_DRAGON_R(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord;
    vTexcoord.x = 1.f - vTexcoord.x;
    
    float fVisibleRatio = 0.66f;
    if (vTexcoord.x > fVisibleRatio)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
    Out.vColor.a = Out.vColor.r;
    
    float fColorChangeRatio = 0.28f;

    float fNoiseSample = 1.f - g_MaskColor.Sample(LinearSampler, vTexcoord).r;
    float fNoiseColor = (fNoiseSample - 0.5f) * 0.5f;
    float fNoiseAppear = fNoiseSample * 0.4f;

    // Color
    float3 vStartColor = float3(0.992f, 0.827f, 0.012f);
    float3 vEndColor = float3(1.0f, 0.18f, 0.02f);
    
    if (g_ProgressRatio < fColorChangeRatio)
    {
        Out.vColor.rgb = float3(1.f, 0.f, 0.f);
    }
    else
    {
        float fRatio = (g_ProgressRatio - fColorChangeRatio) / (1.f - fColorChangeRatio);
        if (vTexcoord.x + fNoiseColor > (1.f - fRatio) * fVisibleRatio)
        {
            float t = vTexcoord.x / fVisibleRatio;

            Out.vColor.rgb = lerp(vStartColor, vEndColor, t);
        }
        else
        {
            Out.vColor.rgb = float3(1.f, 0.f, 0.f);
        }
    }
    

    float fNoiseDisAppear = (g_MaskDisappear.Sample(LinearSampler, vTexcoord).r - 0.5f) * 0.2f;
    float fAppearEndRatio = 0.369f;
    float fDisappearEndRatio = 0.9f;

    if (g_ProgressRatio < fAppearEndRatio)
    {
        float fRatio = g_ProgressRatio / fAppearEndRatio;
        if (vTexcoord.x + fNoiseAppear > fRatio)
            discard;
    }
    else
    {
        float fRatio = (g_ProgressRatio - fAppearEndRatio) / (fDisappearEndRatio - fAppearEndRatio);
        if (vTexcoord.x + fNoiseDisAppear > (1.f - fRatio) * fVisibleRatio)
        {
            discard;
        }
    }
    
    return Out;
}

PS_OUT PS_MAIN_DAILIN_W(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);    
    Out.vColor.rgb *= g_Color;
    Out.vColor.a = Out.vColor.r * g_Alpha;
    
    return Out;
}




float g_LeftEndU;
float g_RightStartU;

float g_RenderLeftEnd;
float g_RenderRightStart;

PS_OUT PS_MAIN_REMAPSLICE_U(PS_IN In)
{
    PS_OUT Out;
    
    float2 Texcoord = In.vTexcoord;
    
    if (Texcoord.x <= g_RenderLeftEnd)
    {
        Texcoord.x = (Texcoord.x / g_RenderLeftEnd) * g_LeftEndU;
    }
    else if (Texcoord.x >= g_RenderRightStart)
    {
        Texcoord.x = g_RightStartU + ((Texcoord.x - g_RenderRightStart) / (1.f - g_RenderRightStart)) * (1.f - g_RightStartU);
    }
    else
    {
        Texcoord.x = g_LeftEndU + ((Texcoord.x - g_RenderLeftEnd) / (g_RenderRightStart - g_RenderLeftEnd)) * (g_RightStartU - g_LeftEndU);
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, Texcoord);
    Out.vColor.rgb *= g_Color;
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

    pass ShockWave
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_SHOCKWAVE()));
    }

    pass Lava
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AddAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_LAVA()));
    }

    pass Dragon_R
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AddAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_DRAGON_R()));
    }

    pass Dailin_W
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN_BILLBORAD()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_DAILIN_W()));
    }

    pass RemapSlice_U
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Z_Disable, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN_REMAPSLICE_U()));
    }
}