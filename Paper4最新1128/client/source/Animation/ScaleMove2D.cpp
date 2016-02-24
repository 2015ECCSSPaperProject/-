#include	"iextreme.h"
#include	"ScaleMove2D.h"

/*	スケールを利用したアニメーション　*/

ScaleMove2D::ScaleMove2D(char* name, float DefaultScale, float FluctuationRange, float AddScale)
{
	handle = new iex2DObj(name);
	default_scale = DefaultScale;
	fluctuation_range = FluctuationRange;
	add_scale = AddScale;
	add_flag = ADD_FLAG::NONE;//アニメを実行するスイッチ
}

ScaleMove2D::~ScaleMove2D()
{
	delete handle;
}

void ScaleMove2D::Action()
{
	add_flag = ADD_FLAG::INCREASE;
	scale = 1.0f;//初期スケール
};

void ScaleMove2D::Stop()
{
	add_flag = ADD_FLAG::NONE;
	scale = 1.0f;//初期スケール
}

void ScaleMove2D::Update()
{
	switch (add_flag)
	{
	case ADD_FLAG::INCREASE:
		if ((scale += add_scale) >= default_scale + fluctuation_range)
		{
			add_flag = ADD_FLAG::DECREASE;
		}
		break;
	case ADD_FLAG::DECREASE:
		if ((scale -= add_scale) <= default_scale - fluctuation_range)
		{
			add_flag = ADD_FLAG::INCREASE;
		}
		break;
	}
}

void ScaleMove2D::Render(int dstX, int dstY, u32 dwFlag)
{
	// 変更
	handle->SetScale(scale);
	handle->Render(dstX, dstY, dwFlag);
};