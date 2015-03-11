uniform extern float4x4 mWVP;
uniform extern texture texBox;

sampler sampBox = sampler_state
{
	Texture = <texBox>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
	AddressV  = WRAP;
};

struct OutVS
{
	float4 pos : POSITION0;
	float2 baseCoords: TEXCOORD0;
};

OutVS SpaceboxVS(float3 pos : POSITION0, float2 baseCoords: TEXCOORD0)
{
	OutVS res = (OutVS)0;
	res.pos = mul(float4(pos, 1.0f), mWVP);
	res.baseCoords = baseCoords;
	return res;
}

float4 SpaceboxPS(float2 baseCoords : TEXCOORD0) : COLOR
{
	float3 resultColor=tex2D(sampBox,baseCoords).rgb;
	return float4(resultColor,1.0f);
}

technique MainTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 SpaceboxVS();
        pixelShader  = compile ps_2_0 SpaceboxPS();
    }
}
