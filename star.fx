uniform extern float4x4 mWVP;
uniform extern float4 colorMain;
uniform extern float4 colorHighlight;
uniform extern texture texCloudsCurrent;
uniform extern texture texCloudsNext;
uniform extern texture texCrownCurrent;
uniform extern texture texCrownNext;
uniform extern texture texGradient;
uniform extern float progressClouds;
uniform extern float progressCrown;

static const float starRadius=0.398f;

sampler sampCloudsCurrent = sampler_state
{
	Texture = <texCloudsCurrent>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
	AddressV  = WRAP;
};
sampler sampCloudsNext = sampler_state
{
	Texture = <texCloudsNext>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
	AddressV  = WRAP;
};
sampler sampCrownCurrent = sampler_state
{
	Texture = <texCrownCurrent>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
	AddressV  = WRAP;
};
sampler sampCrownNext = sampler_state
{
	Texture = <texCrownNext>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
	AddressV  = WRAP;
};
sampler sampGradient = sampler_state
{
    Texture = <texGradient>;
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

OutVS StarVS(float3 pos : POSITION0, float2 baseCoords: TEXCOORD0)
{
	OutVS res = (OutVS)0;
	res.pos = mul(float4(pos, 1.0f), mWVP);
	res.baseCoords = baseCoords;
	return res;
}

float4 StarPS(float2 baseCoords : TEXCOORD0) : COLOR
{
	float2 offsetCoordsSqr=pow(baseCoords-float2(0.5f,0.5f),2);
	float radiusVector=sqrt(offsetCoordsSqr.x+offsetCoordsSqr.y);
	float3 resultColor=(float3)0;
    float resultAlpha=1.0f;
    float4 gradientColor=tex2D(sampGradient,baseCoords).rgba;
	if(radiusVector<starRadius)
	{
		//apply fading between 2 clouds textures
		resultColor=lerp(tex2D(sampCloudsCurrent,baseCoords).rgb,tex2D(sampCloudsNext,baseCoords).rgb,progressClouds);
		//apply temperature color (screen)
		resultColor=1.0f-(1.0f-resultColor)*(1.0f-colorMain.rgb);
		//apply gradient by alpha blending
		resultColor=lerp(resultColor,gradientColor.rgb,gradientColor.a);
	}
	else
	{
        float3 crownColor=lerp(tex2D(sampCrownCurrent,baseCoords).rgb,tex2D(sampCrownNext,baseCoords).rgb,progressCrown)*gradientColor.a;
        resultColor=1.0f-(1.0f-gradientColor.rgb)*(1.0f-crownColor*colorHighlight.rgb);
        resultAlpha=gradientColor.a;
	}
	return float4(resultColor,resultAlpha);
}

technique MainTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 StarVS();
        pixelShader  = compile ps_2_0 StarPS();
    }
}
