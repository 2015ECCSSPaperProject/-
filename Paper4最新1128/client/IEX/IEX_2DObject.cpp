#include	"iExtreme.h"
#include	<tchar.h>

// 画像の補間への対処のためのサイズ調整
// テクスチャ上のテクセルはそのど真ん中が切りの良い整数座標になっているため、
//「0.5」というのはテクセルとテクセルの調度境目に相当する位置になる
static const float abjustSize = 0.5f;

//*****************************************************************************
//
//		２Ｄオブジェクト
//
//*****************************************************************************
//------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------
iex2DObj::iex2DObj(){
	//	情報初期化
	width = height = 0;
	lpTexture = NULL;
	lpSurface = NULL;

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;
};

iex2DObj::iex2DObj( char* filename )
{
	//	情報初期化
	width = height = 0;
	lpSurface = NULL;
	
	//	テクスチャ読み込み
	lpTexture = iexTexture::Load(filename);
	if( lpTexture == NULL ) return;

	//	サイズ保存
	D3DSURFACE_DESC	sd;
	lpTexture->GetLevelDesc( 0, &sd );
	width  = sd.Width;
	height = sd.Height;

	//	メインサーフェス取得
	lpTexture->GetSurfaceLevel( 0, &lpSurface );

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;

	// 頂点情報初期化
	//v[0].color = v[1].color = v[2].color = v[3].color = color;
	//v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0;
	//v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?
}

iex2DObj::iex2DObj( u32 width, u32 height, u8 flag )
{
	//	情報初期化
	this->width = this->height = 0;
	lpSurface = NULL;

	D3DFORMAT	fmt = D3DFMT_R8G8B8;
	u32			usage = 0;
	D3DPOOL		pool = D3DPOOL_MANAGED;

	switch( flag ){
	case IEX2D_RENDERTARGET:	//	レンダー可能
					usage = D3DUSAGE_RENDERTARGET;
					fmt = iexSystem::ScreenFormat;
					pool = D3DPOOL_DEFAULT;
					break;
	case IEX2D_FLOAT2:	//	レンダー可能
					usage = D3DUSAGE_RENDERTARGET;
					fmt = D3DFMT_G32R32F;
					pool = D3DPOOL_DEFAULT;
					break;
	case IEX2D_FLOAT:	//	レンダー可能
					usage = D3DUSAGE_RENDERTARGET;
					fmt = D3DFMT_R32F;
					pool = D3DPOOL_DEFAULT;
					break;

	case IEX2D_USEALPHA:		//	透明度付もレンダー可能
					usage = D3DUSAGE_RENDERTARGET;
					fmt = D3DFMT_A8R8G8B8;
					pool = D3DPOOL_DEFAULT;
					break;
	case IEX2D_SYSTEMMEM: //iexSystem::ScreenFormatからダウンサンプリング用に
		usage = D3DUSAGE_DYNAMIC;	//←読み込み専用
					fmt = D3DFMT_R32F;
					pool = D3DPOOL_SYSTEMMEM;
					break;
	case IEX2D_HDR:	//	HDRレンダー用
					usage = D3DUSAGE_RENDERTARGET;
					fmt = D3DFMT_A16B16G16R16F;
					pool = D3DPOOL_DEFAULT;
					break;
	case IEX2D_R16_SYSTEMMEM: //iexSystem::ScreenFormatからダウンサンプリング用に
		usage = D3DUSAGE_DYNAMIC;	//←多分これで書き込める？
		fmt = D3DFMT_R16F;
		pool = D3DPOOL_SYSTEMMEM;
		break;
	case IEX2D_R16:	//	HDRレンダー用
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_R16F;
		pool = D3DPOOL_DEFAULT;
		break;
	case IEX2D_FLOAT4_SYSTEMMEM: //iexSystem::ScreenFormatからダウンサンプリング用に
		usage = D3DUSAGE_DYNAMIC;	//←多分これで書き込める？
		fmt = D3DFMT_A32B32G32R32F;
		pool = D3DPOOL_DEFAULT;
		break;
	case IEX2D_FLOAT4:	//	
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_A32B32G32R32F;
		pool = D3DPOOL_DEFAULT;
		break;
	case IEX2D_DYNAMIC:	//	書き込み用
		usage = D3DUSAGE_DYNAMIC;
		fmt = D3DFMT_A8R8G8B8;
		pool = D3DPOOL_DEFAULT;
		break;

	}
	dwFlags= 0;
	//	テクスチャ作成
	D3DXCreateTexture( iexSystem::Device, width, height, 0, usage, fmt, pool, &lpTexture );
	if( lpTexture == NULL ) return;

	//	サイズ保存
	D3DSURFACE_DESC	sd;
	lpTexture->GetLevelDesc( 0, &sd );
	this->width  = sd.Width;
	this->height = sd.Height;
	//	メインサーフェス取得
	lpTexture->GetSurfaceLevel( 0, &lpSurface );

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;
}


//	文字テクスチャをロード
bool	iex2DObj::LoadFontTexture(LPCSTR character, UINT createSize, LPCSTR _FontName)
{
	//	現在保持しているテクスチャを破棄
	if (lpTexture != NULL){
		lpTexture->Release();
		lpTexture = NULL;
	}

	//	フォントの生成
	int	fontsize = createSize;
	LOGFONT	lf = { fontsize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, (FIXED_PITCH | FF_MODERN), "ＭＳ 明朝" };
	strcpy_s(lf.lfFaceName, _FontName);
	HFONT	hFont;
	if (!(hFont = CreateFontIndirect(&lf))){
		return FALSE;
	}

	//	デバイスコンテキスト取得
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	//	文字コード取得
	TCHAR *c = _T((LPSTR)character);
	UINT code = 0;
	if (IsDBCSLeadByte(*c))
		code = (BYTE)c[0] << 8 | (BYTE)c[1];
	else
		code = c[0];

	//	フォントビットマップ取得
	TEXTMETRIC	TM;
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS	GM;
	CONST MAT2	Mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	DWORD		size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE		*ptr = new BYTE[size];
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);

	// テクスチャ作成
	if (FAILED(iexSystem::Device->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1,
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
		&lpTexture, NULL)))
	if (FAILED(iexSystem::Device->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &lpTexture, NULL)))
	{
		delete[] ptr;	return FALSE;
	}

	//	テクスチャにフォントビットマップ書き込み
	D3DLOCKED_RECT	LockedRect;
	if (FAILED(lpTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD)))
	if (FAILED(lpTexture->LockRect(0, &LockedRect, NULL, 0)))
	{
		delete[] ptr;	return FALSE;
	}

	//	フォント情報の書き込み
	int	iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
	int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
	int iBmp_h = GM.gmBlackBoxY;
	int Level = 17;
	int	x, y;
	DWORD	Alpha, Color;
	FillMemory(LockedRect.pBits, LockedRect.Pitch * TM.tmHeight, 0);
	for (y = iOfs_y; y < (iOfs_y + iBmp_h); y++){

		for (x = iOfs_x; x < (int)(iOfs_x + GM.gmBlackBoxX); x++){

			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);

			Color = 0x00ffffff | (Alpha << 24);

			memcpy((BYTE*)LockedRect.pBits + (LockedRect.Pitch * y) + (4 * x), &Color, sizeof(DWORD));

		}

	}
	//	サーフェイスアンロック
	lpTexture->UnlockRect(0);
	delete[] ptr;

	//	パラメータをセット
	UINT	byte = _mbclen((BYTE*)character);
	width = GM.gmCellIncX;
	height = TM.tmHeight;

	return	TRUE;
}

//*****************************************************************************
//	レンダーターゲット設定
//*****************************************************************************
void	iex2DObj::RenderTarget( int index )
{
	iexSystem::Device->SetRenderTarget( index,lpSurface );
}


//*****************************************************************************
//	レンダリング
//*****************************************************************************

void iex2DObj::Render()
{
	TLVERTEX	v[4];
	//	全画面設定
	v[0].sx = v[2].sx = 0;
	v[1].sx = v[3].sx = iexSystem::ScreenWidth - 0.5f;
	v[0].sy = v[1].sy = 0;
	v[2].sy = v[3].sy = iexSystem::ScreenHeight - 0.5f;

	v[0].tu = v[2].tu = 0;
	v[1].tu = v[3].tu = 1;
	v[0].tv = v[1].tv = 0;
	v[2].tv = v[3].tv = 1;

	v[0].color = v[1].color = v[2].color = v[3].color = 0xFFFFFFFF;
	v[0].sz    = v[1].sz    = v[2].sz    = v[3].sz    = 0;
	v[0].rhw   = v[1].rhw   = v[2].rhw   = v[3].rhw   = 1.0f;

	iexPolygon::Render2D( v, 2, this, RS_COPY );
}
void iex2DObj::Render(iexShader* shader, char* tech)
{
	TLVERTEX	v[4];
	//	全画面設定
	v[0].sx = v[2].sx = 0;
	v[1].sx = v[3].sx = iexSystem::ScreenWidth - abjustSize;
	v[0].sy = v[1].sy = 0;
	v[2].sy = v[3].sy = iexSystem::ScreenHeight - abjustSize;

	v[0].tu = v[2].tu = 0;
	v[1].tu = v[3].tu = 1;
	v[0].tv = v[1].tv = 0;
	v[2].tv = v[3].tv = 1;

	v[0].color = v[1].color = v[2].color = v[3].color = 0xFFFFFFFF;
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	iexPolygon::Render2D(v, 2, this, shader, tech);
}



//
void iex2DObj::Render(int x, int y, u32 dwFlags)
{

	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// Zは0
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(width * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(width * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(height * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(height * 0.5f - centerY) * scale;


	//v[0].sx = v[2].sx = (float)x;
	//v[1].sx = v[3].sx = (float)(x + this->width) - abjustSize; //0.5fずらす
	//v[0].sy = v[1].sy = (float)y;
	//v[2].sy = v[3].sy = (float)(y + this->height) - abjustSize;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}


	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (width * 0.5f));
		transY = (float)(y + centerY + (height * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;


	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)abjustSize / (float)this->width;// ほんの少しUV座標をずらす
		
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 1.0;
		else
			 v[1].tu = v[3].tu = 1.0f - (float)abjustSize / (float)this->width;
		
	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 0.0f;//
		else
			v[1].tu = v[3].tu = (float)abjustSize / (float)this->width;//

			v[0].tu = v[2].tu = 1.0;
	}
	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)abjustSize / (float)this->height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = 1.0;
	else 
		v[2].tv = v[3].tv = 1.0f - (float)abjustSize / (float)this->height;
	

	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;

	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?

	// ↑の情報でポリゴン作成!!
	iexPolygon::Render2D(v, 2, this, dwFlags);

}

void iex2DObj::Render(int x, int y, iexShader* shader, char* name)
{

	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// Zは0
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(width * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(width * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(height * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(height * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}


	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (width * 0.5f));
		transY = (float)(y + centerY + (height * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;


	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)abjustSize / (float)this->width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 1.0;
		else
			v[1].tu = v[3].tu = 1.0f - (float)abjustSize / (float)this->width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 0.0f;//
		else
			v[1].tu = v[3].tu = (float)abjustSize / (float)this->width;//

		v[0].tu = v[2].tu = 1.0;
	}
	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)abjustSize / (float)this->height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = 1.0;
	else
		v[2].tv = v[3].tv = 1.0f - (float)abjustSize / (float)this->height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;

	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?

	// ↑の情報でポリゴン作成!!
	iexPolygon::Render2D(v, 2, this, shader, name);

}

// サイズ指定の2D描画
void iex2DObj::Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(w * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(w * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(h * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(h * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}

	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (w * 0.5f));
		transY = (float)(y + centerY + (h * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)(tx + (float)abjustSize) / ((float)this->width);// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx + tw) / (float)this->width;
		else
			v[1].tu = v[3].tu = (float)((tx + tw) - abjustSize) / (float)this->width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx) / ((float)this->width);//
		else
			v[1].tu = v[3].tu = (float)(tx + (float)abjustSize) / ((float)this->width);//

		v[0].tu = v[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->width);
	}

	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)(ty + abjustSize) / (float)this->height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = (float)((ty + th)) / (float)this->height;
	else
		v[2].tv = v[3].tv = (float)((ty + th) - abjustSize) / (float)this->height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?	
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;// Zは0

	// ↑の情報でポリゴン作成!!
	iexPolygon::Render2D(v, 2, this, dwFlags);

}

void iex2DObj::Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, iexShader* shader, char* tech)
{
	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(w * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(w * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(h * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(h * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}

	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (w * 0.5f));
		transY = (float)(y + centerY + (h * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)(tx + (float)abjustSize) / ((float)this->width);// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx + tw) / (float)this->width;
		else
			v[1].tu = v[3].tu = (float)((tx + tw) - abjustSize) / (float)this->width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx) / ((float)this->width);//
		else
			v[1].tu = v[3].tu = (float)(tx + (float)abjustSize) / ((float)this->width);//

		v[0].tu = v[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->width);
	}

	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)(ty + abjustSize) / (float)this->height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = (float)((ty + th)) / (float)this->height;
	else
		v[2].tv = v[3].tv = (float)((ty + th) - abjustSize) / (float)this->height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?	
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;// Zは0

	// ↑の情報でポリゴン作成!!
	iexPolygon::Render2D(v, 2, this, shader, tech);


}


// サイズ指定の2D描画（奥に描画）
void iex2DObj::RenderBack(int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(w * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(w * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(h * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(h * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}

	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (w * 0.5f));
		transY = (float)(y + centerY + (h * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)(tx + (float)abjustSize) / ((float)this->width);// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx + tw) / (float)this->width;
		else
			v[1].tu = v[3].tu = (float)((tx + tw) - abjustSize) / (float)this->width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx) / ((float)this->width);//
		else
			v[1].tu = v[3].tu = (float)(tx + (float)abjustSize) / ((float)this->width);//

		v[0].tu = v[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->width);
	}

	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)(ty + abjustSize) / (float)this->height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = (float)((ty + th)) / (float)this->height;
	else
		v[2].tv = v[3].tv = (float)((ty + th) - abjustSize) / (float)this->height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?	
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 1.0f;// Zを1

	// ↑の情報でポリゴン作成!!
	iexPolygon::Render2D(v, 2, this, dwFlags);

}


// 3D描画
void iex2DObj::Render3D(float x, float y, float z, u32 dwFlags)
{

	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;

	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);
	
	// 拡大率&幅
	// v[0].sx = v[2].sx = (float)(width * -0.5f - centerX) * scale;
	float maxSize;
	(width > height) ? ((float)(maxSize = (float)width)) : ((float)(maxSize = (float)height));
	float sizeX = (float)(width / maxSize)* scale;
	float sizeY = (float)(height / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	// 回転(R)
	//for (DWORD i = 0; i < 4; i++){
	//	const float xrot = v2[i].x;
	//	const float yrot = v2[i].y;
	//	const float zrot = v2[i].z;

	//	v2[i].x = xrot * cos(angle) + yrot * sin(angle);
	//	v2[i].y = -xrot * sin(angle) + yrot * cos(angle);
	//}

	// 頂点を平行移動(T)	
	v2[0].x += (float)x;
	v2[0].y += (float)y;
	v2[0].z += (float)z;
					
	v2[1].x += (float)x;
	v2[1].y += (float)y;
	v2[1].z += (float)z;
					
	v2[2].x += (float)x;
	v2[2].y += (float)y;
	v2[2].z += (float)z;
					
	v2[3].x += (float)x;
	v2[3].y += (float)y;
	v2[3].z += (float)z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)abjustSize / (float)this->width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 1.0f;
		else
			v2[1].tu = v2[3].tu = 1.0f - (float)abjustSize / (float)this->width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 0.0f;//
		else
			v2[1].tu = v2[3].tu = (float)abjustSize / (float)this->width;//

		v2[0].tu = v2[2].tu = 1.0f;
	}
	// 縦のuv座標は変わらず
	v2[0].tv = v2[1].tv = (float)abjustSize / (float)this->height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = 1.0f;
	else
		v2[2].tv = v2[3].tv = 1.0f - (float)abjustSize / (float)this->height;


	//描画
	iexPolygon::Render3D(v2, 2, this, dwFlags);

}

void iex2DObj::Render3D(Vector3 pos, u32 dwFlags)
{

	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;

	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	// v[0].sx = v[2].sx = (float)(width * -0.5f - centerX) * scale;
	float maxSize;
	(width > height) ? ((float)(maxSize = (float)width)) : ((float)(maxSize = (float)height));
	float sizeX = (float)(width / maxSize)* scale;
	float sizeY = (float)(height / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	// 回転(R)
	//for (DWORD i = 0; i < 4; i++){
	//	const float xrot = v2[i].x;
	//	const float yrot = v2[i].y;
	//	const float zrot = v2[i].z;

	//	v2[i].x = xrot * cos(angle) + yrot * sin(angle);
	//	v2[i].y = -xrot * sin(angle) + yrot * cos(angle);
	//}

	// 頂点を平行移動(T)	
	v2[0].x += (float)pos.x;
	v2[0].y += (float)pos.y;
	v2[0].z += (float)pos.z;

	v2[1].x += (float)pos.x;
	v2[1].y += (float)pos.y;
	v2[1].z += (float)pos.z;

	v2[2].x += (float)pos.x;
	v2[2].y += (float)pos.y;
	v2[2].z += (float)pos.z;

	v2[3].x += (float)pos.x;
	v2[3].y += (float)pos.y;
	v2[3].z += (float)pos.z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)abjustSize / (float)this->width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 1.0f;
		else
			v2[1].tu = v2[3].tu = 1.0f - (float)abjustSize / (float)this->width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 0.0f;//
		else
			v2[1].tu = v2[3].tu = (float)abjustSize / (float)this->width;//

		v2[0].tu = v2[2].tu = 1.0f;
	}
	// 縦のuv座標は変わらず
	v2[0].tv = v2[1].tv = (float)abjustSize / (float)this->height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = 1.0f;
	else
		v2[2].tv = v2[3].tv = 1.0f - (float)abjustSize / (float)this->height;


	//描画
	iexPolygon::Render3D(v2, 2, this, dwFlags);

}

void iex2DObj::Render3D(float x, float y, float z, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;
	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	float maxSize;
	(width > height) ? ((float)(maxSize = (float)width)) : ((float)(maxSize = (float)height));
	float sizeX = (float)(w / maxSize)* scale;
	float sizeY = (float)(h / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));


	// 頂点を平行移動(T)	
	v2[0].x += (float)x;
	v2[0].y += (float)y;
	v2[0].z += (float)z;

	v2[1].x += (float)x;
	v2[1].y += (float)y;
	v2[1].z += (float)z;

	v2[2].x += (float)x;
	v2[2].y += (float)y;
	v2[2].z += (float)z;

	v2[3].x += (float)x;
	v2[3].y += (float)y;
	v2[3].z += (float)z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)(tx + (float)abjustSize) / ((float)this->width);// ほんの少しUv2座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx + tw) / (float)this->width;
		else
			v2[1].tu = v2[3].tu = (float)((tx + tw) - abjustSize) / (float)this->width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx) / ((float)this->width);//
		else
			v2[1].tu = v2[3].tu = (float)(tx + (float)abjustSize) / ((float)this->width);//

		v2[0].tu = v2[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->width);
	}

	// 縦のuv2座標は変わらず
	v2[0].tv = v2[1].tv = (float)(ty + abjustSize) / (float)this->height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = (float)((ty + th)) / (float)this->height;
	else
		v2[2].tv = v2[3].tv = (float)((ty + th) - abjustSize) / (float)this->height;


	//描画
	iexPolygon::Render3D(v2, 2, this, dwFlags);


}

void iex2DObj::Render3D(Vector3 pos, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;
	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	float maxSize;
	(width > height) ? ((float)(maxSize = (float)width)) : ((float)(maxSize = (float)height));
	float sizeX = (float)(w / maxSize)* scale;
	float sizeY = (float)(h / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));


	// 頂点を平行移動(T)	
	v2[0].x += (float)pos.x;
	v2[0].y += (float)pos.y;
	v2[0].z += (float)pos.z;

	v2[1].x += (float)pos.x;
	v2[1].y += (float)pos.y;
	v2[1].z += (float)pos.z;
	
	v2[2].x += (float)pos.x;
	v2[2].y += (float)pos.y;
	v2[2].z += (float)pos.z;
	
	v2[3].x += (float)pos.x;
	v2[3].y += (float)pos.y;
	v2[3].z += (float)pos.z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)(tx + (float)abjustSize) / ((float)this->width);// ほんの少しUv2座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx + tw) / (float)this->width;
		else
			v2[1].tu = v2[3].tu = (float)((tx + tw) - abjustSize) / (float)this->width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx) / ((float)this->width);//
		else
			v2[1].tu = v2[3].tu = (float)(tx + (float)abjustSize) / ((float)this->width);//

		v2[0].tu = v2[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->width);
	}

	// 縦のuv2座標は変わらず
	v2[0].tv = v2[1].tv = (float)(ty + abjustSize) / (float)this->height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = (float)((ty + th)) / (float)this->height;
	else
		v2[2].tv = v2[3].tv = (float)((ty + th) - abjustSize) / (float)this->height;


	//描画
	iexPolygon::Render3D(v2, 2, this, dwFlags);




}





//-----------------------------------
///	情報更新
//-----------------------------------

// 拡大縮小
void iex2DObj::SetScale(float scale)
{
	this->scale = scale;
}

// 回転
void iex2DObj::SetAngle(float angle)
{
	this->angle = angle;
}

// 色調整 0~255まで
void iex2DObj::SetARGB(BYTE A, BYTE R, BYTE G, BYTE B)
{

	// ARGBをchar型四つ分に入れていく
	color = ARGB(A, R, G, B);
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

// 色調整(int型) 0~255まで
void iex2DObj::SetARGB(int A, int R, int G, int B)
{
	//ラムダ式Min~Maxの範囲に抑える　0~255
	auto Clamp = [](int val, int Min, int Max){
		return min(Max, max(val, Min));
	};
	
	BYTE a = Clamp(A, 0, 255);
	BYTE r = Clamp(R, 0, 255);
	BYTE g = Clamp(G, 0, 255);
	BYTE b = Clamp(B, 0, 255);

	/// ARGBをchar型四つ分に入れていく
	// DWORD color = ARGB(a, r, g, b);
	color = ARGB(a, r, g, b);
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

// 16進数での色調整
void iex2DObj::SetARGB(DWORD ARGB)
{
	color = ARGB;
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

void iex2DObj::SetTurnOver(bool turnFlag)
{

	isTurnOver = turnFlag;

}

void iex2DObj::SetShiftCenter(bool ShiftFlag)
{
	//　画像を座標の中心に持っていくか
	isShiftCenter = ShiftFlag;

}

// 回転の中心変更
void iex2DObj::SetCenter(float x, float y)
{
	centerX = x;
	centerY = y;
}