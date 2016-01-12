#include	"iextreme.h"
#include	"Animation3D.h"

/*３Dアニメーション*/

Animation3D::Animation3D(char* name, float XSize, float DIST, float MAX_ANIMATION, int SPEED, bool isROOP)
{
	handle = new iex2DObj(name);

	width = XSize;//イラストのXのサイズ
	//Dist=DIST;//アニメの間隔サイズ
	ANIMATION_MAX = (int)MAX_ANIMATION;//アニメーション数
	ANIME_SPEED = SPEED;//速度
	isAction = false;//アニメを実行するか

	count = 0;
	flame = 0;

	Dist = (float)DIST / width;

	isRoop = isROOP;//ループ処理か？
}

Animation3D::~Animation3D()
{
	delete handle;
}

void Animation3D::Action()
{
	isAction = true;//起動
	count = 0;//0にもどす
	flame = 0;
};

void Animation3D::Stop()
{
	isAction = false;//止める
	count = 0;//0にもどす
	flame = 0;
};

void Animation3D::Update()
{
	if (isAction == false)return;//実行されてないなら出てけ！！
	
		flame++;
		if (flame >= ANIME_SPEED)
		{
			flame = 0;
			count++;
		}

		if (count == ANIMATION_MAX)
		{
			count = 0;//0にもどす
			flame = 0;

			if (isRoop==false)//ループじゃなかったら
			{
				isAction = false;
			}
			
		}
	
}

void Animation3D::Render(Vector3 pos, float scale, u32 dwFlags)
{
	if (isAction == false)return;//実行されてないなら出てけ！！

	float tu[2], tv[2];
	tu[0] = (Dist*count);
	tu[1] = (Dist*count) + Dist;
	tv[0] = 0.0f;
	tv[1] = 1.0f;

	Vector3 up = Vector3(0, 0, 1);
	Vector3 right = Vector3(1, 0, 0);

	//Billboard::Draw_VecBoard(up, right, pos, handle, scale, scale, tu, tv, dwFlags);
	//Billboard::Draw_MatBoard(matView, pos, handle, scale, scale, tu, tv, dwFlags);

	// ポジション　と　画像　と　サイズ1.0f　と　0~1
	Billboard::Draw3D(pos, handle, scale, scale, tu, tv, dwFlags);

}