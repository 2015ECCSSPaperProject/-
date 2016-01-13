#pragma once

//*****************************************************************************************************************************
//
//		�J�����N���X
//
//*****************************************************************************************************************************
#include	"../textLoader\textLoader.h"

class TimeLineCameraPattern
{
public:
	int no;			// �����̃p�^�[���ԍ�
	int data_count;	// �f�[�^�̎����Ă鐔
	struct Data
	{
		int start_frame;		// �^�C�����C����ł̊J�n�t���[��
		int end_frame;			// �^�C�����C����ł̏I���t���[��
		int frame_len;			// 1�p�^�[��������̃t���[����(�o�[�̒���)
		Vector3 *pos_array;		// ���W(�J�n�ƏI��肾���łȂ��A���ԂƂ������̂ŉϒ�)
		Vector3 *target_array;	// �����_(�������ϒ�)
		int num_elements;		// ��L�ϒ��̗v�f��
	}*data;
};

class Camera;

class EffectCamera : public textLoader
{
private:
	enum class APPOINT
	{
		NONE,			// ���W���̂܂�
		DIRECT,			// Pos���ڎw��^
		SOME_ONE,		// �N����Pos
		SOME_ONE_COOD	// �N����Pos�{Vector3
	};

	//------------------------------------------------------
	//	�X�N���v�g�֌W
	//------------------------------------------------------
	int pattern_no;
	int data_cursor;
	char message[256];	// �R�}���h�̖��O�H
	unsigned long ptr;	// 
	int current_frame;	// ���݂̃t���[����
	TimeLineCameraPattern *time_line_data = new TimeLineCameraPattern;

	bool In_event(char *command);
	void Out_event();
	bool Jump(char *label_name);
	void Change_camera_mode();
	void Setting_camera(Camera::MODE mode);
	void Getting_targeter(Vector3 *out);
	void Getting_targeter_coodinate(Vector3 *out);

	//------------------------------------------------------
	//	���䂷��J��������
	//------------------------------------------------------
	Camera *camera;

	// �x�W�G�Ȑ�
	void Bezier(Vector3 *out, Vector3 pos_array[], int num_elements_array, int current_frame, int end_frame);

	void Get_time_line_camera_pos(Vector3 *out_pos, Vector3 *out_target);

public:

	//------------------------------------------------------
	//	�������E���
	//------------------------------------------------------
	EffectCamera();
	~EffectCamera();
	void Initialize(Camera *me, LPSTR filename);

	//------------------------------------------------------
	//	�X�V
	//------------------------------------------------------
	void Update();

	//------------------------------------------------------
	//	�s���Z�b�g
	//------------------------------------------------------
	bool Set_pattern(int pat);
};