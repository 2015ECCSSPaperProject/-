#define PL_MAX 8


float3	PL_pos[PL_MAX];
float3	PL_color[PL_MAX];
float	PL_range[PL_MAX];
float	PL_power[PL_MAX];

float3 PointLightRate(float3 Pos, float3 Normal){
	float3 col = (float3)0;
	float3 Sumcol = (float3)0;

	for (int i = 0; i < PL_MAX; i++){
		//	減衰量
		float rate = pow(max(0, 1.0 - (length(PL_pos[i] - Pos)
			/ PL_range[i])), 10);//←数値を変え絞る　やり過ぎると処理が重くなる
		col = PL_color[i] * rate;
		col *= PL_power[i];
		//法線を考慮
		float3 pVec;
		pVec = ((Pos - PL_pos[i]) / PL_range[i]);
		pVec = normalize(pVec);
		col *= max(0, dot(-pVec, Normal));

		Sumcol += col;//加算


	}


	return Sumcol;
}