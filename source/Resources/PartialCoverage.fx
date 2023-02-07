Texture2D gDiffuseMap	: DiffuseMap;

float4x4 gWorldViewProj : WorldViewProjection;


SamplerState gSamState
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; //or Mirror, Clamp, Border
	AddressV = Wrap; //or Mirror, Clamp, Border
};

RasterizerState gRasterizerState
{
	CullMode = front;
	FrontCounterClockwise = false; // default
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	//others are redundant because
	//	StencilEnable is FALSE
	//(for demo purposes only)
	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F; 

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	backFaceStencilFail = keep;
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
	float2 UV				: TEXCOORD;
};


//------------------------------------------------------
//	Vertex Shader
//------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.UV = input.UV;

	return output;
}


//------------------------------------------------------
//	Pixel Shader
//------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(gSamState, input.UV);
}

//------------------------------------------------------
//	Technique
//------------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}




