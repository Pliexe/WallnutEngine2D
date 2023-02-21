Texture2D<float4> tex : register(t25);

cbuffer Cbuf
{
    matrix worldViewPortProjection;
    matrix worldProjection;
    float4 color;
    float entityId;
};

struct VSIn
{
    float3 position : Position;
    float2 texCoord : TexCoord;
    float3 normal : Normal;
};

struct VSOut
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float entityId : EntityId;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float4 position : SV_Position;
};

VSOut main(VSIn vin)
{
    VSOut vout;
    vout.position = mul(float4(vin.position, 1.0f), worldViewPortProjection);
    vout.texCoord =  vin.texCoord;
    vout.color = color;
    vout.entityId = entityId;
    vout.normal = mul(float4(vin.normal, 1.0f), worldProjection);
    vout.worldPos = mul(float4(vin.position, 1.0f), worldProjection);
    return vout;
}