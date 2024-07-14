Texture2D MainTex : register(t0);
Texture2D PrevFrame : register(t1);
int Frame;

SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct Input
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

float4 main(Input input) : SV_TARGET
{
    float4 oldRender = PrevFrame.Sample(MeshTextureSampler, input.position.xy);
    float4 newRender = MainTex.Sample(MeshTextureSampler, input.position.xy);

    float weight = 1.0 / (Frame + 1);
    float4 accumulatedAverage = saturate(oldRender * (1 - weight) + newRender * weight);

    return accumulatedAverage;
}