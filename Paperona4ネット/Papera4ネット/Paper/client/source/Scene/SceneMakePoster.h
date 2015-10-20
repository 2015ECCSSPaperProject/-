#pragma once

#include	"../../../share_data/Enum_public.h"

class SceneMakePoster : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneMakePoster();
	//����
	void Update();
	void CatchStamp();//�@�X�^���v���߂��ɂ���ΑI��
	//�`��
	void Render();
	void PosterRender();
private:

	iex2DObj* posterScreen;
	Surface* backbuffer;// �T�[�t�F�C�X�@

	//�@�|�X�^�[�̃T�C�Y�ƃt���[��
	iex2DObj* posterSizePic;
	iex2DObj* posterFramePic;
	float posterScreen_W;//�@�ꏊX
	float posterScreen_H;//�@�ꏊY

	iexView* view;
	iex2DObj* backPic;

	//�@��
	iex2DObj* cursorPic;
	bool isPush;
	bool isTap;

private:

	/******************************/
	// �X�^���v�̎��
	/******************************/


	//�@���MAX
	//static const int KIND_MAX = 3;
	//�@��̎�ނ̃X�^���v����MAX
	//static const int NO_MAX = 10;

	//�@�X�^���v�̉摜��2�����z��������
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];

	// ��ޕ��̃t�@�C��
	Vector3 FileSelectPos[KIND_MAX];
	iex2DObj* FilePic[KIND_MAX];
	void FileChange();//�I���������ނ��`�F���W 

	/******************************/
	// �X�^���v�蒠
	/******************************/
	struct STAMP_DATA
	{
		/*�|�W�V����*/
		float x, y;

		/*���̉摜�̃T�C�Y�⍡���܂�Ă邩*/
		int size;

	};
	STAMP_DATA StampData[KIND_MAX][NO_MAX];

	int SelectKind;//�@���I�����Ă��ނ̔ԍ��@(�y�[�W)


	/******************************/
	// ���C���[
	/******************************/
	enum { NO_LAYER = -1 };
	//�@���C���[MAX
	//static const int LAYER_MAX = 10;
	//  ���C���[
	struct LAYER_DATA
	{
		/*��ނƃi���o�[*/
		int kind;
		int num;

		float x, y;//	�ꏊ
		int size;//		�T�C�Y
		bool isHold;//	�͂�ł邩
	};

	LAYER_DATA layer_data[LAYER_MAX];


private: //�֐�

	// �X�^���v�̊G�̏�����
	void StampInit();

	// ���C���[�̏�����
	void LayerInit();

	//�@�}�E�X�ʒu�擾
	Vector3 GetMousePos();


	// ���C���[��ǉ�
	void AddLayer(int kind, int no, STAMP_DATA stamp);

	// �w�i�����C���[�ɒǉ�
	void AddBackLayer(int kind, int no, STAMP_DATA stamp);

	// ���C���[������
	void DeleteLayer();

	// ���C���[�𓮂���
	void MoveLayer();

	// ���C���[��͂�
	void CatchLayer();

	// ���C���[����ԏ�ɂ���֐�
	void ChangeTopLayer(int layer_num);

	// ���C���[����ԉ��ɂ���֐�
	void ChangeBottomLayer(int layer_num);

	// ���C���[��ǂݍ���
	void LoadLayer();

	// ���C���[�������o��
	void SaveLayer();

};
