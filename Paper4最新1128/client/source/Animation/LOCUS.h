#pragma once


//////////////////////////////
//
//		剣先エフェクト		
//
/////////////////////////////
class LOCUS
{
private:
	Vector3	*kako_zahyou1;
	Vector3	*kako_zahyou2;
	unsigned int frame_max;
	unsigned int locus_max;

	iex2DObj* tex;
	float *tu;

public:
	unsigned int separate_num;

	void Update(const Vector3& p1, const Vector3& p2);
	void Render();

	LOCUS(unsigned int max = 5, unsigned int n = 3);
	~LOCUS();
};
