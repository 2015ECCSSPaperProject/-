#include	"iExtreme.h"
#include	<mbstring.h>
#include	<tchar.h>



/*********************************/
//
///		FONT
//
/********************************/

//	管理用パラメータ
bool					Font::m_IsInitialized = false;
Font::CacheDesc		Font::m_CacheDesc = { 0 };
// ↑
/***
UINT		nextUseCacheNum;		//	次に使用するキャッシュの番号
iex2DObj*		textureCacheList;		//	文字テクスチャのリスト
RefData*	referenceDataList;		//	キャッシュの照合用データリスト
***/

// アウトライン
Font::CacheDescOL Font::m_CacheDescOL = { 0 };

//	描画用パラメータ
DWORD					Font::m_DefaultFontColor = 0xFFFFFFFF;
float					Font::m_DefaultFontWidth = 25.0f;
float					Font::m_DefaultFontHeight = 25.0f;

const	UINT	CacheQty = 512;			//	キャッシュの数



// 初期化
void Font::Initialize(void)
{
	//	通常文字キャッシュの初期化
	{
		//	すでに確保されている領域がある場合は破棄
		delete[]	m_CacheDesc.textureCacheList;	m_CacheDesc.textureCacheList = NULL;//	文字テクスチャのリスト
		delete[]	m_CacheDesc.referenceDataList;	m_CacheDesc.referenceDataList = NULL;//	キャッシュの照合用データリスト

		//	キャッシュを保存する領域を確保
		m_CacheDesc.textureCacheList = new iex2DObj[CacheQty];  // 文字テクスチャのリスト
		m_CacheDesc.referenceDataList = new	RefData[CacheQty];	// キャッシュの照合用データのリスト

		//	キャッシュ分初期化
		for (UINT i = 0; i < CacheQty; i++)
		{
			m_CacheDesc.referenceDataList[i].isEnable = false;// 有効フラグをfalse
		}

		m_CacheDesc.nextUseCacheNum = 0;//	次に使用するキャッシュの番号
	}

	//	アウトライン付き文字キャッシュの初期化
	{

	//	すでに確保されている領域がある場合は破棄
	delete[]	m_CacheDescOL.textureCacheList;		m_CacheDescOL.textureCacheList = NULL;
	delete[]	m_CacheDescOL.referenceDataList;	m_CacheDescOL.referenceDataList = NULL;

	//	キャッシュを保存する領域を確保
	m_CacheDescOL.textureCacheList = new	iex2DObj[CacheQty];
	m_CacheDescOL.referenceDataList = new	RefDataOL[CacheQty];

	//	初期化
	for (UINT i = 0; i < CacheQty; i++){
		m_CacheDescOL.referenceDataList[i].isEnable = false;
	}

	m_CacheDescOL.nextUseCacheNum = 0;
}

	//	初期化フラグを立てる
	m_IsInitialized = true;

}

void Font::Release(void)
{

	//	通常キャッシュ
	{
		delete[]	m_CacheDesc.textureCacheList;	m_CacheDesc.textureCacheList = NULL;
		delete[]	m_CacheDesc.referenceDataList;	m_CacheDesc.referenceDataList = NULL;
		m_CacheDesc.nextUseCacheNum = 0;
	}
	
	//	アウトライン付き文字キャッシュ
	{
	delete[]	m_CacheDescOL.textureCacheList;		m_CacheDescOL.textureCacheList = NULL;
	delete[]	m_CacheDescOL.referenceDataList;	m_CacheDescOL.referenceDataList = NULL;
	m_CacheDescOL.nextUseCacheNum = 0;
	}


	//	初期化フラグをリセット
	m_IsInitialized = false;
}


//	▲文字列描画　幅と高さと色を考慮しないバージョン
void	Font::DrawString2D(LPCSTR _String, LPCSTR _FontName, float _DrawX, float _DrawY, float _Depth, DWORD _iexRenderStateFlag)
{
	//	描画関数の呼び出し
	DrawString2D(_String, _FontName, _DrawX, _DrawY, m_DefaultFontWidth, m_DefaultFontHeight, 0.0f, m_DefaultFontColor, _Depth, _iexRenderStateFlag);
}

//　■↑の本物
void	Font::DrawString2D(LPCSTR _String, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	変数の用意
	float	x = _DrawX;
	float	y = _DrawY;
	float	halfWidth = _Width  * 0.5f;
	float	halfHeight = _Height * 0.5f;
	float	centerX = _DrawX + _Width  * 0.5f * strlen(_String) * 0.5f;
	float	centerY = _DrawY + _Height * 0.5f;
	UINT	myByte = 0;
	//	終端文字までループ
	for (UINT i = 0; _String[i] != '\0'; i += myByte){
		//	文字のバイト数を調べる
		myByte = _mbclen((BYTE*)&_String[i]);

		//	空白はスキップ
		bool	skip(false);
		{
			if (myByte == 1){
				if (_String[i] == ' ')				skip = true;
			}
			else	if (myByte == 2){
				char	buffer[3] = { _String[i], _String[i + 1], '\0' };
				if (strcmp(buffer, "　") == 0)	skip = true;
			}
		}

		//	文字を描画
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
		//	描画座標をずらす
		x += halfWidth * myByte;
	}
}

//	文字列描画（無制限）
void	Font::DrawString2Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _Depth, DWORD _iexRenderStateFlag)
{
	//	描画関数の呼び出し↓
	DrawString2Dc(_String, _FontName, _FontSize, _DrawX, _DrawY, 1.0f, 1.0f, 0xFFFFFFFF, _Depth, _iexRenderStateFlag);

}

// 無制限はここにこい
void	Font::DrawString2Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	変数の用意
	float	x = _DrawX;
	float	y = _DrawY;
	UINT	myByte = 0;
	//	終端文字までループ
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	文字のバイト数を調べる	
		myByte = _mbclen((BYTE*)&_String[i]);
		
		//	文字を描画
		float	drawCoordX = x;
		float	drawCoordY = y;
		Vector2	drawedSize = DrawCharacter2Dc(&_String[i], _FontName, _FontSize, drawCoordX, drawCoordY, _ScaleX, _ScaleY, 0.0f, _Color, _Depth, _iexRenderStateFlag);

		//	描画座標をずらす
		x += drawedSize.x;
	}
}

void Font::RenderFont2D(LPCSTR _String, int _FontSize, float _DrawX, float _DrawY, DWORD col)
{

		LPSTR	c_FontName = "HGS創英角ﾎﾟｯﾌﾟ体";
		int Quality = 5;
	
		DrawString2Dc(_String, c_FontName, _FontSize,
			_DrawX, _DrawY, 1.0f, 1.0f, col, 0.0f, RS_COPY);
}


// 無制限はここにこい
void	Font::DrawString3Dc(LPCSTR _String, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	変数の用意
	float	x = _DrawX;
	float	y = _DrawY;
	float	z = _Depth;
	UINT	myByte = 0;
	//	終端文字までループ
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	文字のバイト数を調べる	
		myByte = _mbclen((BYTE*)&_String[i]);

		//	文字を描画
		float	drawCoordX = x;
		float	drawCoordY = y;
		Vector2	drawedSize = DrawCharacter3Dc(&_String[i], _FontName, _FontSize, drawCoordX, drawCoordY, _ScaleX, _ScaleY, 0.0f, _Color, z, _iexRenderStateFlag);

		//	描画座標をずらす

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
	LPSTR	c_FontName = "HGS創英角ﾎﾟｯﾌﾟ体";
	int Quality = 5;

	std::string str = _String;
	int len = str.length();
	//_Pos.x -= (len*.5f) * _FontSize;
	_Pos -= Vector3(matView._11, matView._21, matView._31) * (len*.5f);

	DrawString3Dc(_String, c_FontName, _FontSize,
		_Pos.x, _Pos.y, 1.0f, 1.0f, col, _Pos.z, RS_COPY);
}

// 文字描画
//void Font::RnederFont(LPCSTR string, float x, float y, int fontSize)
//{
//
//	LPSTR	c_FontName = "HGS創英角ﾎﾟｯﾌﾟ体";
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
//	LPSTR	c_FontName = "HGS創英角ﾎﾟｯﾌﾟ体";
//	int Quality = 5;
//
//	DrawOutlineString3D(string, c_FontName, fontSize,
//		120, 4, 0xffffffff, 0xff020000,
//		Quality, pos.x, pos.y, 1.0f, 1.0f, 0xffffffff, pos.z, RS_COPY_NOZ);
//}

//	アウトライン付き文字
//void	Font::DrawOutlineString2D(LPCSTR _String, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	変数の用意
//	float	x = _DrawX;
//	float	y = _DrawY;
//	UINT	myByte = 0;
//	//	終端文字までループ
//	for (UINT i = 0; _String[i] != '\0'; i += myByte){
//		//	文字のバイト数を調べる
//		myByte = _mbclen((BYTE*)&_String[i]);
//
//		//	描画座標計算
//		float	drawCoordX = x;
//		float	drawCoordY = y;
//
//		if (_String[i] == 'っ')
//		{
//			_DrawY += 32;
//		}
//
//		//	描画
//		Vector2	charaSize = DrawOutlineCharacter2D(_FontName, &_String[i], _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality, drawCoordX, drawCoordY, _ScaleX, _ScaleY, _Color, _Depth, _iexRenderStateFlag);
//
//		//	描画座標をずらす
//		x += charaSize.x;
//	}
//}

//	アウトライン付き文字3D
//void	Font::DrawOutlineString3D(LPCSTR _String, LPCSTR _FontName, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	変数の用意
//	float	x = _DrawX;
//	float	y = _DrawY;
//	float	z = _Depth;
//	UINT	myByte = 0;
//	//	終端文字までループ
//	for (UINT i = 0; _String[i] != '\0'; i += myByte){
//		//	文字のバイト数を調べる
//		myByte = _mbclen((BYTE*)&_String[i]);
//
//		//	描画座標計算
//		float	drawCoordX = x;
//		float	drawCoordY = y;
//
//		if (_String[i] == 'っ')
//		{
//			_DrawY += 30;
//		}
//		
//		//	描画
//		Vector2	charaSize = DrawOutlineCharacter3D(_FontName, &_String[i], _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality, drawCoordX, drawCoordY, _ScaleX, _ScaleY, _Color, z, _iexRenderStateFlag);
//
//		//	描画座標をずらす
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
///			★Char描画
//
/******************************************************/

// Char描画　シェーダーなし版
void	Font::DrawCharacter2D(LPCSTR _Character, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	文字テクスチャ管理パラメータが初期化されているかチェック
	if (!m_IsInitialized)	Initialize();

	//	変数の用意
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);
	//	文字のバイト数に応じて幅を調整
	_Width = _Width * 0.5f * byteSize;
	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
	if (_Width > _Height)	createSize = (UINT)_Width;
	else					createSize = (UINT)_Height;

	//	使用するテクスチャのアドレスをセット
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	描画
	//pImage->Render(_DrawX, _DrawY, _Width, _Height, 0, 0, _Width, _Height, _iexRenderStateFlag, _Color, _Depth);
	pImage->Render((int)_DrawX, (int)_DrawY, (int)_Width, (int)_Height, 0, 0, (int)_Width, (int)_Height, _iexRenderStateFlag);
}

// Char　シェーダーあり版
void	Font::DrawCharacter2D(LPCSTR _Character, LPCSTR _FontName, float _DrawX, float _DrawY, float _Width, float _Height, float _Angle, DWORD _Color, float _Depth, iexShader* _pShader, LPSTR _Technique)
{
	//	文字テクスチャ管理パラメータが初期化されているかチェック
	if (!m_IsInitialized)	Initialize();

	//	変数の用意
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);
	//	文字のバイト数に応じて幅を調整
	_Width = _Width * 0.5f * byteSize;
	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
	if (_Width > _Height)	createSize = (UINT)_Width;
	else					createSize = (UINT)_Height;

	//	使用するテクスチャのアドレスをセット
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	描画
	//pImage->Render(_DrawX, _DrawY, _Width, _Height, 0, 0, _Width, _Height, _pShader, _Technique, _Color, _Depth);
	//pImage->Draw2Ds(_DrawX, _DrawY, _Width, _Height, _Angle, _Color, _Depth, _pShader, _Technique);
	pImage->Render((int)_DrawX, (int)_DrawY, (int)_Width, (int)_Height, 0, 0, (int)_Width, (int)_Height, _pShader, _Technique);
}

Vector2	Font::DrawCharacter2Dc(LPCSTR _Character, LPCSTR _FontName, int _FontSize, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{
	//	文字テクスチャ管理パラメータが初期化されているかチェック
	if (!m_IsInitialized)	Initialize();

	//	変数の用意
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);

	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
	int		createSizeW = (int)((float)_FontSize * _ScaleX);
	int		createSizeH = (int)((float)_FontSize * _ScaleY);
	if (createSizeW > createSizeH)	createSize = createSizeW;
	else							createSize = createSizeH;

	//	使用するテクスチャのアドレスをセット
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	空白チェック
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
				 if (strcmp(buffer, "　") == 0){
					 draw = false;
				 }
	}
		break;
	}

	//	描画
	if (draw){
		//pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
		//	0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
		//	pImage->Draw2D(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _Angle, _Color, _Depth, _iexRenderStateFlag);
		pImage->SetARGB(_Color);
		pImage->Render((int)_DrawX, (int)_DrawY, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY),
			0, 0, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY), _iexRenderStateFlag);
	}

	//	描画した文字のサイズを返す
	return	Vector2(pImage->GetWidth(), pImage->GetHeight());
}

//3D
Vector2	Font::DrawCharacter3Dc(LPCSTR _Character, LPCSTR _FontName, int _FontSize, float _DrawX,
	float _DrawY, float _ScaleX, float _ScaleY, float _Angle, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
{

	//	文字テクスチャ管理パラメータが初期化されているかチェック
	if (!m_IsInitialized)	Initialize();

	//	変数の用意
	iex2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)_Character);

	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
	int		createSizeW = (int)((float)_FontSize * _ScaleX);
	int		createSizeH = (int)((float)_FontSize * _ScaleY);
	if (createSizeW > createSizeH)	createSize = createSizeW;
	else							createSize = createSizeH;

	//	使用するテクスチャのアドレスをセット
	pImage = &m_CacheDesc.textureCacheList[SearchCache(_Character, createSize, _FontName)];

	//	空白チェック
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
				 if (strcmp(buffer, "　") == 0){
					 draw = false;
				 }
	}
		break;
	}

	//	描画
	if (draw){
		//pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
		//	0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
		//	pImage->Draw2D(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _Angle, _Color, _Depth, _iexRenderStateFlag);
		pImage->SetARGB(_Color);
		//pImage->SetScale(10);
		pImage->Render3D(_DrawX, _DrawY, _Depth, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY),
			0, 0, (int)(pImage->GetWidth() * _ScaleX), (int)(pImage->GetHeight() * _ScaleY), _iexRenderStateFlag);
	}

	//	描画した文字のサイズを返す
	return	Vector2(pImage->GetWidth(), pImage->GetHeight());



}




//	アウトライン付き
//Vector2	Font::DrawOutlineCharacter2D(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	文字テクスチャ管理パラメータが初期化されているかチェック
//	if (!m_IsInitialized)	Initialize();
//
//	//	変数の用意
//	iex2DObj*	pImage;
//	UINT	createSize;
//	UINT	byteSize = _mbclen((BYTE*)_Character);
//
//	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
//	int		createSizeW = (int)((float)_FontSize * _ScaleX);
//	int		createSizeH = (int)((float)_FontSize * _ScaleY);
//	if (createSizeW > createSizeH)	createSize = createSizeW;
//	else							createSize = createSizeH;
//
//	//	キャッシュ番号を取得
//	int		cacheNum = SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//
//	//	使用するテクスチャのアドレスをセット
//	pImage = &m_CacheDescOL.textureCacheList[SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality)];
//
//	//	空白チェック
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
//				 if (strcmp(buffer, "　") == 0){
//					 draw = false;
//				 }
//	}
//		break;
//	}
//
//	//	描画
//	if (draw){
//		//	描画位置計算
//		Vector2			charaSize = GetCharacterSizeOL(_Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		TEXTMETRIC		textMatric;
//		GLYPHMETRICS	glyphMetrics;
//		GetMatric(&textMatric, &glyphMetrics, _Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		float			upper = (float)(textMatric.tmAscent - glyphMetrics.gmptGlyphOrigin.y);
//
//		//	描画
//		pImage->Render(_DrawX, _DrawY, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY,
//			0, 0, pImage->GetWidth() * _ScaleX, pImage->GetHeight() * _ScaleY, _iexRenderStateFlag, _Color, _Depth);
//	}
//
//	//	描画した文字のサイズを返す
//	return	Vector2((float)pImage->GetWidth(), (float)pImage->GetHeight());
//}



//	アウトライン付き3D
//Vector2	Font::DrawOutlineCharacter3D(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality, float _DrawX, float _DrawY, float _ScaleX, float _ScaleY, DWORD _Color, float _Depth, DWORD _iexRenderStateFlag)
//{
//	//	文字テクスチャ管理パラメータが初期化されているかチェック
//	if (!m_IsInitialized)	Initialize();
//
//	//	変数の用意
//	iex2DObj*	pImage;
//	UINT	createSize;
//	UINT	byteSize = _mbclen((BYTE*)_Character);
//
//	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
//	int		createSizeW = (int)((float)_FontSize * _ScaleX);
//	int		createSizeH = (int)((float)_FontSize * _ScaleY);
//	if (createSizeW > createSizeH)	createSize = createSizeW;
//	else							createSize = createSizeH;
//
//	//	キャッシュ番号を取得
//	int		cacheNum = SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//
//	//	使用するテクスチャのアドレスをセット
//	pImage = &m_CacheDescOL.textureCacheList[SearchCacheOL(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality)];
//
//	//	空白チェック
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
//				 if (strcmp(buffer, "　") == 0){
//					 draw = false;
//				 }
//	}
//		break;
//	}
//
//	//	描画
//	if (draw){
//		//	描画位置計算
//		Vector2			charaSize = GetCharacterSizeOL(_Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		TEXTMETRIC		textMatric;
//		GLYPHMETRICS	glyphMetrics;
//		GetMatric(&textMatric, &glyphMetrics, _Character, _FontName, _FontSize, _FontWeight, _EdgeWidth, _Quality);
//		float			upper = (float)(textMatric.tmAscent - glyphMetrics.gmptGlyphOrigin.y);
//
//		//	描画
//		Vector3 POS;
//		POS.x = _DrawX;
//		POS.y = _DrawY;
//		POS.z = _Depth;
//		pImage->Render3D(POS, _iexRenderStateFlag);
//	}
//
//	//	描画した文字のサイズを返す
//	return	Vector2((float)pImage->GetWidth(), (float)pImage->GetHeight());
//}

//****************************************************************************************************************************************************************
//		情報の取得
//****************************************************************************************************************************************************************

//	指定されたフォント、文字サイズで生成される文字列のサイズを取得
Vector2	Font::GetStringSize(LPCSTR _String, LPCSTR _FontName, int _FontSize)
{
	Vector2	strSize(0, _FontSize);

	UINT	myByte = 0;

	//	終端文字までループ
	for (UINT i = 0; _String[i] != '\0'; i += myByte)
	{
		//	文字のバイト数を調べる
		myByte = _mbclen((BYTE*)&_String[i]);

		//	サイズを調べる
		Vector2	charSize = GetCharacterSize(&_String[i], _FontName, _FontSize);
		strSize.x += charSize.x;
	}

	return	strSize;

}

// 一文字フォント、サイズで
Vector2	Font::GetCharacterSize(LPCSTR _Character, LPCSTR _FontName, int _FontSize)
{
	//	フォントの生成
	LOGFONT	lf = { _FontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, (FIXED_PITCH | FF_MODERN), "ＭＳ 明朝" };
	strcpy_s(lf.lfFaceName, _FontName);
	HFONT	hFont;
	if (!(hFont = CreateFontIndirect(&lf)))	return	Vector2(0, 0);

	//	デバイスコンテキスト取得
	HDC		hdc = GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hdc, hFont);

	//	文字コード取得
	TCHAR *c = _T((LPSTR)_Character);
	UINT code = 0;
	if (IsDBCSLeadByte(*c))	code = (BYTE)c[0] << 8 | (BYTE)c[1];
	else						code = c[0];

	//	フォントビットマップ取得
	TEXTMETRIC	TM;
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS	GM;
	CONST MAT2	Mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	DWORD		size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);

	//	デバイスコンテキストとフォントハンドルの開放
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
//	// デバイスコンテキスト作成
//	HDC	 hDC = GetDC(hWnd);
//	HDC	 memDC = CreateCompatibleDC(hDC);
//	ReleaseDC(hWnd, hDC);   // 解放していいよ
//
//	// フォント作成
//	LOGFONTA lf = {};
//	lf.lfHeight = fontHeight * quality;    // 拡大サイズで
//	lf.lfCharSet = charSet;
//	lf.lfWeight = weight;
//	lf.lfQuality = ANTIALIASED_QUALITY;
//	memcpy(lf.lfFaceName, faceName, strlen(faceName));
//	HFONT	 hFont = CreateFontIndirectA(&lf);
//	HFONT	 oldFont = (HFONT)SelectObject(memDC, hFont);     // フォントを設定しないとエラーになります
//
//	//	でっかい"ま"を描画するBMPを作成
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
//	// デバイスコンテキスト作成
//	HDC	 hDC = GetDC(hWnd);
//	HDC	 memDC = CreateCompatibleDC(hDC);
//	ReleaseDC(hWnd, hDC);   // 解放していいよ
//
//	// フォント作成
//	LOGFONTA lf = {};
//	lf.lfHeight = fontHeight;
//	lf.lfCharSet = charSet;
//	lf.lfWeight = weight;
//	lf.lfQuality = ANTIALIASED_QUALITY;
//	memcpy(lf.lfFaceName, faceName, strlen(faceName));
//	HFONT	 hFont = CreateFontIndirectA(&lf);
//	HFONT	 oldFont = (HFONT)SelectObject(memDC, hFont);     // フォントを設定しないとエラーになります
//
//	//	でっかい"ま"を描画するBMPを作成
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
//		サポート関数
//****************************************************************************************************************************************************************
//	キャッシュの検索＆作成
UINT	Font::SearchCache(LPCSTR chara, UINT size, LPCSTR _FontName)
{
	//	使用可能なキャッシュを探す
	UINT	sizeAllow = 0;							//	許容サイズ差
	UINT	byteSize = _mbclen((BYTE*)chara);	//	バイト数
	//	キャッシュ数分ループ
	for (UINT cNum = 0; cNum < CacheQty; cNum++){
		RefData&	refData = m_CacheDesc.referenceDataList[cNum];
		if (!refData.isEnable)									continue;
		//	バイト数判定
		if (refData.byte != byteSize)							continue;
		//	文字判定（バイト数に応じて分岐）
		if (byteSize != 2){
			if (refData.chara[0] != chara[0])					continue;
		}
		else{
			if (refData.chara[0] != chara[0])					continue;
			if (refData.chara[1] != chara[1])					continue;
		}
		//	サイズ判定
		if (refData.size - sizeAllow > size)					continue;
		if (refData.size + sizeAllow < size)					continue;
		//	フォント判定
		if (strlen(refData.fontName) == 0){
			if (strlen(_FontName) != 0)						continue;
		}
		else{
			if (strlen(_FontName) == 0)						continue;
			if (strcmp(refData.fontName, _FontName) != 0)	continue;
		}
		//	判定をクリア
		return	cNum;
	}

	//	新しく文字テクスチャを作成
	UINT	useNum = m_CacheDesc.nextUseCacheNum;
	m_CacheDesc.textureCacheList[useNum].LoadFontTexture(chara, size, _FontName);
	//	照合用パラメータを更新
	RefData&	refData = m_CacheDesc.referenceDataList[useNum];
	refData.chara[0] = chara[0];
	refData.chara[1] = chara[1];
	refData.size = size;
	refData.byte = byteSize;
	refData.isEnable = true;
	refData.fontName = _FontName;

	//	次に書き換えるキャッシュの番号を更新
	if (++m_CacheDesc.nextUseCacheNum == CacheQty)	m_CacheDesc.nextUseCacheNum = 0;

	//	新しくテクスチャを作成した番号を返す
	return	useNum;
}


// アウトライン用

//	キャッシュの検索＆作成（アウトライン付きフォント用）
//UINT		Font::SearchCacheOL(LPCSTR _FontName, LPCSTR _Character, UINT _FontSize, UINT _FontWeight, int _EdgeWidth, DWORD _FillColor, DWORD _EdgeColor, int _Quality)
//{
//	//	使用可能なキャッシュを探す
//	UINT	sizeAllow = 0;								//	許容サイズ差
//	UINT	byteSize = _mbclen((BYTE*)_Character);		//	バイト数
//	//	キャッシュ数分ループ
//	for (UINT cNum = 0; cNum < CacheQty; cNum++){
//		RefDataOL&	refData = m_CacheDescOL.referenceDataList[cNum];
//		if (!refData.isEnable)									continue;
//		//	バイト数判定
//		if (refData.byte != byteSize)							continue;
//		//	文字判定（バイト数に応じて分岐）
//		if (byteSize != 2){
//			if (refData.chara[0] != _Character[0])				continue;
//		}
//		else{
//			if (refData.chara[0] != _Character[0])				continue;
//			if (refData.chara[1] != _Character[1])				continue;
//		}
//		//	サイズ判定
//		if (refData.size - sizeAllow > _FontSize)				continue;
//		if (refData.size + sizeAllow < _FontSize)				continue;
//		//	フォント判定
//		if (strlen(refData.fontName) == 0){
//			if (strlen(_FontName) != 0)						continue;
//		}
//		else{
//			if (strlen(_FontName) == 0)						continue;
//			if (strcmp(refData.fontName, _FontName) != 0)	continue;
//		}
//		//	フォントの幅（？）判定
//		if (refData.fontWeight != _FontWeight)					continue;
//		//	アウトラインの幅判定
//		if (refData.edgeWidth != _EdgeWidth)					continue;
//		//	塗りの色（文字の色）判定
//		if (refData.fillColor != _FillColor)					continue;
//		//	アウトラインの色判定
//		if (refData.edgeColor != _EdgeColor)					continue;
//		//	品質判定
//		if (refData.quality != _Quality)						continue;
//
//		//	判定をクリア
//		return	cNum;
//	}
//
//	//	新しく文字テクスチャを作成
//	UINT	useNum = m_CacheDescOL.nextUseCacheNum;
//	m_CacheDescOL.textureCacheList[useNum].LoadOutlineFontTexture(_FontName, _Character, _FontSize, _FontWeight, _EdgeWidth, _FillColor, _EdgeColor, _Quality);
//	//	エラーチェック
//	if (m_CacheDescOL.textureCacheList[useNum].GetTexture() == NULL)	return	-1;
//
//	//	照合用パラメータを更新
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
//	//	次に書き換えるキャッシュの番号を更新
//	if (++m_CacheDescOL.nextUseCacheNum == CacheQty)	m_CacheDescOL.nextUseCacheNum = 0;
//
//	//	新しくテクスチャを作成した番号を返す
//	return	useNum;
//}