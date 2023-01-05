float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap	: DiffuseMap;
Texture2D gNormalMap	: DiffuseMap;
Texture2D gSpecualrMap	: DiffuseMap;
Texture2D gGlossinessMap: DiffuseMap;
float3 gLightDirection	: LightDirection;
float4x4 gWorldMatrix	: WorldMarix;
float4x4 gViewInverseMatrix	: ViewInverseMarix;

bool gUseNormalMap : UseNormalMap;




float gShadingMode = 0;
//ObservedArea = 0
//Diffuse = 1
//Specular = 2
//Combined = 3



SamplerState gSamState
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; //or Mirror, Clamp, Border
	AddressV = Wrap; //or Mirror, Clamp, Border
};


//------------------------------------------------------
//	Input/Output Structs
//------------------------------------------------------
struct VS_INPUT
{
	float3 Position			: POSITION;
	float2 UV				: TEXCOORD;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
};

struct VS_OUTPUT
{
	float4 Position			: SV_POSITION;
	float4 WorldPosition	: WORLD_POS;
	float2 UV				: TEXCOORD;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
};


//------------------------------------------------------
//	Vertex Shader
//------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output		= (VS_OUTPUT)0;
	output.Position			= mul(float4(input.Position, 1.f), gWorldViewProj);
	output.WorldPosition	= float4(input.Position, 1.f);
	output.UV				= input.UV;
	output.Normal			= mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent			= mul(normalize(input.Tangent), (float3x3)gWorldMatrix);

	return output;
}

//------------------------------------------------------
//	Pixel Shader
//------------------------------------------------------
float3 DoShading(float3 viewdir, VS_OUTPUT input);


float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);
	float3 shadingColor = DoShading(viewDirection, input);
	return float4(shadingColor, 1.f);
}

float3 DoShading(float3 viewdir, VS_OUTPUT input)
{
	float3 finalColor = float3( 0.f,0.f,0.f );

	// Light settings
	float3 lightDirection = float3( 0.577f, -0.577f, 0.577f );
	lightDirection = normalize(lightDirection);
	float lightIntensity = 7.f;
	float specularShininess = 25.f;

	if (gUseNormalMap)
	{
		
	}

	// OBSERVED AREA
	float ObservedArea = dot(input.Normal,  -lightDirection);
	ObservedArea = max(ObservedArea, 0.3f);

	float3 observedAreaRGB = float3( ObservedArea ,ObservedArea ,ObservedArea );

	// DIFFUSE
	float4 TextureColor = float4(gDiffuseMap.Sample(gSamState, input.UV));

	// SPECULAR
	float3 reflection = reflect(-lightDirection, input.Normal);
	float cosAlpha = dot(reflection, viewdir);
	cosAlpha = max(0.f, cosAlpha);


	//float specularExp{ specularShininess * m_pGlossinessMap->Sample(v.uv).r };

	//float3 specular{ m_pSpecularMap->Sample(v.uv) * powf(cosAlpha, specularExp) };



	switch (gShadingMode)
	{
	case 0:
	{
		finalColor += observedAreaRGB;
		break;
	}
	//case 1:
	//{
	//	finalColor += lightIntensity * observedAreaRGB * TextureColor / PI;
	//	break;
	//}
	//case 2:
	//{

	//	finalColor += specular;// *observedAreaRGB;
	//	break;
	//}
	//case 3:
	//{
	//	finalColor += lightIntensity * observedAreaRGB * TextureColor / PI;
	//	finalColor += specular;
	//	break;
	//}
	}

	return float4(finalColor, 1.f);
}



//------------------------------------------------------
//	Technique
//------------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
