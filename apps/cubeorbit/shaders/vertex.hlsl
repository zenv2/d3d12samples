struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


cbuffer constantBuffer : register(b0)
{
    float4x4 wvpMat;
}

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = mul(position, wvpMat);
    result.color = color;

    return result;
}
