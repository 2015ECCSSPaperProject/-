#pragma once

/*�u���\�t�B���^�[.h*/

class BlurFilter
{
private:
	static iex2DObj* screen;
	static float BluePower;
	static Surface* savebackbuffer;// �ۑ��p�T�[�t�F�C�X
	BlurFilter();
	~BlurFilter();
public:
	static void Init();/*������*/
	static void CleanUp();/*���*/
	
	static void Update();/*�X�V*/
	static void Set(float power, float x, float y);/*�u���\���ʂ𔭓�*/
	
	static void Start_Copy();
	static void End_Copy();

	static void Render();

	// �ǉ�
	static void Set_YBlur(float power);/*�u���\���ʂ𔭓�*/
	static void Render_YBlur();
};