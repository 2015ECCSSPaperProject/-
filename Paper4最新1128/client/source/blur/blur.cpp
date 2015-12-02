#include	"iextreme.h"
#include	"../system/system.h"
#include	"blur.h"


/*�u���\�t�B���^�[.cpp*/

/*	�X�^�e�B�b�N	*/
iex2DObj* BlurFilter::screen = nullptr;
float BlurFilter::BluePower = 0;
Surface* BlurFilter::savebackbuffer = nullptr;

BlurFilter::BlurFilter(){

};
BlurFilter::~BlurFilter(){
};

/*������*/
void BlurFilter::Init()
{
	screen = new iex2DObj(1280, 720, IEX2D_RENDERTARGET);
	BluePower = 0.0f;
};

/*���*/
void BlurFilter::CleanUp()
{
	delete screen;
};

void BlurFilter::Update(){

	BluePower -= 0.8f;//�@����X�s�[�h
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

/*����͒��ڂ̃X�N���[���ɂ�����̂Ŏg��Ȃ�*/
/*�u���\���|�������G������*/
void BlurFilter::Start_Copy(){

	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	screen->RenderTarget();
};

void BlurFilter::End_Copy(){
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};

/*�`��*/
void BlurFilter::Render(){

	screen->Render(0, 0, 1280, 720, 0, 0, 1280, 720, shader2D, "RadialBlur");

};

//�@�ǉ�
void BlurFilter::Set_YBlur(float power)
{
	BluePower = power;
}

void BlurFilter::Render_YBlur(){

	screen->Render(0, 0, 1280, 720, 0, 0, 1280, 720, shader2D, "YBlur");

};