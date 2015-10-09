#include	"iextreme.h"
//#include<windows.h>
//#include<windef.h>
#include	"Mouse.h"

//=============================================================================================
//		static�����o�ϐ�
POINT Mouse::p;
RECT Mouse::rc;
float Mouse::axis_x;
float Mouse::axis_y;


//=============================================================================================
//		�X		�V
void Mouse::Update()
{
	static const float Max = 0.5f;
	static const float Min = 0.05f;

	//�擾
	GetCursorPos(&p);
	GetWindowRect(iexSystem::Window, &rc);
	//���ʒu�Ɖ��ɂ�钲��
	int ScreenXpos = p.x - rc.left - 8;
	int ScreenYpos = p.y - rc.top - 29;
	////�����I�t�Z�b�g�����K��
	axis_x = ((float)ScreenXpos - 640.0f) / 640.0f;
	axis_y = -(((float)ScreenYpos - 384.0f) / 384.0f);

	// �ő�l����
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// �ŏ��l�Ȃ�0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min * 5) axis_x = 0;
	if (val_y < Min) axis_y = 0;
}
