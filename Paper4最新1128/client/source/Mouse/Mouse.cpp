#include "iextreme.h"
#include "Mouse.h"

//*******************************************************************************************************************
//
//		�����̃N���X�̓^�C�g���Ƃ��Z���N�g���Ŏg���܂��B�v���C���[�̈ړ��ɂ͎g���Ă܂���
//
//*******************************************************************************************************************

//=============================================================================================
//		�}	�E	�X	�X		�V
void Mouse::Update()
{
	static const float Max = 0.6f;
	static const float Min = 0.25f;

	// �O����W�ۑ�
	prev_point = current_point;

	// �擾
	GetCursorPos(&current_point);
	GetWindowRect(iexSystem::Window, &rc);
	// ���ʒu�Ɖ��ɂ�钲��
	pos.x = current_point.x - rc.left - 8;
	pos.y = current_point.y - rc.top - 29;
	////�����I�t�Z�b�g�����K��
	axis_x = ((float)pos.x - (iexSystem::ScreenWidth / 2)) / (iexSystem::ScreenWidth / 2);
	axis_y = -((float)pos.y - (iexSystem::ScreenHeight / 2)) / (iexSystem::ScreenHeight / 2);

	// �ő�l����
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// �ŏ��l�Ȃ�0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min) axis_x = 0;
	if (val_y < Min * 1.5f) axis_y = 0;
}
