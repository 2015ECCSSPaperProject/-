#include	"iextreme.h"

#include	"../../../share_data/Enum_public.h"

#include	"../Net/Socket.h"
#include	"../system/Framework.h"
#include	"../system/Thread.h"
#include	"../../IEX/OKB.h"

#include	"SceneMakePoster.h"
#include	"SceneSelect.h"

#include	"../Net/Socket.h"

/*�����ȂǓ����蔻��p*/
//****************************************************************
//		���W�ƍ��W�̋�����Ԃ�
//****************************************************************
inline float Length(Vector3 PosA, Vector3 PosB)
{
	//  PosA��PosB�Ƃ̃x�N�g�����v�Z
	//�i����͋��������߂邾���Ȃ̂ŕ����͊֌W�Ȃ��j
	Vector3 Vec = PosA - PosB;

	//  �x�N�g���̒������O�����̒藝���狁�߁A���߂��x�N�g���̒�����Ԃ�
	return sqrtf(Vec.x*Vec.x + Vec.y*Vec.y + Vec.z*Vec.z);
}


bool SceneMakePoster::Initialize()
{
	

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	view = new iexView();
	view->Set(Vector3(0, 0, -10), Vector3(0, 0, 0));

	//�@�J�[�\��
	cursorPic = new iex2DObj("DATA/makePoster/cursor4.png");
	isPush = false;
	isTap = false;
	ShowCursor(FALSE);//�@�}�E�X��`�悵�Ȃ�

	// �o�b�N
	backPic = new iex2DObj("DATA/makePoster/back.png");

	// �|�X�^�[�̏ꏊ
	posterSizePic = new iex2DObj("DATA/makePoster/PosterSize.png");
	posterFramePic = new iex2DObj("DATA/makePoster/Posterframe.png");
	posterScreen_W = 64.0f;
	posterScreen_H = 128.0f;

	// �t�@�C��
	char fileName[128];
	for (int i = 0; i < KIND_MAX; i++)
	{
		
		sprintf(fileName, "DATA/makePoster/file/file_%d.png", i);
		FilePic[i] = new iex2DObj(fileName);

		// �t�@�C���`�F���W�̖ڈ�
		FileSelectPos[i] = Vector3(750, 120 + ((float)i * 196), 0);
	}
	

	//�X�^���v�̏�����
	StampInit();

	//���C���[�̏�����
	LayerInit();

	posterScreen = new iex2DObj(512, 512, IEX2D_RENDERTARGET);
	iexSystem::Device->GetRenderTarget(0, &backbuffer);//���`���
	
	return true;
}


// �X�^���v�̏�����
void SceneMakePoster::StampInit()
{
	//�@�C���X�g���������X�^���v�蒠������
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			StampPicture[i][no] = NULL;
			//�@�X�^���v�̃f�[�^���S���O��
			ZeroMemory(&StampData[i][no], sizeof(STAMP_DATA));
		}
	}

	SelectKind = 0;// ���I�����Ă���


	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::OBJECT][0] = new iex2DObj("DATA/makePoster/stamp/yen.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::OBJECT][0].x = 900;
	StampData[KIND_NAME::OBJECT][0].y = 100;
	StampData[KIND_NAME::OBJECT][0].size = 50;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::OBJECT][1] = new iex2DObj("DATA/makePoster/stamp/coffee.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::OBJECT][1].x = 900;
	StampData[KIND_NAME::OBJECT][1].y = 300;
	StampData[KIND_NAME::OBJECT][1].size = 80;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::OBJECT][2] = new iex2DObj("DATA/makePoster/stamp/gentei.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::OBJECT][2].x = 1090;
	StampData[KIND_NAME::OBJECT][2].y = 150;
	StampData[KIND_NAME::OBJECT][2].size = 60;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::OBJECT][3] = new iex2DObj("DATA/makePoster/stamp/hamburger.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::OBJECT][3].x = 1050;
	StampData[KIND_NAME::OBJECT][3].y = 500;
	StampData[KIND_NAME::OBJECT][3].size = 60;

	//ANIKI
	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::FOOD][0] = new iex2DObj("DATA/makePoster/stamp/aniki.png");
	StampData[KIND_NAME::FOOD][0].x = 1050;
	StampData[KIND_NAME::FOOD][0].y = 450;
	StampData[KIND_NAME::FOOD][0].size = 110;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::FOOD][1] = new iex2DObj("DATA/makePoster/stamp/�c�݂˂���.png");
	StampData[KIND_NAME::FOOD][1].x = 1000;
	StampData[KIND_NAME::FOOD][1].y = 90;
	StampData[KIND_NAME::FOOD][1].size = 50;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::FOOD][2] = new iex2DObj("DATA/makePoster/stamp/�I.png");
	StampData[KIND_NAME::FOOD][2].x = 1000;
	StampData[KIND_NAME::FOOD][2].y = 170;
	StampData[KIND_NAME::FOOD][2].size = 50;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::FOOD][3] = new iex2DObj("DATA/makePoster/stamp/���炵�˂���.png");
	StampData[KIND_NAME::FOOD][3].x = 1000;
	StampData[KIND_NAME::FOOD][3].y = 240;
	StampData[KIND_NAME::FOOD][3].size = 50;


	//�w�i
	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK ][0] = new iex2DObj("DATA/makePoster/stamp/back/not.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][0].x = 900;
	StampData[KIND_NAME::BACK][0].y = 150;
	StampData[KIND_NAME::BACK][0].size = 90;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK][1] = new iex2DObj("DATA/makePoster/stamp/back/tehutehu.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][1].x = 1100;
	StampData[KIND_NAME::BACK][1].y = 150;
	StampData[KIND_NAME::BACK][1].size = 90;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK][2] = new iex2DObj("DATA/makePoster/stamp/back/block.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][2].x = 900;
	StampData[KIND_NAME::BACK][2].y = 300;
	StampData[KIND_NAME::BACK][2].size = 90;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK][3] = new iex2DObj("DATA/makePoster/stamp/back/midori.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][3].x = 1100;
	StampData[KIND_NAME::BACK][3].y = 300;
	StampData[KIND_NAME::BACK][3].size = 90;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK][4] = new iex2DObj("DATA/makePoster/stamp/back/blue.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][4].x = 900;
	StampData[KIND_NAME::BACK][4].y = 450;
	StampData[KIND_NAME::BACK][4].size = 90;

	//�C���X�g�͂����œ���Ă����܂��B
	StampPicture[KIND_NAME::BACK][5] = new iex2DObj("DATA/makePoster/stamp/back/�Q�C�p���X.png");
	//���̊G���ǂ��Ɏ蒠�ł͂ǂ��ɕ\�����邩�Ȃǂ̏�������
	StampData[KIND_NAME::BACK][5].x = 1100;
	StampData[KIND_NAME::BACK][5].y = 450;
	StampData[KIND_NAME::BACK][5].size = 90;


}

// ���C���[�̏�����
void SceneMakePoster::LayerInit()
{
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//�@���C���[�̃f�[�^��S���O��
		ZeroMemory(&layer_data[i], sizeof(LAYER_DATA));

		layer_data[i].kind = NO_LAYER;//������ނ�-1��������`�悵�Ȃ�
	}

	//�@�w�i������ɒu���Ă���
	layer_data[(LAYER_MAX - 1)].kind = KIND_NAME::BACK;
	layer_data[(LAYER_MAX - 1)].num = 0;

	layer_data[(LAYER_MAX - 1)].x = posterScreen_W + 256;// ���Œ�
	layer_data[(LAYER_MAX - 1)].y = posterScreen_H + 256;// �c�Œ�
	layer_data[(LAYER_MAX - 1)].size = 90;


	//�@���O�̃f�[�^������΃��C���[�ǂݍ���
	LoadLayer();

}


SceneMakePoster::~SceneMakePoster()
{
	//�@�X�N���[��
	SAFE_DELETE(posterScreen);

	SAFE_DELETE(view);

	SAFE_DELETE(backPic);
	SAFE_DELETE(cursorPic);

	SAFE_DELETE(posterSizePic);
	SAFE_DELETE(posterFramePic);

	//�@�t�@�C��
	for (int i = 0; i < KIND_MAX; i++)
	{
		SAFE_DELETE(FilePic[i]);
	}

	//�@�X�^���v
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			SAFE_DELETE(StampPicture[i][no]);
		}
	}


}


bool SceneMakePoster::Update()
{
	//�@�}�E�X�̏���
	isTap = false;//����͂ގ�ɖ߂�
	if (KeyBoard(MOUSE_LEFT))	isPush = true;
	else	isPush = false;
	


	// �t�@�C���̃`�F���W
	FileChange();

	//�X�^���v�蒠����L���b�`���邺
	CatchStamp();

	//���C���[��͂�&������
	CatchLayer();

	//���񂾃X�^���v��������
	MoveLayer();

	// (��)�Z�[�u
	if (KEY_Get(KEY_ENTER)==3)
	{
		SaveLayer();
	}

	if (KEY_Get(KEY_SPACE) == 3)
	{
		MainFrame->ChangeScene(new SceneSelect());
		return true;
	}
	return true;
}

// �I��������KindSelect���`�F���W
void SceneMakePoster::FileChange()
{
	for (int i = 0; i < KIND_MAX; i++)
	{
		float len = 0;
		len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
			Vector3(FileSelectPos[i].x, FileSelectPos[i].y, 0));

		//�@�I�������t�H���_(���)�Ƀ`�F���W
		if (len < 50)
		{
			isTap = true;// �����A�C�R���Ƀ`�F���W

			if (KeyBoard(MOUSE_LEFT) == 1)
			{
				SelectKind = i;
			}
		}

	}
}

//�@�蒠�̃X�^���v���߂��ɂ���ΑI��
void SceneMakePoster::CatchStamp()
{

	// �X�^���v
	for (int i = 0; i < NO_MAX; i++)
	{
		//�@�G����������ʂ�
		if (StampPicture[SelectKind][i] != NULL)
		{
			float len = 0;
			len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
				Vector3(StampData[SelectKind][i].x, StampData[SelectKind][i].y, 0));
		
			//�@�߂��Ɏ��Ă��������
			if (len < StampData[SelectKind][i].size)
			{
				// ���@��ނ��w�i��������^�b�v��
				if (SelectKind == KIND_NAME::BACK)
				{
					isTap = true;//�A�C�R����������

					// �߂��Ń{�^������������@������
					if (KeyBoard(MOUSE_LEFT) == 1)
					{
						AddBackLayer(SelectKind, i, StampData[SelectKind][i]);// �w�i���C���[�ǉ�
					}

				}
				// �߂��Ń{�^������������@���C���[�ɂ��̊G��ǉ�
				else if (KeyBoard(MOUSE_LEFT)==1)
				{
					AddLayer(SelectKind, i, StampData[SelectKind][i]);// ���C���[�ǉ�
					return;
				}

			}

		}
	}


}

// ���C���[�ǉ�
void SceneMakePoster::AddLayer(int kind, int no, STAMP_DATA stamp)
{
	//�@�󂢂Ă郌�C���[��T��
	for (int i = 0; i < LAYER_MAX; i++)
	{

		if (layer_data[i].kind == NO_LAYER)//�@���C���[���󂾂����炻�̏ꏊ��
		{
			//�@���̑I�񂾉摜�̎�ނƃi���o�[
			layer_data[i].kind = kind;
			layer_data[i].num = no;
		
			layer_data[i].x = stamp.x;
			layer_data[i].y = stamp.y;
			layer_data[i].size = stamp.size;
		

			ChangeTopLayer(i);// ������������C���[����ԏ��

			return;
		}
		
	}
	
}

// �w�i�����C���[�ǉ�
void SceneMakePoster::AddBackLayer(int kind, int no, STAMP_DATA stamp)
{
	//�@�󂢂Ă郌�C���[��T��
	for (int i = 0; i < LAYER_MAX; i++)
	{

		//if (layer_data[i].kind == NO_LAYER)//�@���C���[���󂾂����炻�̏ꏊ��
		//{
		//	//�@���̑I�񂾉摜�̎�ނƃi���o�[
		//	layer_data[i].kind = kind;
		//	layer_data[i].num = no;

		//	layer_data[i].x = posterScreen_W+256;// ���Œ�
		//	layer_data[i].y = posterScreen_H+256;// �c�Œ�
		//	layer_data[i].size = stamp.size;


		//	ChangeBottomLayer(i);// ������������C���[����ԉ���

		//	return;
		//}


		//�@�w�i�͈�ԉ��Œ�Ł@���̉摜������ւ�
		layer_data[(LAYER_MAX-1)].kind = kind;
		layer_data[(LAYER_MAX - 1)].num = no;

		layer_data[(LAYER_MAX - 1)].x = posterScreen_W + 256;// ���Œ�
		layer_data[(LAYER_MAX - 1)].y = posterScreen_H + 256;// �c�Œ�
		layer_data[(LAYER_MAX - 1)].size = stamp.size;

		return;

	}

}

// �͂�ł郌�C���[������
void SceneMakePoster::DeleteLayer()
{
	//�@�͂܂�Ă郌�C���[��T��
	for (int i = 0; i < LAYER_MAX; i++)
	{

		if (layer_data[i].isHold == true)//�@���C���[���͂܂�Ă�
		{
			//�@���C���[�̃f�[�^��S���O��
			ZeroMemory(&layer_data[i], sizeof(LAYER_DATA));
			layer_data[i].kind = NO_LAYER;//������ނ�-1��������`�悵�Ȃ�

			return;
		}

	}

}

// �͂񂾃��C���[������
void SceneMakePoster::MoveLayer()
{
	//�@�z�[���h==true�̂�𓮂���
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//�@�����ꂽ����Ă���悤�ɂ���
		if (layer_data[i].isHold == true)
		{
			layer_data[i].x = GetMousePos().x;
			layer_data[i].y = GetMousePos().y;
		}
	}

}

// ���C���[��͂񂾂藣������
void SceneMakePoster::CatchLayer()
{
	// ���C���[�����
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//�@�󂶂�Ȃ�������ʂ� ���@�w�i����Ȃ�������͂�
		if (layer_data[i].kind != NO_LAYER && layer_data[i].kind != KIND_NAME::BACK )
		{
			float len = 0;
			len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
				Vector3(layer_data[i].x, layer_data[i].y, 0));

			//�@�߂��Ɏ��Ă��������
			if (len < layer_data[i].size)
			{
				// �߂��Ń{�^������������@����
				if (KeyBoard(MOUSE_LEFT) == 1)
				{
					ChangeTopLayer(i);
				//	ChangeBottomLayer(i);
					return;
				}
				// (��)�߂��Ń{�^���𗣂�����@�����Ēu��
				if (KeyBoard(MOUSE_LEFT) == 0)
				{
					//���������͔͈͊O�₲�ݔ��ɒu���ƃ��C���[����������
					if (GetMousePos().x<posterScreen_W - (32) || GetMousePos().x>posterScreen_W + 512 + (32) ||
						GetMousePos().y<posterScreen_H - (32) || GetMousePos().y>posterScreen_H + 512 + (32))//(��)
					{
						DeleteLayer();
					}

					//�@������Ȃ�������S���t�@���X(�K��
					for (int i = 0; i < LAYER_MAX; i++)
					{
						layer_data[i].isHold = false;
					}

				}

			}

		}
	}



}

// ���C���[����ԏ��
void SceneMakePoster::ChangeTopLayer(int layer_num)
{
	layer_data[layer_num].isHold = true;
	if (layer_num != 0)//���C���[NO���O��������擪�Ȃ̂ŏ��������Ȃ��B
	{
		for (int i = layer_num; i > 0; i--)// -1�Ƃ��Ȃ�����O�ŏI���
		{
			LAYER_DATA temp = layer_data[i];//���̃��C���[���R�s�[

			layer_data[i] = layer_data[i - 1];//���O�̃��C���[�����݂̃��C���[�̏ꏊ�Ɉڂ�
			layer_data[i - 1] = temp;//���O�̃��C���[�����݂̃��C���[�̏��ɂ���
		}
	}

}

// ���C���[����ԉ���
void SceneMakePoster::ChangeBottomLayer(int layer_num)
{
	//layer_data[layer_num].isHold = true;
	if (layer_num != (LAYER_MAX-1))//���C���[NO��10��������Ō���Ȃ̂ŏ��������Ȃ��B
	{
		for (int i = layer_num; i < (LAYER_MAX-1); i++)// +11�Ƃ��Ȃ�����10�ŏI���
		{
			LAYER_DATA temp = layer_data[i];//���̃��C���[���R�s�[

			layer_data[i] = layer_data[i + 1];//����̃��C���[�����݂̃��C���[�̏ꏊ�Ɉڂ�
			layer_data[i + 1] = temp;//����̃��C���[�����݂̃��C���[�̏��ɂ���
		}
	}
}

// ���C���[��ǂݍ���
void SceneMakePoster::LoadLayer()
{
	/* �Z�[�u�������C���[�̓ǂݍ��݁@*/
	std::ifstream in("DATA/makePoster/text/Layer.txt");
	// �o�b�t�@�ɓǂݍ���
	if (in)//���̃t�@�C��������Ȃ�I
	{
		for (int i = 0; i < LAYER_MAX; i++)
		{
			in >> layer_data[i].kind;
			in >> layer_data[i].num;
			in >> layer_data[i].x;
			in >> layer_data[i].y;
			in >> layer_data[i].size;
			in >> layer_data[i].isHold;
		}

		in.close();
	}

}

// ���C���[�������o��
void SceneMakePoster::SaveLayer()
{

	/* ���C���[�̃Z�[�u */
	std::ofstream out("DATA/makePoster/text/Layer.txt");
	for (int i = 0; i < LAYER_MAX; i++)
	{
		out << layer_data[i].kind	<< "\n";
		out << layer_data[i].num	<< "\n";
		out << layer_data[i].x		<< "\n";
		out << layer_data[i].y		<< "\n";
		out << layer_data[i].size	<< "\n";
		out << layer_data[i].isHold << "\n";

	}
//	std::fclose();

}

//******************************************************
//		�}�E�X�֘A
//******************************************************
Vector3 SceneMakePoster::GetMousePos(){
	Vector3 pos;
	POINT Mpos;
	RECT rc;
	GetCursorPos(&Mpos);
	GetWindowRect(iexSystem::Window, &rc);
	//�}�E�X�̎擾�ʒu�����E�C���h�E����
	Mpos.x = (Mpos.x) - (rc.left + 8);
	Mpos.y = (Mpos.y) - (rc.top + 35);
	pos = Vector3((float)Mpos.x, (float)Mpos.y, 0.0f);
	return pos;
}

/*-----------------------------------*/
//		�`��
/*-----------------------------------*/
void SceneMakePoster::Render()
{
	view->Activate();

	PosterRender();

	//�o�b�N�o�b�t�@
	iexSystem::Device->SetRenderTarget(0, backbuffer);
	view->Clear();

	//�@���̔w�i
	backPic->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	// �|�X�^�[�̏ꏊ
	posterSizePic->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//�@�t�@�C��
	for (int i = 0; i < KIND_MAX; i++)
	{
		FilePic[i]->Render(700, 20, 512, 1024, 0, 0, 512, 1024);
	}
	//�@�I�������t�@�C������ԑO��
	FilePic[SelectKind]->Render(700, 20, 512, 1024, 0, 0, 512, 1024);

	// �X�^���v�蒠�̉摜
	for (int no = (NO_MAX-1); no >= 0; no--)//���`�揇���O����O�ɗ���悤��
	{
		//�@NULL����Ȃ�������`�悷��
		if (StampPicture[SelectKind][no] != NULL)
		{
			if (SelectKind==KIND_NAME::BACK)//�w�i�͏������`��
			{
				StampPicture[SelectKind][no]->Render((int)(StampData[SelectKind][no].x - 256 / 4),
					(int)(StampData[SelectKind][no].y - 256 / 4), 512 / 4, 512 / 4, 0, 0, 512, 512);
			}
			else
			{
				StampPicture[SelectKind][no]->Render((int)StampData[SelectKind][no].x - 256,
					(int)StampData[SelectKind][no].y - 256, 512, 512, 0, 0, 512, 512);
			}

		}
	}


	//�@���C���[�̉摜
	for (int i = (LAYER_MAX - 1); i >= 0; i--)//���`�揇���O����O�ɗ���悤��
	{
		if (layer_data[i].kind != NO_LAYER)//�@���C���[���󂾂�����`�悵�Ȃ�
		{
			if (layer_data[i].isHold==true)//�͂�ł��镨�����`�悵�Ȃ��I
			{
			//�@�G��\��
				StampPicture[layer_data[i].kind][layer_data[i].num]->SetARGB(0x66ffffff);
			StampPicture[layer_data[i].kind][layer_data[i].num]
				->Render((int)layer_data[i].x - 256, (int)layer_data[i].y - 256, 512, 512, 0, 0, 512, 512);
			}

		}

	}

	// �|�X�^�[�̃t���[��
	posterFramePic->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//�J�[�\���A�C�R��
	cursorPic->Render((int)GetMousePos().x - (32), (int)GetMousePos().y - (32), 64, 64, isPush * 64, isTap * 64, 64, 64);


	posterScreen->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//	�f�o�b�O����
#ifdef _DEBUG

	for (int i = 0; i < LAYER_MAX; i++)
	{
		Text::Draw(10, 80 + (i * 30), 0xff0000ff, "���C���[[%d]�̎��->%d", i, layer_data[i].kind);
		Text::Draw(10, 400 + (i * 30), 0xff0000ff, "���C���[[%d]�̃i���o�[->%d", i, layer_data[i].num);
	}

	for (int i = 0; i < KIND_MAX; i++)
	{
		cursorPic->Render((int)FileSelectPos[i].x - 32, (int)FileSelectPos[i].y - 32, 64, 64, 0, 0, 64, 64);
	}
	
#endif

}

//�@�|�X�^�[�̃e�N�X�`���[�G
void SceneMakePoster::PosterRender()
{
	posterScreen->RenderTarget();
	view->Clear(0xffffffff);
	//�@��������|�X�^�[�̃e�N�X�`���[�Ɉڂ�G��`��

	//�@���C���[�̉摜
	for (int i = (LAYER_MAX - 1); i >= 0; i--)//���`�揇���O����O�ɗ���悤��
	{
		if (layer_data[i].kind != NO_LAYER)//�@���C���[���󂾂�����`�悵�Ȃ�
		{
			//�@�G��\���@���|�X�^�[�̕\���ꏊ�����炵�Ă���
			StampPicture[layer_data[i].kind][layer_data[i].num]
				->Render((int)(layer_data[i].x - 256 - (posterScreen_W)), (int)(layer_data[i].y - 256 - (posterScreen_H)), 512, 512, 0, 0, 512, 512);
		
		}
	}

	// �|�X�^�[�̃t���[�� ���N���C�A���g�̔ԍ��ɂ���ĐF����ŕ�����
	posterFramePic->Render(0, 0, 512, 512, 0, 0, 512, 512);

	//�J�[�\���A�C�R�� ���|�X�^�[�̕\���ꏊ��������炵�Ă���
	cursorPic->Render((int)(GetMousePos().x - (32) - (posterScreen_W)), (int)(GetMousePos().y - (32) - (posterScreen_H)), 64, 64, isPush * 64, 0, 64, 64);


}