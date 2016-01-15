#include	"iextreme.h"
#include	"../system/System.h"

#include	"../Player/BasePlayer.h"
#include	"camera.h"
#include	"EffectCamera.h"


//*****************************************************************************************************************************
//
//		�������������J�����N���X
//
//*****************************************************************************************************************************

//=============================================================================================
//		��	��	��
EffectCamera::EffectCamera() :pattern_no(0), ptr(0), current_frame(0), time_line_data(nullptr), data_cursor(-1)
{
}

void EffectCamera::Initialize(Camera *me, LPSTR filename)
{
	// �X�N���v�gIN
	textLoader::Open(filename);

	memset(message, 0, sizeof(message));
	time_line_data = nullptr;
	camera = me;
}
//
//=============================================================================================


//=============================================================================================
//		�J		��
EffectCamera::~EffectCamera()
{

}
//
//=============================================================================================


//=============================================================================================
//		�X		�V
Vector3 operator * (const Vector3 &v, const Matrix &m)
{
	Vector3 ret;
	ret.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]);
	ret.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]);
	ret.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]);
	return ret;
}
void Rot2D(float rad, float *x, float *y)
{
	float ret_x = (*x * cosf(rad)) - (*y * sinf(rad));
	float ret_y = (*x * sinf(rad)) + (*y * cosf(rad));
	*x = ret_x;
	*y = ret_y;
}

void EffectCamera::Update()
{
	//Out_event();
	data_cursor = -1;

	if (time_line_data)
	{
		for (int i = 0; i < time_line_data->data_count; i++)
		{
			if (current_frame >= time_line_data->data[i].start_frame &&
				current_frame <= time_line_data->data[i].end_frame)
			{
				data_cursor = i;
				Vector3 pt[2];
				Get_time_line_camera_pos(&pt[0], &pt[1]);

				// �v���C���[���W�n�ɂ���
				Rot2D(camera->Get_my_player()->Get_angleY(), &pt[0].z, &pt[0].x);
				pt[0] += camera->Get_pos();
				pt[1] += camera->Get_my_player()->Get_pos();
				camera->Set(pt[0], pt[1]);
				break;
			}
		}

		if (++current_frame > time_line_data->data[time_line_data->data_count - 1].end_frame)
		{
			current_frame = 0;
			camera->scriptON = false;
			delete time_line_data, time_line_data = nullptr;
		}
	}
}
//
//=============================================================================================

//=============================================================================================
//		���x���W�����v
bool EffectCamera::Jump(char *label_name)
{
	//�|�C���^��擪��
	ptr = GetPointer();
	SetPointer(0);

	//���x������
	if (Search(label_name))
	{
		return true;	// ���������ꏊ�Ƀ|�C���^���ړ����Ă���̂ŉ������Ȃ�
	}

	SetPointer(ptr);	// ������Ȃ��ꍇ�߂�
	return false;
}
//
//=============================================================================================


//=============================================================================================
//		�s	��	�Z	�b	�g
bool EffectCamera::Set_pattern(int pat)
{
	// �X�N���v�g�G���W��ON
	camera->scriptON = true;
	current_frame = 0;
	data_cursor = 0;

	char str[16];
	sprintf_s(str, "PATTERN:%d", pat);

	bool success = Jump(str);	// ��ׂ����ǂ���(�{��true)

	if (success)
	{
		pattern_no = pat;

		if (time_line_data) delete time_line_data;
		time_line_data = new TimeLineCameraPattern;

		char work[64];
		LoadString(work);
		LPSTR ctx;// strtok_s�̑�3�����Ɏg�p����̂����A�����I�Ɏg�p����̂Ő[���l���Ȃ��炵��
		LPSTR t = strtok_s(work, ":", &ctx);
		t = strtok_s(0, "\t\n", &ctx);
		// ��
		time_line_data->data_count = atoi(t);
		time_line_data->data = new TimeLineCameraPattern::Data[time_line_data->data_count];	// �f�[�^�̌���

		for (int i = 0; i < time_line_data->data_count; i++)
		{
			// �J�n�t���[���ƏI���t���[���ǂݍ���
			time_line_data->data[i].start_frame = LoadInt();
			time_line_data->data[i].end_frame = LoadInt();
			time_line_data->data[i].frame_len = time_line_data->data[i].end_frame - time_line_data->data[i].start_frame;	// �t���[���̒����͎����Ōv�Z

			// ���W�E�����_�v�f���ǂݍ���
			time_line_data->data[i].num_elements = LoadInt();
			// �v�f���̕�����new
			time_line_data->data[i].pos_array = new Vector3[time_line_data->data[i].num_elements];
			time_line_data->data[i].target_array = new Vector3[time_line_data->data[i].num_elements];

			// ���W�E�����_�ǂݍ���
			for (int j = 0; j < time_line_data->data[i].num_elements; j++)
			{
				time_line_data->data[i].pos_array[j].x = LoadFloat();
				time_line_data->data[i].pos_array[j].y = LoadFloat();
				time_line_data->data[i].pos_array[j].z = LoadFloat();
				time_line_data->data[i].target_array[j].x = LoadFloat();
				time_line_data->data[i].target_array[j].y = LoadFloat();
				time_line_data->data[i].target_array[j].z = LoadFloat();
			}

			time_line_data->data[i].fix = (time_line_data->data[i].num_elements == 1);
		}
	}
	return success;
}
//
//=============================================================================================



void EffectCamera::Get_time_line_camera_pos(Vector3 *out_pos, Vector3 *out_target)
{
	if (data_cursor == -1) return;
	if (time_line_data->data[data_cursor].fix)
	{
		*out_pos = time_line_data->data[data_cursor].pos_array[0];
		*out_target = time_line_data->data[data_cursor].target_array[0];
	}

	// �x�W�G�v�Z�֐��Ɋۓ���
	Bezier(
		out_pos,																	// �󂯎M
		time_line_data->data[data_cursor].pos_array,								// �n�_�A���ԁA�I�_�������Ă���W�z��
		time_line_data->data[data_cursor].num_elements,								// �̗v�f��
		current_frame - time_line_data->data[data_cursor].start_frame,				// ���̃t���[��
		time_line_data->data[data_cursor].frame_len									// �I���t���[��
		);

	Bezier(
		out_target,
		time_line_data->data[data_cursor].target_array,								// �n�_�A���ԁA�I�_�������Ă���W�z��
		time_line_data->data[data_cursor].num_elements,								// �̗v�f��
		current_frame - time_line_data->data[data_cursor].start_frame,				// ���̃t���[��
		time_line_data->data[data_cursor].frame_len
		);
}

void EffectCamera::Bezier(Vector3 *out, Vector3 pos_array[], int num_elements_array, int current_frame, int end_frame)
{
	assert(num_elements_array > 0);

	float b = current_frame / (float)end_frame;
	float a = 1 - b;

	/*				//		�Q�l����		//
	//�x�W�F�Ȑ����@�܂��@�@�ŏ��ƒ��ԁ@�@�@�@���Ɂ@�@�@�@���ԂƍŌ�
	pos->x = a*a*a* p1.x + 3 * a*a*b*p2.x + 3 * a*b*b*p2.x + b*b*b*p3.x;
	pos->y = a*a*a* p1.y + 3 * a*a*b*p2.y + 3 * a*b*b*p2.y + b*b*b*p3.y;
	pos->z = a*a*a* p1.z + 3 * a*a*b*p2.z + 3 * a*b*b*p2.z + b*b*b*p3.z;
	*/

	// �n�_
	*out = pos_array[0] * (float)pow(a, num_elements_array);

	// ����
	for (int i = 1; i < num_elements_array - 1; i++)	// -1�Ȃ̂͏I�_���Ȃ�����
	{
		float mult = b;
		for (int j = 1; j < num_elements_array - 1; j++)
		{
			mult *= (j >= i) ? a : b;
		}
		*out += pos_array[i] * (num_elements_array * mult);
	}

	// �I�_
	*out += pos_array[num_elements_array - 1] * (float)pow(b, num_elements_array);
}
//
//*****************************************************************************************************************************




//=============================================================================================
//		�����R�}���h���s
bool EffectCamera::In_event(char *command)
{
	// ��true:�܂������������牽���ǂݍ��܂Ȃ��ł�


	while (EndCheck())
	{
		// �R�}���h�ǂݍ���
		LoadString(command);

		//================================================================
		//	�ҋ@�R�}���h
		if (lstrcmp(command, "WAIT") == 0)
		{
			//wait_timer = LoadInt();	// �ҋ@���ԓǂݍ���
			return true;
		}
		break;
	}
	return false;
}
//
//=============================================================================================

//=============================================================================================
//		�O���R�}���h���s
void EffectCamera::Out_event()
{
	char com[256];
	while (EndCheck())
	{
		//�X�v���N�g���s
		if (In_event(com))return;	// �܂�����������������A��((((�ߎM�)�

		//----------------------------
		// �O���p�R�}���h����
		//----------------------------
		if (strcmp(com, "CHANGE") == 0)
		{
			Change_camera_mode();
			break;
		}
		if (strcmp(com, "END") == 0)
		{
			// �X�N���v�gOFF�ɂ��āA�ǂݍ��ރt�@�C���̈�Ԑ擪�Ƀ|�C���^��߂�(Set_pattern���Ăяo�����܂ŃX�N���v�g��~)
			camera->scriptON = false;
			Jump("START");
			break;
		}
	}

}
//
//=============================================================================================





//=============================================================================================
//		�J�����ݒ�
void EffectCamera::Setting_camera(Camera::MODE mode)
{
	Vector3 pos;
	Vector3 target;

	//================================================================
	//	�ǂݍ��ސ��l�w��
	int appoint[2];
	appoint[0] = LoadInt();
	appoint[1] = LoadInt();


	//================================================================
	//	�ʒu�ǂݍ���
	switch ((APPOINT)appoint[0])
	{
	case APPOINT::NONE:
		pos = camera->Get_pos();
		break;
	case APPOINT::DIRECT:
		pos.x = LoadFloat();
		pos.y = LoadFloat();
		pos.z = LoadFloat();
		break;
	case APPOINT::SOME_ONE:
		Getting_targeter(&pos);
		break;
	case APPOINT::SOME_ONE_COOD:
		Getting_targeter_coodinate(&pos);
		break;
	}

	//================================================================
	//	�^�[�Q�b�g�ǂݍ���
	switch ((APPOINT)appoint[1])
	{
	case APPOINT::NONE:
		target = camera->Get_target();
		break;
	case APPOINT::DIRECT:
		target.x = LoadFloat();
		target.y = LoadFloat();
		target.z = LoadFloat();
		break;
	case APPOINT::SOME_ONE:
		Getting_targeter(&target);
		break;
	case APPOINT::SOME_ONE_COOD:
		Getting_targeter_coodinate(&target);
		break;
	}

	//================================================================
	//	�ʒu�E�����_�̏��Z�b�g�I�I�I
	camera->Change_mode(mode, pos, target);


	//================================================================
	//	�␳���[�h�Ȃ炳��ɓǂݍ���
	if (mode == Camera::MODE::M_SLERP)
	{

		//================================================================
		//	�ǂݍ��ސ��l�w��
		appoint[0] = LoadInt();
		appoint[1] = LoadInt();


		//================================================================
		//	�u�ڕW�v�ʒu�ǂݍ���
		switch ((APPOINT)appoint[0])
		{
		case APPOINT::NONE:
			pos = camera->Get_pos();
			break;
		case APPOINT::DIRECT:
			pos.x = LoadFloat();
			pos.y = LoadFloat();
			pos.z = LoadFloat();
			break;
		case APPOINT::SOME_ONE:
			Getting_targeter(&pos);
			break;
		case APPOINT::SOME_ONE_COOD:
			Getting_targeter_coodinate(&pos);
			break;
		}

		//================================================================
		//	�u�ڕW�v�^�[�Q�b�g�ǂݍ���
		switch ((APPOINT)appoint[1])
		{
		case APPOINT::NONE:
			target = camera->Get_pos();
			break;
		case APPOINT::DIRECT:
			target.x = LoadFloat();
			target.y = LoadFloat();
			target.z = LoadFloat();
			break;
		case APPOINT::SOME_ONE:
			Getting_targeter(&target);
			break;
		case APPOINT::SOME_ONE_COOD:
			Getting_targeter_coodinate(&target);
			break;
		}

		//================================================================
		//	�⊮�����ǂݍ���
		const float p = LoadFloat();


		//================================================================
		//	�ڕW���W�Z�b�g
		camera->Set_slerp(pos, target, p);
	}
}

void EffectCamera::Getting_targeter(Vector3 *out)
{
	char targeter_name[16];
	LoadString(targeter_name);
	if (strcmp(targeter_name, "PLAYER") == 0)
	{
		*out = camera->Get_my_player()->Get_center_pos();
	}
}

void EffectCamera::Getting_targeter_coodinate(Vector3 *out)
{
	char targeter_name[16];
	LoadString(targeter_name);
	if (strcmp(targeter_name, "PLAYER") == 0)
	{
		Vector3 set;
		set = camera->Get_my_player()->Get_pos();
		set.x += LoadFloat();
		set.y += LoadFloat();
		set.z += LoadFloat();
		*out = set;
	}
}



//=============================================================================================
//		�J�������[�h�ύX
void EffectCamera::Change_camera_mode()
{
	// ���[�h���ǂݍ���
	char str[16];
	LoadString(str);

	static const char *n_list[4] =
	{
		"FIX", "PAN", "SLERP", "TPS"
	};

	static const Camera::MODE m_list[4] =
	{
		Camera::MODE::M_FIX, Camera::MODE::M_PAN, Camera::MODE::M_SLERP, Camera::MODE::M_TPS
	};

	for (int i = 0; i < 4; i++)
	{
		//	��������
		if (strcmp(str, n_list[i]) == 0)
		{
			Setting_camera(m_list[i]);
			return;
		}
	}

	MessageBox(0, "�e�L�X�g�̃J�������[�h�����Ԉ���Ă����", null, MB_OK);
}
//
//=============================================================================================

