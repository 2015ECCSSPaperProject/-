#include	"iextreme.h"


void iexParticlePlus::Set2(int type, int aFrame, COLOR aColor, int eFrame, COLOR eColor, int mFrame, COLOR mColor,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate, float scale, float stretch, u8 flag, bool fWork, float Work, float reflection)
{
	PARTICLE pd;

	pd.type = type;
	pd.aFrame = aFrame;
	pd.aColor = aColor;
	pd.eFrame = eFrame;
	pd.eColor = eColor;
	pd.mFrame = mFrame;
	pd.mColor = mColor;

	pd.Pos = *Pos;
	pd.Move = *Move;
	pd.Power = *Power;
	pd.rotate = rotate;

	pd.scale = scale;
	pd.angle = angle;
	pd.stretch = stretch;

	pd.flag = flag;
	pd.isWork = fWork;
	pd.work = Work;

	pd.Reflection = reflection;

	pd.isDeceleration = false;

	Set(&pd);
}
//------------------------------------------------------
//		データ個別指定
//------------------------------------------------------

void	iexParticlePlus::Set2(int type, int aFrame, float aAlpha, int eFrame, float eAlpha, int mFrame, float mAlpha,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate,
	float r, float g, float b, float scale, float stretch, u8 flag, bool fWork, float Work, float reflection)
{
	COLOR	color;
	DWORD	aa, ea, ma;

	aa = ((DWORD)(aAlpha*255.0f) << 24);
	ea = ((DWORD)(eAlpha*255.0f) << 24);
	ma = ((DWORD)(mAlpha*255.0f) << 24);
	color = ((DWORD)(r*255.0f) << 16) | ((DWORD)(g*255.0f) << 8) | (DWORD)(b*255.0f);
	Set2(type, aFrame, aa | color, eFrame, ea | color, mFrame, ma | color, Pos, Move, Power, angle, rotate, scale, stretch, flag, fWork, Work, reflection);
}


void iexParticlePlus::Set3(int type, int aFrame, COLOR aColor, int eFrame, COLOR eColor, int mFrame, COLOR mColor,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate, float scale, float stretch, u8 flag)
{
	PARTICLE pd;

	pd.type = type;
	pd.aFrame = aFrame;
	pd.aColor = aColor;
	pd.eFrame = eFrame;
	pd.eColor = eColor;
	pd.mFrame = mFrame;
	pd.mColor = mColor;

	pd.Pos = *Pos;
	pd.Move = *Move;
	pd.Power = *Power;
	pd.rotate = rotate;
	pd.stretch = stretch;

	pd.scale = scale;
	pd.angle = angle;
	pd.stretch = stretch;

	pd.flag = flag;

	pd.isWork = false;
	pd.isDeceleration = false;


	Set(&pd);
}
//------------------------------------------------------
//		データ個別指定
//------------------------------------------------------

void	iexParticlePlus::Set3(int type, int aFrame, float aAlpha, int eFrame, float eAlpha, int mFrame, float mAlpha,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate,
	float r, float g, float b, float scale, float stretch, u8 flag)
{
	COLOR	color;
	DWORD	aa, ea, ma;

	aa = ((DWORD)(aAlpha*255.0f) << 24);
	ea = ((DWORD)(eAlpha*255.0f) << 24);
	ma = ((DWORD)(mAlpha*255.0f) << 24);
	color = ((DWORD)(r*255.0f) << 16) | ((DWORD)(g*255.0f) << 8) | (DWORD)(b*255.0f);
	Set3(type, aFrame, aa | color, eFrame, ea | color, mFrame, ma | color, Pos, Move, Power, angle, rotate, scale, stretch, flag);
}



void iexParticlePlus::Set4(int type, int aFrame, COLOR aColor, int eFrame, COLOR eColor, int mFrame, COLOR mColor,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate, float scale, float stretch, u8 flag, bool fDeceleration, float deceleration)
{
	PARTICLE pd;

	pd.type = type;
	pd.aFrame = aFrame;
	pd.aColor = aColor;
	pd.eFrame = eFrame;
	pd.eColor = eColor;
	pd.mFrame = mFrame;
	pd.mColor = mColor;

	pd.Pos = *Pos;
	pd.Move = *Move;
	pd.Power = *Power;
	pd.rotate = rotate;

	pd.scale = scale;
	pd.angle = angle;
	pd.stretch = stretch;

	pd.flag = flag;
	pd.isWork = false;

	pd.isDeceleration = fDeceleration;
	pd.deceleration = deceleration;


	Set(&pd);
}

void	iexParticlePlus::Set4(int type, int aFrame, float aAlpha, int eFrame, float eAlpha, int mFrame, float mAlpha,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate,
	float r, float g, float b, float scale, float stretch, u8 flag, bool fDeceleration, float deceleration)
{
	COLOR	color;
	DWORD	aa, ea, ma;

	aa = ((DWORD)(aAlpha*255.0f) << 24);
	ea = ((DWORD)(eAlpha*255.0f) << 24);
	ma = ((DWORD)(mAlpha*255.0f) << 24);
	color = ((DWORD)(r*255.0f) << 16) | ((DWORD)(g*255.0f) << 8) | (DWORD)(b*255.0f);
	Set4(type, aFrame, aa | color, eFrame, ea | color, mFrame, ma | color, Pos, Move, Power, angle, rotate, scale, stretch, flag, fDeceleration, deceleration);
}

void iexParticlePlus::Set5(int type, int aFrame, COLOR aColor, int eFrame, COLOR eColor, int mFrame, COLOR mColor,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate, float scale, float stretch, u8 flag, bool fDeceleration, float deceleration, bool fWork, float Work, float reflection)
{
	PARTICLE pd;

	pd.type = type;
	pd.aFrame = aFrame;
	pd.aColor = aColor;
	pd.eFrame = eFrame;
	pd.eColor = eColor;
	pd.mFrame = mFrame;
	pd.mColor = mColor;

	pd.Pos = *Pos;
	pd.Move = *Move;
	pd.Power = *Power;
	pd.rotate = rotate;

	pd.scale = scale;
	pd.angle = angle;
	pd.stretch = stretch;

	pd.flag = flag;

	pd.isWork = fWork;
	pd.work = Work;
	pd.Reflection = reflection;

	pd.isDeceleration = fDeceleration;
	pd.deceleration = deceleration;


	Set(&pd);
}

void	iexParticlePlus::Set5(int type, int aFrame, float aAlpha, int eFrame, float eAlpha, int mFrame, float mAlpha,
	LPVECTOR3 Pos, LPVECTOR3 Move, LPVECTOR3 Power, float angle, float rotate,
	float r, float g, float b, float scale, float stretch, u8 flag, bool fDeceleration, float deceleration, bool fWork, float Work, float reflection)
{
	COLOR	color;
	DWORD	aa, ea, ma;

	aa = ((DWORD)(aAlpha*255.0f) << 24);
	ea = ((DWORD)(eAlpha*255.0f) << 24);
	ma = ((DWORD)(mAlpha*255.0f) << 24);
	color = ((DWORD)(r*255.0f) << 16) | ((DWORD)(g*255.0f) << 8) | (DWORD)(b*255.0f);
	Set5(type, aFrame, aa | color, eFrame, ea | color, mFrame, ma | color, Pos, Move, Power, angle, rotate, scale, stretch, flag, fDeceleration, deceleration, fWork, Work, reflection);
}
