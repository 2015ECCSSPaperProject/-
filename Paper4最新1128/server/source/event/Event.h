
#pragma once

// �w�肵���^�C�~���O�ŃC�x���g�𔭐�������
class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// �C�x���g���������鎞��<�Q�[���J�n����̌o�ߎ��� (1/1000�b)>
	void Set_time(unsigned int time);
	void Set_time(unsigned int min, unsigned int sec, unsigned int msec);
	// �C�x���g�������Ƀe���b�v�̎�ނ�ύX����
	void Set_telop_id(int in);
	// �X�V : ���Ԃ�������<Start�����s, true��Ԃ�>
	virtual bool Update();

protected:
	unsigned int start_time;

	int telop_id;
};
