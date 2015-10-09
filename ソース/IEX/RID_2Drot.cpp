#include	"iextreme.h"
#include	"system/system.h"
#include	"RID_2Drot.h"

void RID_2DObj::rot_render(float angle)
{
	float srad = sin(angle), crad = cos(angle);
	float sentx, senty;
	float newx, newy;

	TLVERTEX	v[4];
	//	ëSâÊñ ê›íË
	v[0].sx = v[2].sx = 0;
	v[1].sx = v[3].sx = iexSystem::ScreenWidth - 0.5f;
	v[0].sy = v[1].sy = 0;
	v[2].sy = v[3].sy = iexSystem::ScreenHeight - 0.5f;

	sentx = v[1].sx * 0.5f;
	senty = v[2].sy * 0.5f;

	for (int num = 0; num < 4; num++)
	{
		v[num].sx -= sentx, v[num].sy -= senty;

		newx = v[num].sx * crad - v[num].sy * srad;
		newy = v[num].sx * srad + v[num].sy * crad;

		v[num].sx = newx + sentx, v[num].sy = newy + senty;
	}

	v[0].tu = v[2].tu = 0;
	v[1].tu = v[3].tu = 1;
	v[0].tv = v[1].tv = 0;
	v[2].tv = v[3].tv = 1;

	v[0].color = v[1].color = v[2].color = v[3].color = 0xFFFFFFFF;
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	iexPolygon::Render2D(v, 2, this, RS_COPY);
}

void RID_2DObj::rot_render(float angle, iexShader* shader, char* tech)
{
	float srad = sin(angle), crad = cos(angle);
	float sentx, senty;
	float newx, newy;

	TLVERTEX	v[4];
	//	ëSâÊñ ê›íË
	v[0].sx = v[2].sx = 0;
	v[1].sx = v[3].sx = iexSystem::ScreenWidth - 0.5f;
	v[0].sy = v[1].sy = 0;
	v[2].sy = v[3].sy = iexSystem::ScreenHeight - 0.5f;

	sentx = v[1].sx * 0.5f;
	senty = v[2].sy * 0.5f;

	for (int num = 0; num < 4; num++)
	{
		v[num].sx -= sentx, v[num].sy -= senty;

		newx = v[num].sx * crad - v[num].sy * srad;
		newy = v[num].sx * srad + v[num].sy * crad;

		v[num].sx = newx + sentx, v[num].sy = newy + senty;
	}

	v[0].tu = v[2].tu = 0;
	v[1].tu = v[3].tu = 1;
	v[0].tv = v[1].tv = 0;
	v[2].tv = v[3].tv = 1;

	v[0].color = v[1].color = v[2].color = v[3].color = 0xFFFFFFFF;
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	iexPolygon::Render2D(v, 2, this, shader, tech);
}

void RID_2DObj::rot_render(float angle, s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags, COLOR color, float z)
{
	float srad = sin(angle), crad = cos(angle);
	float sentx, senty;
	float newx, newy;

	TLVERTEX	v[4];
	v[0].sx = v[2].sx = (float)DstX;
	v[1].sx = v[3].sx = (float)(DstX + DstW) - 0.5f;
	v[0].sy = v[1].sy = (float)DstY;
	v[2].sy = v[3].sy = (float)(DstY + DstH) - 0.5f;

	sentx = (v[0].sx + v[1].sx) * 0.5f;
	senty = (v[0].sy + v[2].sy) * 0.5f;

	for (int num = 0; num < 4; num++)
	{
		v[num].sx -= sentx, v[num].sy -= senty;

		newx = v[num].sx * crad - v[num].sy * srad;
		newy = v[num].sx * srad + v[num].sy * crad;

		v[num].sx = newx + sentx, v[num].sy = newy + senty;
	}

	v[0].tu = v[2].tu = (float)(SrcX + 0.5f) / (float)this->width;
	v[1].tu = v[3].tu = (float)(SrcX + width) / (float)this->width;
	v[0].tv = v[1].tv = (float)(SrcY + 0.5f) / (float)this->height;
	v[2].tv = v[3].tv = (float)(SrcY + height) / (float)this->height;

	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = z;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	iexPolygon::Render2D(v, 2, this, dwFlags);
}

void RID_2DObj::rot_render(float angle, s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, iexShader* shader, char* tech, COLOR color, float z)
{
	float srad = sin(angle), crad = cos(angle);
	float sentx, senty;
	float newx, newy;

	TLVERTEX	v[4];
	v[0].sx = v[2].sx = (float)DstX;
	v[1].sx = v[3].sx = (float)(DstX + DstW) - 0.5f;
	v[0].sy = v[1].sy = (float)DstY;
	v[2].sy = v[3].sy = (float)(DstY + DstH) - 0.5f;

	sentx = (v[0].sx + v[1].sx) * 0.5f;
	senty = (v[0].sy + v[2].sy) * 0.5f;

	for (int num = 0; num < 4; num++)
	{
		v[num].sx -= sentx, v[num].sy -= senty;

		newx = v[num].sx * crad - v[num].sy * srad;
		newy = v[num].sx * srad + v[num].sy * crad;

		v[num].sx = newx + sentx, v[num].sy = newy + senty;
	}

	v[0].tu = v[2].tu = (float)(SrcX + 0.5f) / (float)this->width;
	v[1].tu = v[3].tu = (float)(SrcX + width) / (float)this->width;
	v[0].tv = v[1].tv = (float)(SrcY + 0.5f) / (float)this->height;
	v[2].tv = v[3].tv = (float)(SrcY + height) / (float)this->height;

	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = z;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	iexPolygon::Render2D(v, 2, this, shader, tech);
}





RID_2Drot::RID_2Drot()
{
	view = NULL;
	turn = NULL;
	angle = 0;
}

RID_2Drot::~RID_2Drot()
{
	if (view)
		delete view;

	if (turn)
		delete turn;
}


bool RID_2Drot::Initialize()
{
	//	ä¬ã´ê›íË
	iexLight::SetFog(800, 1000, 0);
	view = new iexView();
	turn = new RID_2DObj("DATA/2dturn.png");

	return false;
}

void RID_2Drot::Update()
{
	angle += 0.05f;

	if (angle > PI + PI)
		angle -= PI + PI;
}

void RID_2Drot::Render()
{
	view->Clear(0x808080);
	view->Activate();

	turn->rot_render(angle, 280, 0, 720, 720, 0, 0, 1828, 1510);
}

