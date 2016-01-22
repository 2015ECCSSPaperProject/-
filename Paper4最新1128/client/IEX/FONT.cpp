#include	"iExtreme.h"
#include	<mbstring.h>
#include	<tchar.h>



/*********************************/
//
///		FONT
//
/********************************/

//	�Ǘ��p�p�����[�^
bool					Font::m_IsInitialized = false;
Font::CacheDesc		Font::m_CacheDesc = { 0 };
// ��
/***
UINT		nextUseCacheNum;		//	���Ɏg�p����L���b�V���̔ԍ�
iex2DObj*		textureCacheList;		//	�����e�N�X�`���̃��X�g
RefData*	referenceDataList;		//	�L���b�V���̏ƍ��p�f�[�^���X�g
***/

// �A�E�g���C��
Font::CacheDescOL Font::m_CacheDescOL = { 0 };

//	�`��p�p�����[�^
DWORD					Font::m_DefaultFontColor = 0xFFFFFFFF;
float					Font::m_DefaultFontWidth = 25.0f;
float					Font::m_DefaultFontHeight = 25.0f;

const	UINT	CacheQty = 512;			//	�L���b�V���̐�



// ������
void Font::Initialize(void)
{
	//	�ʏ핶���L���b�V���̏�����
	{
		//	���łɊm�ۂ���Ă���̈悪����ꍇ�͔j��
		delete[]	m_CacheDesc.textureCacheList;	m_CacheDesc.textureCacheList = NULL;//	�����e�N�X�`���̃��X�g
		delete[]	m_CacheDesc.referenceDataList;	m_CacheDesc.referenceDataList = NULL;//	�L���b�V���̏ƍ��p�f�[�^���X�g

		//	�L���b�V����ۑ�����̈���m��
		m_CacheDesc.textureCacheList = new iex2DObj[CacheQty];  // �����e�N�X�`���̃��X�g
		m_CacheDesc.referenceDataList = new	RefData[CacheQty];	// �L���b�V���̏ƍ��p�f�[�^�̃��X�g

		//	�L���b�V����������
		for (UINT i = 0; i < CacheQty; i++)
		{
			m_CacheDesc.referenceDataList[i].isEnable = false;// �L���t���O��false
		}

		m_CacheDesc.nextUseCacheNum = 0;//	���Ɏg�p����L���b�V���̔ԍ�
	}

	//	�A�E�g���C���t�������L���b�V���̏�����
	{

	//	���łɊm�ۂ���Ă���̈悪����ꍇ�͔j��
	delete[]	m_CacheDescOL.textureCacheList;		m_CacheDescOL.textureCacheList = NULL;
	delete[]	m_CacheDescOL.referenceDataList;	m_CacheDescOL.referenceDataList = NULL;

	//	�L���b�V����ۑ�����̈���m��
	m_CacheDescOL.textureCacheList = new	iex2DObj[CacheQty];
	m_CacheDescOL.referenceDataList = new	RefDataOL[CacheQty];

	//	������
	for (UINT i = 0; i < CacheQty; i++){
		m_CacheDescOL.referenceDataList[i].isEnable = false;
	}

	m_CacheDescOL.nextUseCacheNum = 0;
}

	//	�������t���O�𗧂Ă�
	m_IsInitialized = true;

}

void Font::Release(void)
{

	//	�ʏ�L���b�V��
	{
		delete[]	m_CacheDesc.textureCacheList;	m_CacheDesc.textureCacheList = NULL;
		delete[]	m_CacheDesc.referenceDataList;	m_CacheDesc.referenceDataList = NULL;
		m_CacheDesc.nextUseCacheNum = 0;
	}
	
	//	�A�E�g���C���t�������L���b�V��
	{
	delete[]	m_CacheDescOL.textureCacheList;		m_CacheDescOL.textureCacheList = NULL;
	delete[]	m_CacheDescOL.referenceDataList;	m_CacheDescOL.referenceDataList = NULL;
	m_CacheDescOL.nextUseCacheNum = 0;
	}


	//	�������t���O�����Z�b�g
	m_IsInitialized = false;
}


//	��������`��@���ƍ����ƐF���l�����Ȃ��o�[�W����
void	Font::DrawString2D(LPCSTR _String, LPCSTR _FontName, float _DrawX, float _DrawY, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�`��֐��̌Ăяo��
	DrawString2D(_String, _FontName, _DrawX, _DrawY, m_DefaultFontWidth, m_DefaultFontHeight, 0.0f, m_DefaultFontColor, _Depth, _iexRenderStateFlag);
}

//�@�����̖{��
void	Font::DrawString2D(LPCSTR _String, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�ϐ��̗p��
	float	x = _DrawX;
	float	y = _DrawY;
	float	halfWidth = _Width  * 0.5f;
	float	halfHeight = _Height * 0.5f;
	float	centerX = _DrawX + _Width  * 0.5f * strlen(_String) * 0.5f;
	float	centerY = _DrawY + _Height * 0.5f;
	UINT	myByte = 0;
	//	�I�[�����܂Ń��[�v
	for (UINT i = 0; _String[i] != '\0'; i += myByte){
		//	�����̃o�C�g���𒲂ׂ�
		myByte = _mbclen((BYTE*)&_String[i]);

		//	�󔒂̓X�L�b�v
		bool	skip(false);
		{
			if (myByte == 1){
				if (_String[i] == ' ')				skip = true;
			}
			else	if (myByte == 2){
				char	buffer[3] = { _String[i], _String[i + 1], '\0' };
				if (strcmp(buffer, "�@") == 0)	skip = true;
			}
		}

		//	������`��
		if (!skip){
			float	drawCoordX = x;
			float	drawCoordY = y;

			float	charaCenX = x + halfWidth  * 0.5f * myByte;
			float	charaCenY = y + halfHeight;
			float	distX = centerX - charaCenX;
			float	distY = centerY - charaCenY;
			float	distance = sqrtf(distX * distX + distY * distY);
			float	beforeAngle = atan2(distX, distY);

			//drawCoordX = centerX - sin(beforeAngle - _Angle) * distance - halfWidth  * 0.5f * myByte;
			//drawCoordY = centerY - cos(beforeAngle - _Angle) * distance - halfHeight;
			DrawCharacter2D(&_String[i], _FontName, drawCoordX, drawCoordY, _Width, _Height, _Angle, _Color, _Depth, _iexRenderStateFlag);
		}
		//	�`����W�����炷
		x += halfWidth * myByte;
	}
}

//	������`��i�������j
void	Font::DrawString2Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�`��֐��̌Ăяo����
	DrawString2Dc(_String, _FontName, _FontSize, _DrawX, _DrawY, 1.0f, 1.0f, 0xFFFFFFFF, _Depth, _iexRenderStateFlag);

}

// �������͂����ɂ���
void	Font::DrawString2Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�ϐ��̗p��
	float	x = _DrawX;
	float	y = _DrawY;
	UINT	myByte = 0;
	//	�I�[�����܂Ń��[�v
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	�����̃o�C�g���𒲂ׂ�	
		myByte = _mbclen((BYTE*)&_String[i]);
		
		//	������`��
		float	drawCoordX = x;
		float	drawCoordY = y;
		Vector2	drawedSize = DrawCharacter2Dc(&_String[i], _FontName, _FontSize, drawCoordX, drawCoordY, _ScaleX, _ScaleY, 0.0f, _Color, _Depth, _iexRenderStateFlag);

		//	�`����W�����炷
		x += drawedSize.x;
	}
}

void Font::RenderFont2D(LPCSTR _String, int _FontSize, float _DrawX, float _DrawY, DWORD col)
{

		LPSTR	c_FontName = "HGS�n�p�p�߯�ߑ�";
		int Quality = 5;
	
		DrawString2Dc(_String, c_FontName, _FontSize,
			_DrawX, _DrawY, 1.0f, 1.0f, col, 0.0f, RS_COPY);
}


// �������͂����ɂ���
void	Font::DrawString3Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�ϐ��̗p��
	float	x = _DrawX;
	float	y = _DrawY;
	float	z = _Depth;
	UINT	myByte = 0;
	//	�I�[�����܂Ń��[�v
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	�����̃o�C�g���𒲂ׂ�	
		myByte = _mbclen((BYTE*)&_String[i]);

		//	������`��
		float	drawCoordX = x;
		float	drawCoordY = y;
		Vector2	drawedSize = DrawCharacter3Dc(&_String[i], _FontName, _FontSize, drawCoordX, drawCoordY, _ScaleX, _ScaleY, 0.0f, _Color, z, _iexRenderStateFlag);

		//	�`����W�����炷

		Vector3 ViewVec;
		ViewVec.x = matView._11;
		ViewVec.y = matView._21;
		ViewVec.z = matView._31;
		ViewVec.Normalize();

		x += ViewVec.x * 2;
		y += ViewVec.y * 2;
		z += ViewVec.z * 2;


	}
}

void Font::RenderFont3D(LPCSTR _String, int _FontSize, Vector3 _Pos, DWORD col)
{
	LPSTR	c_FontName = "HGS�n�p�p�߯�ߑ�";
	int Quality = 5;

	std::string str = _String;
	int len = str.length();
	//_Pos.x -= (len*.5f) * _FontSize;
	_Pos -= Vector3(matView._11, matView._21, matView._31) * (len*.5f);

	DrawString3Dc(_String, c_FontName, _FontSize,
		_Pos.x, _Pos.y, 1.0f, 1.0f, col, _Pos.z, RS_COPY);
}

// �����`��
//void Font::RnederFont(LPCSTR string, float x, float y, int fontSize)
//{
//
//	LPSTR	c_FontName = "HGS�n�p�p�߯�ߑ�";
//	int Quality = 5;
//
//	DrawOutlineString2D(string, c_FontName, fontSize,
//		120, 4, 0xffffffff, 0xff020000,
//		Quality, x, y, 1.0f, 1.0f, 0xffffffff, 0.0f, RS_COPY_NOZ);
//}
//
//void Font::RnederFont3D(LPCSTR string, Vector3 pos, int fontSize)
//{
//
//	LPSTR	c_FontName = "HGS�n�p�p�߯�ߑ�";
//	int Quality = 5;
//
//	DrawOutlineString3D(string, c_FontName, fontSize,
//		120, 4, 0xffffffff, 0xff020000,
//		Quality, pos.x, pos.y, 1.0f, 1.0f, 0xffffffff, pos.z, RS_COPY_NOZ);
//}

//	�A�E�g���C���t������
//void	Font::DrawOutlineString2D(LPCSTR _String, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	�ϐ��̗p��
//	float	x = _DrawX;
//	float	y = _DrawY;
//	UINT	myByte = 0;
//	//	�I�[�����܂Ń��[�v
//	for (UINT i = 0; _String[i] != '\0'; i += myByte){
//		//	�����̃o�C�g���𒲂ׂ�
//		myByte = _mbclen((BYTE*)&_String[i]);
//
//		//	�`����W�v�Z
//		float	drawCoordX = x;
//		float	drawCoordY = y;
//
//		if (_String[i] == '��')
//		{
//			_DrawY += 32;
//		}
//
//		//	�`��
//		Vector2	charaSize = DrawOutlineCharacter2D(_FontName, &_String[i], _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality, drawCoordX, drawCoordY, _ScaleX, _ScaleY, _Color, _Depth, _iexRenderStateFlag);
//
//		//	�`����W�����炷
//		x += charaSize.x;
//	}
//}

//	�A�E�g���C���t������3D
//void	Font::DrawOutlineString3D(LPCSTR _String, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	�ϐ��̗p��
//	float	x = _DrawX;
//	float	y = _DrawY;
//	float	z = _Depth;
//	UINT	myByte = 0;
//	//	�I�[�����܂Ń��[�v
//	for (UINT i = 0; _String[i] != '\0'; i += myByte){
//		//	�����̃o�C�g���𒲂ׂ�
//		myByte = _mbclen((BYTE*)&_String[i]);
//
//		//	�`����W�v�Z
//		float	drawCoordX = x;
//		float	drawCoordY = y;
//
//		if (_String[i] == '��')
//		{
//			_DrawY += 30;
//		}
//		
//		//	�`��
//		Vector2	charaSize = DrawOutlineCharacter3D(_FontName, &_String[i], _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality, drawCoordX, drawCoordY, _ScaleX, _ScaleY, _Color, z, _iexRenderStateFlag);
//
//		//	�`����W�����炷
//		Vector3 ViewVec;
//		ViewVec.x = matView._11;
//		ViewVec.y = matView._21;
//		ViewVec.z = matView._31;
//		ViewVec.Normalize();
//
//		x += ViewVec.x*2;
//		y += ViewVec.y*2;
//		z += ViewVec.z*2;
//		//x += charaSize.x;
//	}
//}

/******************************************************/
//
///			��Char�`��
//
/******************************************************/

// Char�`��@�V�F�[�_�[�Ȃ���
void	Font::DrawCharacter2D(LPCSTR _Character, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
	if (!m_IsInitialized)	Initialize();

	//	�ϐ��̗p��
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);
	//	�����̃o�C�g���ɉ����ĕ��𒲐�
	_Width = _Width * 0.5f * byteSize;
	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
	if (_Width > _Height)	createSize = (UINT)_Width;
	else					createSize = (UINT)_Height;

	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	�`��
	//pImage->Render(_DrawX, _DrawY, _Width, _Height, 0, 0, _Width, _Height, _iexRenderStateFlag, _Color, _Depth);
	pImage->Render((int)_DrawX, (int)_DrawY, (int)_Width, (int)_Height, 0, 0, (int)_Width, (int)_Height, _iexRenderStateFlag);
}

// Char�@�V�F�[�_�[�����
void	Font::DrawCharacter2D(LPCSTR _Character, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, iexShader* _pShader, LPSTR _Technique)
{
	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
	if (!m_IsInitialized)	Initialize();

	//	�ϐ��̗p��
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);
	//	�����̃o�C�g���ɉ����ĕ��𒲐�
	_Width = _Width * 0.5f * byteSize;
	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
	if (_Width > _Height)	createSize = (UINT)_Width;
	else					createSize = (UINT)_Height;

	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	�`��
	//pImage->Render(_DrawX, _DrawY, _Width, _Height, 0, 0, _Width, _Height, _pShader, _Technique, _Color, _Depth);
	//pImage->Draw2Ds(_DrawX, _DrawY, _Width, _Height, _Angle, _Color, _Depth, _pShader, _Technique);
	pImage->Render((int)_DrawX, (int)_DrawY, (int)_Width, (int)_Height, 0, 0, (int)_Width, (int)_Height, _pShader, _Technique);
}

Vector2	Font::DrawCharacter2Dc(LPCSTR _Character, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
	if (!m_IsInitialized)	Initialize();

	//	�ϐ��̗p��
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);

	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
	int		createSizeW = (int)((float)_FontSize * _ScaleX);
	int		createSizeH = (int)((float)_FontSize * _ScaleY);
	if (createSizeW > createSizeH)	createSize = createSizeW;
	else							createSize = createSizeH;

	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	�󔒃`�F�b�N
	bool	draw(true);
	switch (byteSize){
	case	1:
		if (_Character[0] == ' '){
			draw = false;
		}
		break;
	case	2:
	{
				 char	buffer[3] = { _Character[0], _Character[1], '\0' };
				 if (strcmp(buffer, "�@") == 0){
					 draw = false;
				 }
	}
		break;
	}

	//	�`��
	if (draw){
		//pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
		//	0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
		//	pImage->Draw2D(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _Angle, _Color, _Depth, _iexRenderStateFlag);
		pImage->SetARGB(_Color);
		pImage->Render((int)_DrawX, (int)_DrawY, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY),
			0, 0, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY), _iexRenderStateFlag);
	}

	//	�`�悵�������̃T�C�Y��Ԃ�
	return	Vector2(pImage->GetWidth(), pImage->GetHeight());
}

//3D
Vector2	Font::DrawCharacter3Dc(LPCSTR _Character, LPCSTR _FontName, int _FontSize, float _DrawX,
	float _DrawY, float _ScaleX, float _ScaleY, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{

	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
	if (!m_IsInitialized)	Initialize();

	//	�ϐ��̗p��
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);

	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
	int		createSizeW = (int)((float)_FontSize * _ScaleX);
	int		createSizeH = (int)((float)_FontSize * _ScaleY);
	if (createSizeW > createSizeH)	createSize = createSizeW;
	else							createSize = createSizeH;

	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	�󔒃`�F�b�N
	bool	draw(true);
	switch (byteSize){
	case	1:
		if (_Character[0] == ' '){
			draw = false;
		}
		break;
	case	2:
	{
				 char	buffer[3] = { _Character[0], _Character[1], '\0' };
				 if (strcmp(buffer, "�@") == 0){
					 draw = false;
				 }
	}
		break;
	}

	//	�`��
	if (draw){
		//pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
		//	0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
		//	pImage->Draw2D(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _Angle, _Color, _Depth, _iexRenderStateFlag);
		pImage->SetARGB(_Color);
		//pImage->SetScale(10);
		pImage->Render3D(_DrawX, _DrawY, _Depth, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY),
			0, 0, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY), _iexRenderStateFlag);
	}

	//	�`�悵�������̃T�C�Y��Ԃ�
	return	Vector2(pImage->GetWidth(), pImage->GetHeight());



}




//	�A�E�g���C���t��
//Vector2	Font::DrawOutlineCharacter2D(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
//	if (!m_IsInitialized)	Initialize();
//
//	//	�ϐ��̗p��
//	iex2DObj*	pImage;
//	UINT	createSize;
//	UINT	byteSize = _mbclen((BYTE*)_Character);
//
//	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
//	int		createSizeW = (int)((float)_FontSize * _ScaleX);
//	int		createSizeH = (int)((float)_FontSize * _ScaleY);
//	if (createSizeW > createSizeH)	createSize = createSizeW;
//	else							createSize = createSizeH;
//
//	//	�L���b�V���ԍ����擾
//	int		cacheNum = SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//
//	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
//	pImage = &m_CacheDescOL.textureCacheList[SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality)];
//
//	//	�󔒃`�F�b�N
//	bool	draw(true);
//	switch (byteSize){
//	case	1:
//		if (_Character[0] == ' '){
//			draw = false;
//		}
//		break;
//	case	2:
//	{
//				 char	buffer[3] = { _Character[0], _Character[1], '\0' };
//				 if (strcmp(buffer, "�@") == 0){
//					 draw = false;
//				 }
//	}
//		break;
//	}
//
//	//	�`��
//	if (draw){
//		//	�`��ʒu�v�Z
//		Vector2			charaSize = GetCharacterSizeOL(_Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		TEXTMETRIC		textMatric;
//		GLYPHMETRICS	glyphMetrics;
//		GetMatric(&textMatric, &glyphMetrics, _Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		float			upper = (float)(textMatric.tmAscent - glyphMetrics.gmptGlyphOrigin.y);
//
//		//	�`��
//		pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
//			0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
//	}
//
//	//	�`�悵�������̃T�C�Y��Ԃ�
//	return	Vector2((float)pImage->GetWidth(), (float)pImage->GetHeight());
//}



//	�A�E�g���C���t��3D
//Vector2	Font::DrawOutlineCharacter3D(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	�����e�N�X�`���Ǘ��p�����[�^������������Ă��邩�`�F�b�N
//	if (!m_IsInitialized)	Initialize();
//
//	//	�ϐ��̗p��
//	iex2DObj*	pImage;
//	UINT	createSize;
//	UINT	byteSize = _mbclen((BYTE*)_Character);
//
//	//	���ƍ������r���đ傫���ق�����Ƀe�N�X�`�����쐬
//	int		createSizeW = (int)((float)_FontSize * _ScaleX);
//	int		createSizeH = (int)((float)_FontSize * _ScaleY);
//	if (createSizeW > createSizeH)	createSize = createSizeW;
//	else							createSize = createSizeH;
//
//	//	�L���b�V���ԍ����擾
//	int		cacheNum = SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//
//	//	�g�p����e�N�X�`���̃A�h���X���Z�b�g
//	pImage = &m_CacheDescOL.textureCacheList[SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality)];
//
//	//	�󔒃`�F�b�N
//	bool	draw(true);
//	switch (byteSize){
//	case	1:
//		if (_Character[0] == ' '){
//			draw = false;
//		}
//		break;
//	case	2:
//	{
//				 char	buffer[3] = { _Character[0], _Character[1], '\0' };
//				 if (strcmp(buffer, "�@") == 0){
//					 draw = false;
//				 }
//	}
//		break;
//	}
//
//	//	�`��
//	if (draw){
//		//	�`��ʒu�v�Z
//		Vector2			charaSize = GetCharacterSizeOL(_Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		TEXTMETRIC		textMatric;
//		GLYPHMETRICS	glyphMetrics;
//		GetMatric(&textMatric, &glyphMetrics, _Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		float			upper = (float)(textMatric.tmAscent - glyphMetrics.gmptGlyphOrigin.y);
//
//		//	�`��
//		Vector3 POS;
//		POS.x = _DrawX;
//		POS.y = _DrawY;
//		POS.z = _Depth;
//		pImage->Render3D(POS, _iexRenderStateFlag);
//	}
//
//	//	�`�悵�������̃T�C�Y��Ԃ�
//	return	Vector2((float)pImage->GetWidth(), (float)pImage->GetHeight());
//}

//****************************************************************************************************************************************************************
//		���̎擾
//****************************************************************************************************************************************************************

//	�w�肳�ꂽ�t�H���g�A�����T�C�Y�Ő�������镶����̃T�C�Y���擾
Vector2	Font::GetStringSize(LPCSTR _String, LPCSTR _FontName, int _FontSize)
{
	Vector2	strSize(0, _FontSize);

	UINT	myByte = 0;

	//	�I�[�����܂Ń��[�v
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	�����̃o�C�g���𒲂ׂ�
		myByte = _mbclen((BYTE*)&_String[i]);

		//	�T�C�Y�𒲂ׂ�
		Vector2	charSize = GetCharacterSize(&_String[i], _FontName, _FontSize);
		strSize.x += charSize.x;
	}

	return	strSize;

}

// �ꕶ���t�H���g�A�T�C�Y��
Vector2	Font::GetCharacterSize(LPCSTR _Character, LPCSTR _FontName, int _FontSize)
{
	//	�t�H���g�̐���
	LOGFONT	lf = { _FontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, (FIXED_PITCH | FF_MODERN), "�l�r ����" };
	strcpy_s(lf.lfFaceName, _FontName);
	HFONT	hFont;
	if (!(hFont = CreateFontIndirect(&lf)))	return	Vector2(0, 0);

	//	�f�o�C�X�R���e�L�X�g�擾
	HDC		hdc = GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hdc, hFont);

	//	�����R�[�h�擾
	TCHAR *c = _T((LPSTR)_Character);
	UINT code = 0;
	if (IsDBCSLeadByte(*c))	code = (BYTE)c[0] << 8 | (BYTE)c[1];
	else						code = c[0];

	//	�t�H���g�r�b�g�}�b�v�擾
	TEXTMETRIC	TM;
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS	GM;
	CONST MAT2	Mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	DWORD		size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);

	//	�f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);

	return	Vector2(GM.gmCellIncX, TM.tmHeight);
}

//Vector2	Font::GetCharacterSizeOL(LPCSTR _Character, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, int _Quality)
//{
//	HWND				hWnd = iexSystem::Window;
//	IDirect3DDevice9*	dev = iexSystem::Device;
//
//	LPCSTR				faceName = _FontName;
//	UCHAR				charSet = SHIFTJIS_CHARSET;
//	LPCSTR				str = _Character;
//	UINT				fontHeight = _FontSize;
//	UINT				weight = _FontWeight;
//	int					penSize = _EdgeWidth;
//	int					quality = _Quality;
//
//	penSize *= quality;
//
//	// �f�o�C�X�R���e�L�X�g�쐬
//	HDC	 hDC = GetDC(hWnd);
//	HDC	 memDC = CreateCompatibleDC(hDC);
//	ReleaseDC(hWnd, hDC);   // ������Ă�����
//
//	// �t�H���g�쐬
//	LOGFONTA lf = {};
//	lf.lfHeight = fontHeight * quality;    // �g��T�C�Y��
//	lf.lfCharSet = charSet;
//	lf.lfWeight = weight;
//	lf.lfQuality = ANTIALIASED_QUALITY;
//	memcpy(lf.lfFaceName, faceName, strlen(faceName));
//	HFONT	 hFont = CreateFontIndirectA(&lf);
//	HFONT	 oldFont = (HFONT)SelectObject(memDC, hFont);     // �t�H���g��ݒ肵�Ȃ��ƃG���[�ɂȂ�܂�
//
//	//	�ł�����"��"��`�悷��BMP���쐬
//	TEXTMETRIC		tm;
//	GLYPHMETRICS	gm;
//	MAT2	mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
//	int		len = IsDBCSLeadByte(str[0]) ? 2 : 1;
//	UINT	code = (len == 2 ? (unsigned char)str[0] << 8 | (unsigned char)str[1] : (unsigned char)str[0]);
//	GetTextMetricsA(memDC, &tm);
//	GetGlyphOutlineA(memDC, code, GGO_METRICS, &gm, 0, 0, &mat);
//
//	SelectObject(memDC, oldFont);
//	DeleteObject(hFont);
//	DeleteDC(memDC);
//
//
//	Vector2	fontSize;
//	fontSize.x = (float)(((gm.gmBlackBoxX + penSize + quality - 1) / quality * quality) / quality);
//	fontSize.y = (float)(((gm.gmBlackBoxY + penSize + quality - 1) / quality * quality) / quality);
//
//	return	fontSize;
//}

//
//void	Font::GetMatric(TEXTMETRIC* _pTextMetric, GLYPHMETRICS* _pGlyphMetrics, LPCSTR _Character, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, int _Quality)
//{
//	HWND				hWnd = iexSystem::Window;
//	IDirect3DDevice9*	dev = iexSystem::Device;
//
//	LPCSTR				faceName = _FontName;
//	UCHAR				charSet = SHIFTJIS_CHARSET;
//	LPCSTR				str = _Character;
//	UINT				fontHeight = _FontSize;
//	UINT				weight = _FontWeight;
//	int					penSize = _EdgeWidth;
//
//	// �f�o�C�X�R���e�L�X�g�쐬
//	HDC	 hDC = GetDC(hWnd);
//	HDC	 memDC = CreateCompatibleDC(hDC);
//	ReleaseDC(hWnd, hDC);   // ������Ă�����
//
//	// �t�H���g�쐬
//	LOGFONTA lf = {};
//	lf.lfHeight = fontHeight;
//	lf.lfCharSet = charSet;
//	lf.lfWeight = weight;
//	lf.lfQuality = ANTIALIASED_QUALITY;
//	memcpy(lf.lfFaceName, faceName, strlen(faceName));
//	HFONT	 hFont = CreateFontIndirectA(&lf);
//	HFONT	 oldFont = (HFONT)SelectObject(memDC, hFont);     // �t�H���g��ݒ肵�Ȃ��ƃG���[�ɂȂ�܂�
//
//	//	�ł�����"��"��`�悷��BMP���쐬
//	MAT2	mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
//	int		len = IsDBCSLeadByte(str[0]) ? 2 : 1;
//	UINT	code = (len == 2 ? (unsigned char)str[0] << 8 | (unsigned char)str[1] : (unsigned char)str[0]);
//	GetTextMetricsA(memDC, _pTextMetric);
//	GetGlyphOutlineA(memDC, code, GGO_METRICS, _pGlyphMetrics, 0, 0, &mat);
//
//	SelectObject(memDC, oldFont);
//	DeleteObject(hFont);
//	DeleteDC(memDC);
//}


//****************************************************************************************************************************************************************
//		�T�|�[�g�֐�
//****************************************************************************************************************************************************************
//	�L���b�V���̌������쐬
UINT	Font::SearchCache(LPCSTR chara, UINT size, LPCSTR _FontName)
{
	//	�g�p�\�ȃL���b�V����T��
	UINT	sizeAllow = 0;							//	���e�T�C�Y��
	UINT	byteSize = _mbclen((BYTE*)chara);	//	�o�C�g��
	//	�L���b�V���������[�v
	for (UINT cNum = 0; cNum < CacheQty; cNum++){
		RefData&	refData = m_CacheDesc.referenceDataList[cNum];
		if (!refData.isEnable)									continue;
		//	�o�C�g������
		if (refData.byte != byteSize)							continue;
		//	��������i�o�C�g���ɉ����ĕ���j
		if (byteSize != 2){
			if (refData.chara[0] != chara[0])					continue;
		}
		else{
			if (refData.chara[0] != chara[0])					continue;
			if (refData.chara[1] != chara[1])					continue;
		}
		//	�T�C�Y����
		if (refData.size - sizeAllow > size)					continue;
		if (refData.size + sizeAllow < size)					continue;
		//	�t�H���g����
		if (strlen(refData.fontName) == 0){
			if (strlen(_FontName) != 0)						continue;
		}
		else{
			if (strlen(_FontName) == 0)						continue;
			if (strcmp(refData.fontName, _FontName) != 0)	continue;
		}
		//	������N���A
		return	cNum;
	}

	//	�V���������e�N�X�`�����쐬
	UINT	useNum = m_CacheDesc.nextUseCacheNum;
	m_CacheDesc.textureCacheList[useNum].LoadFontTexture(chara, size, _FontName);
	//	�ƍ��p�p�����[�^���X�V
	RefData&	refData = m_CacheDesc.referenceDataList[useNum];
	refData.chara[0] = chara[0];
	refData.chara[1] = chara[1];
	refData.size = size;
	refData.byte = byteSize;
	refData.isEnable = true;
	refData.fontName = _FontName;

	//	���ɏ���������L���b�V���̔ԍ����X�V
	if (++m_CacheDesc.nextUseCacheNum == CacheQty)	m_CacheDesc.nextUseCacheNum = 0;

	//	�V�����e�N�X�`�����쐬�����ԍ���Ԃ�
	return	useNum;
}


// �A�E�g���C���p

//	�L���b�V���̌������쐬�i�A�E�g���C���t���t�H���g�p�j
//UINT		Font::SearchCacheOL(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality)
//{
//	//	�g�p�\�ȃL���b�V����T��
//	UINT	sizeAllow = 0;								//	���e�T�C�Y��
//	UINT	byteSize = _mbclen((BYTE*)_Character);		//	�o�C�g��
//	//	�L���b�V���������[�v
//	for (UINT cNum = 0; cNum < CacheQty; cNum++){
//		RefDataOL&	refData = m_CacheDescOL.referenceDataList[cNum];
//		if (!refData.isEnable)									continue;
//		//	�o�C�g������
//		if (refData.byte != byteSize)							continue;
//		//	��������i�o�C�g���ɉ����ĕ���j
//		if (byteSize != 2){
//			if (refData.chara[0] != _Character[0])				continue;
//		}
//		else{
//			if (refData.chara[0] != _Character[0])				continue;
//			if (refData.chara[1] != _Character[1])				continue;
//		}
//		//	�T�C�Y����
//		if (refData.size - sizeAllow > _FontSize)				continue;
//		if (refData.size + sizeAllow < _FontSize)				continue;
//		//	�t�H���g����
//		if (strlen(refData.fontName) == 0){
//			if (strlen(_FontName) != 0)						continue;
//		}
//		else{
//			if (strlen(_FontName) == 0)						continue;
//			if (strcmp(refData.fontName, _FontName) != 0)	continue;
//		}
//		//	�t�H���g�̕��i�H�j����
//		if (refData.fontWeight != _FontWeight)					continue;
//		//	�A�E�g���C���̕�����
//		if (refData.edgeWidth != _EdgeWidth)					continue;
//		//	�h��̐F�i�����̐F�j����
//		if (refData.fillColor != _FillColor)					continue;
//		//	�A�E�g���C���̐F����
//		if (refData.edgeColor != _EdgeColor)					continue;
//		//	�i������
//		if (refData.quality != _Quality)						continue;
//
//		//	������N���A
//		return	cNum;
//	}
//
//	//	�V���������e�N�X�`�����쐬
//	UINT	useNum = m_CacheDescOL.nextUseCacheNum;
//	m_CacheDescOL.textureCacheList[useNum].LoadOutlineFontTexture(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//	//	�G���[�`�F�b�N
//	if (m_CacheDescOL.textureCacheList[useNum].GetTexture() == NULL)	return	-1;
//
//	//	�ƍ��p�p�����[�^���X�V
//	RefDataOL&	refData = m_CacheDescOL.referenceDataList[useNum];
//	refData.chara[0] = _Character[0];
//	refData.chara[1] = _Character[1];
//	refData.size = _FontSize;
//	refData.byte = byteSize;
//	refData.isEnable = true;
//	refData.fontName = _FontName;
//
//	refData.fontWeight = _FontWeight;
//	refData.edgeWidth = _EdgeWidth;
//	refData.fillColor = _FillColor;
//	refData.edgeColor = _EdgeColor;
//	refData.quality = _Quality;
//
//
//	//	���ɏ���������L���b�V���̔ԍ����X�V
//	if (++m_CacheDescOL.nextUseCacheNum == CacheQty)	m_CacheDescOL.nextUseCacheNum = 0;
//
//	//	�V�����e�N�X�`�����쐬�����ԍ���Ԃ�
//	return	useNum;
//}