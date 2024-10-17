struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct ColorInfo
{
    float4 multiplier;
};

cbuffer colorInfo : register(b3, space0)
{
    ColorInfo cim;
}

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color * cim.multiplier;

    return result;
}
