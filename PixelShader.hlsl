cbuffer CBuf
{
	float4 face_colors[6];
};

float4 main(uint tid : SV_PrimitiveID) : SV_TARGET
{
	//return float4(col,1.0f);
	return face_colors[tid / 2];
}