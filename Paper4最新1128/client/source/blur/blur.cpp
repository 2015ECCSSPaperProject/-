#include	"iextreme.h"
#include	"../system/system.h"
#include	"blur.h"


/*ブラ―フィルター.cpp*/

/*	スタティック	*/
iex2DObj* BlurFilter::screen = nullptr;
float BlurFilter::BluePower = 0;
Surface* BlurFilter::savebackbuffer = nullptr;

BlurFilter::BlurFilter(){

};
BlurFilter::~BlurFilter(){
};

/*初期化*/
void BlurFilter::Init()
{
	screen = new iex2DObj(1280, 720, IEX2D_RENDERTARGET);
	BluePower = 0.0f;
};

/*解放*/
void BlurFilter::CleanUp()
{
	delete screen;
};

void BlurFilter::Update(){

	BluePower -= 0.8f;//　減るスピード
	if (BluePower <= 0.0f)
	{
		BluePower = 0.0f;
	}
	shader2D->SetValue("BluePower", BluePower);
};

void BlurFilter::Set(float power, float x, float y){

	BluePower = power;

	shader2D->SetValue("CenterX", x);
	shader2D->SetValue("CenterY", y);

};

/*今回は直接のスクリーンにかけるので使わない*/
/*ブラ―を掛けたい絵を挟む*/
void BlurFilter::Start_Copy(){

	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	screen->RenderTarget();
};

void BlurFilter::End_Copy(){
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};

/*描画*/
void BlurFilter::Render(){

	screen->Render(0, 0, 1280, 720, 0, 0, 1280, 720, shader2D, "RadialBlur");

};

//　追加
void BlurFilter::Set_YBlur(float power)
{
	BluePower = power;
}

void BlurFilter::Render_YBlur(){

	screen->Render(0, 0, 1280, 720, 0, 0, 1280, 720, shader2D, "YBlur");

};