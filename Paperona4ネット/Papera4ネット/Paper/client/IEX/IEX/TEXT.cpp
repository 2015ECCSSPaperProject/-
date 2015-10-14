#include	"iExtreme.h"

LPD3DXFONT Text::font;

void Text::Init(){
	HFONT hf;
	D3DXFONT_DESC fd;

	hf = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(hf, sizeof(LOGFONT), (LPSTR)&fd);
	fd.Height = 64;
	fd.Width = 0;
	fd.Italic = 0;
	fd.CharSet = SHIFTJIS_CHARSET;
	strcpy(fd.FaceName, "ƒƒCƒŠƒI");

	D3DXCreateFontIndirect(iexSystem::GetDevice(), &fd, &font);
}

void Text::CleanUpModule(){
	font->Release();
}

void Text::Draw(int x, int y, DWORD color, const char * _Format, ...){
	const int strsize = 1024;
	char str[strsize];
	ZeroMemory(str, sizeof(char)* strsize);
	va_list list;
	va_start(list, _Format);
	vsprintf(str, _Format, list);
	va_end(list);

	RECT rec;
	rec.left = x;
	rec.top = y;
	rec.right = iexSystem::ScreenWidth;
	rec.bottom = iexSystem::ScreenHeight;

	font->DrawText(NULL, str, -1, &rec, DT_LEFT | DT_WORDBREAK, color);
	return;
}
