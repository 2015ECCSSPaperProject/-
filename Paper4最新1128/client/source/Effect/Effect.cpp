#include	"iextreme.h"
#include	"Effect.h"
#include	"../Random/Random_mt.h"
//*****************************************************************************************************************************
//
//	エフェクト
//
//*****************************************************************************************************************************
iexParticlePlus	*particle = NULL;

//*****************************************************************************************************************************
//
//	きらきらエフェクト
//
//*****************************************************************************************************************************
void ShiningEffect(Vector3 _pos, float _scale)
{
	Vector3 Pos, Move, Power;

	static int Flame = 0;
	Flame++;
	if (Flame >= 6)
	{
		Flame = 0;


		float RamX = (float)(Random_mt::Get(-2, 2));
		float RamY = (float)(Random_mt::Get(-3, 3));

		Pos.x = _pos.x + RamX;
		Pos.y = _pos.y + RamY;
		Pos.z = _pos.z;
		Move.x = 0.0f;
		Move.y = 0.03f;
		Move.z = 0.0f;
		Power.x = 0.0f;
		Power.y = 0.0f;
		Power.z = 0.0f;
		// 本来は↓
		//particle->Set(10, 0, .0f, 40, .0f, 12, 1.0f, &Pos, &Move, &Power, 1.0f, 1.0f, 1.0f, _scale, RS_COPY_NOZ);

		particle->Set(10, 0, 0x00ffffff, 40, 0x00ffffff, 12, 0xffffffff,
			&Pos, &Move, &Power, 10.0f, 1.0f, _scale, RS_COPY_NOZ);
	}
	
}
//*****************************************************************************************************************************
//
//	バースト打てますのエフェクト
//
//*****************************************************************************************************************************
void AuraMAXEffect(Vector3 _pos, float _scale, Vector3 _col)
{
	Vector3 Pos, Move, Power;


	float RamX = (float)(Random_mt::Get(-2, 2));
	float RamY = (float)(Random_mt::Get(-2, 2));

	Pos.x = _pos.x + RamX;
	Pos.y = _pos.y + RamY;
	Pos.z = _pos.z;
	Move.x = 0.0f;
	Move.y = 0.1f;
	Move.z = 0.0f;
	Power.x = 0.0f;
	Power.y = 0.0f;
	Power.z = 0.0f;

	// 本来は↓
	particle->Set(10, 0, .0f, 50, .0f, 20, 1.0f, &Pos, &Move, &Power, _col.x, _col.y, _col.z, _scale, RS_ADD);

	//particle->Set(10, 0, 0x00ffffff, 50, 0x00ffffff, 20, 0xffffffff,
	//&Pos, &Move, &Power, 10.0f, 1.0f, _scale, RS_ADD);



}


	/*先輩の花火エフェクト改造*/
	void EffectFireFlour(Vector3 p, FIRE_COLOR color, float scale)
	{
		Vector3	Pos, Move, Power;
		Move = Power = Vector3(0, 0, 0);
		Pos = p;

		float a, g, b ;		//外側
		float a2, g2,  b2;	//内側
		float p_scale = 3.0f * scale;
		float p_angle = 0.0f;
		float p_rotate = 0.0f;

		float work = 0.1f;


		switch (color){
		case RED:
			a = 1.0f;
			g = 0.2f;
			b = 0.0f;

			a2 = 1.0f;
			g2 = 0.15f;
			b2 = 0.0f;
			break;
		case GREEN:
			a = .0f;
			g = 1.0f;
			b = .2f;

			a2 = .0f;
			g2 = 1.0f;
			b2 = .0f;
			break;
		case BLUE:

			a = .8f;
			g = .8f;
			b = 1.0f;

			a2 = 1.0f;
			g2 = 1.0f;
			b2 = 1.0f;

			//a = .0f;
			//g = .7f;
			//b = 1.0f;

			//a2 = .0f;
			//g2 = .5f;
			//b2 = 1.0f;
			break;
		case YELLOW:
			a = 0.5f;
			g = 0.5f;
			b = 0.1f;

			a2 = 0.55f;
			g2 = 0.45f;
			b2 = 0.0f;
			break;
		case PURPLE:
			a = 1.0f;
			g = 0.3f;
			b = 1.0f;

			a2 = 1.0f;
			g2 = 0.0f;
			b2 = 1.0f;
			break;
		case RANDOM:

			a = rand() % 10 * 0.1f;
			g = rand() % 10 * 0.1f;
			b = rand() % 10 * 0.1f;

			a2 = rand() % 10 * 0.1f;
			g2 = rand() % 10 * 0.1f;
			b2 = rand() % 10 * 0.1f;
			break;
		}

		//集中線
		particle->Set3(5, 0, 1.0f, 25, .0f, 10, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a, g, b, p_scale+4, 1.06f, RS_COPY);

		Move.y = 5.0f;
		Power.y = -0.001f;

		for (int i = 0; i < 50; i++){

			Move.x = (rand() % 50 - 25)*1.0f;
			Move.y = (rand() % 50 - 25)*1.0f;
			Move.z = (rand() % 50 - 25)*1.0f;
			Move.Normalize();
			Move *= 4;
			//Move *= 0.5f;//　★飛ぶ速度

			p_angle = rand() % 314 * 0.01f;
			//	p_rotate = (rand() % 30 - 15) * 0.01f;


			//particle->Set4(3, 0, 1.0f, 100, .0f, 80, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a, g, b, p_scale, 1.0f, RS_COPY, true, 0.95f);
			particle->Set5(3, 0, 1.0f, 25, .0f, 10, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a, g, b, p_scale, 1.0f, RS_COPY, true, 0.95f, true, work);
			//最初　最後　中間

			Move.x = (rand() % 50 - 25)*1.0f;
			Move.y = (rand() % 50 - 25)*1.0f;
			Move.z = (rand() % 50 - 25)*1.0f;
			Move.Normalize();
			//Move *= 0.2f;//　★飛ぶ速度 Move *= 0.7f;

			p_angle = rand() % 314 * 0.01f;
			//	p_rotate = (rand() % 30 - 15) * 0.01f;


			//		particle->Set4(3, 0, 1.0f, 140, .0f, 80, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a2, g2, b2, p_scale*0.5f, 1.0f, RS_COPY, true, 0.95f);
			particle->Set5(3, 0, 1.0f, 30, .0f, 15, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a2, g2, b2, p_scale*0.5f, 1.0f, RS_COPY, true, 0.95f, true, work);

		}
	}

	void	EffectBomb(Vector3 pos, float scale)
	{

		Vector3	Pos = pos, Move, Power;
		Move = Power = Vector3(0, 0, 0);

		float angle = 0.0f, rotate = 0.0f;

		particle->Set3(9, 0, .0f, 50, .0f, 10, 1.0f, &Pos, &Move, &Power, angle, rotate, 1.0f, 0.5f, 0.0f, 15.7f*scale, 1.02f, RS_ADD);
		particle->Set3(5, 0, .0f, 50, .0f, 10, 0.2f, &Pos, &Move, &Power, angle, rotate, 1.0f, 1.5f, 1.0f, 15.7f*scale, 1.03f, RS_ADD);

		for (int i = 0; i < 5; i++){
			Move.x = (rand() % 100 - 50)*0.005f;
			Move.y = (rand() % 100)*0.005f;
			Move.z = (rand() % 100 - 50)*0.005f;
			Power.x = .0f;
			Power.y = -0.01f;
			Power.z = .0f;
			angle = rand() % 314 * 0.01f;
			rotate = (rand() % 100 - 50)*0.001f;

			Move *= scale;
			Power *= scale;

			particle->Set3(14, 0, .0f, 50, .0f, 10, .2f, &Pos, &Move, &Power, angle, rotate, 1.0f, 1.0f, 1.0f, 20.7f*scale, 1.01f, RS_SUB);

		}
	}
	//*****************************************************************************************************************************
	//
	//	爆発エフェクトクラス
	//
	//*****************************************************************************************************************************

	SimpleExplosion::SimpleExplosion()
	{
		chip_obj = new iexMesh("DATA/scar/melonChip.IMO");
		isLive = false;
	}
	SimpleExplosion::~SimpleExplosion()
	{

		if (chip_obj) delete chip_obj;
	}

	void SimpleExplosion::Init(Vector3 Pos, float Scale)
	{
		pos = Pos;
		scale = Scale;
		timer = 0;

		isLive = true;

		for (int i = 0; i < CHIP_MAX; i++){


			chip[i].pos = pos;


			chip[i].move.x = ((rand() % 100 - 50)*0.01f);
			chip[i].move.y = ((rand() % 300 + 50)*0.005f);
			chip[i].move.z = ((rand() % 100 - 50)*0.01f);

			chip[i].angle.x = rand() % 100 * 0.1f;
			chip[i].angle.y = rand() % 100 * 0.1f;
			chip[i].angle.z = rand() % 100 * 0.1f;

			chip[i].rotate.x = rand() % 100 * 0.001f;
			chip[i].rotate.y = rand() % 100 * 0.001f;
			chip[i].rotate.z = rand() % 100 * 0.001f;


			chip[i].scale = scale*0.05f;
		}


	}
	int SimpleExplosion::Update()
	{
		if (isLive){

			timer++;
			if (timer > 300){
				timer = 0;
				isLive = false;
			}

			for (int i = 0; i < CHIP_MAX; i++){

				chip[i].move.y -= 0.05f;

				if (chip[i].pos.y < -.0f){
					chip[i].move *= 0.0f;
					chip[i].rotate *= 0.0f;
				}
				else{
					EffectChup(chip[i].pos);
				}

				chip[i].pos += chip[i].move;
				chip[i].move *= 0.95f;

				chip[i].angle += chip[i].rotate;
			}
		}

		return 0;
	}
	void SimpleExplosion::Render()
	{
		if (isLive){

			for (int i = 0; i < CHIP_MAX; i++){

				chip_obj->SetPos(chip[i].pos);
				chip_obj->SetAngle(chip[i].angle);
				chip_obj->SetScale(chip[i].scale);
				chip_obj->Update();
				chip_obj->Render();
			}
		}
	}

	void SimpleExplosion::EffectChup(Vector3 p)
	{
		Vector3	Pos, Move, Power;
		Move = Power = Vector3(0, 0, 0);

		float a = .5f, g = 0.0f, b = 0.0f;
		float p_scale = 2.0f * scale;
		float p_angle = 0.0f;
		float p_rotate = 0.0f;


		Move.x = (float)(rand() % 100 - 50);
		Move.y = (float)(rand() % 100 - 50);
		Move.z = (float)(rand() % 100 - 50);

		Move.Normalize();
		Move *= 0.2f;

		Power.y = -0.01f;

		Pos = p;
		p_angle = rand() % 314 * 0.01f;
		p_rotate = (rand() % 30 - 15) * 0.01f;
		particle->Set3(8, 0, 1.0f, 50, .0f, 10, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a, g, b, p_scale, 0.96f, RS_COPY);

	}
	void SimpleExplosion::EffectBomb()
	{
		Vector3	Pos, Move, Power;
		Move = Power = Vector3(0, 0, 0);

		float a = 1.0f, g = 1.0f, b = 1.0f;
		float p_scale = 1.0f * scale;
		float p_angle = 0.0f;
		float p_rotate = 0.0f;
		Pos = pos;

		Pos.y += 2.0f;

		for (int i = 0; i < 50; i++){

			Move.x = (rand() % 100 - 50) * 0.08f;
			Move.y = (rand() % 100) * -0.04f;
			Move.z = (rand() % 100 - 50) * 0.08f;

			Power.y = 0.002f;

			Move *= scale;
			Power *= scale;

			p_angle = rand() % 314 * 0.01f;
			p_rotate = (rand() % 30 - 15) * 0.001f;

			particle->Set3(0, 0, 1.0f, 30, .0f, 30, 1.0f, &Pos, &Move, &Power, p_angle, p_rotate, a, g, b, p_scale, 1.001f, RS_COPY);
		}
	}

	//*****************************************************************************************************************************
	//
	//	爆発の傷跡エフェクトクラス
	//
	//*****************************************************************************************************************************


	BombScar::BombScar()
	{
		obj = nullptr;
	}
	BombScar::~BombScar()
	{
		if (obj){
			delete obj;

		}
	}
	void BombScar::Init()
	{
		obj = new iexMesh("DATA/scar/scar.IMO");

		for (int i = 0; i < SCAR_MAX; i++){
			scar[i].isLive = false;
			scar[i].pos = Vector3(0, 0, 0);
			scar[i].scale = .0f;
			scar[i].angle = .0f;

			scar[i].timer = 600;

		}


	}

	void BombScar::Update()
	{

		for (int i = 0; i < SCAR_MAX; i++){
			if (!scar[i].isLive) continue;

			scar[i].timer--;
			if (scar[i].timer <= 0){
				scar[i].isLive = false;
			}

		}
	}

	void BombScar::Render()
	{
		for (int i = 0; i < SCAR_MAX; i++){
			if (!scar[i].isLive) continue;

			if (scar[i].timer == 595)
				EffectBomb(scar[i].pos, scar[i].scale);

			obj->SetPos(scar[i].pos);
			obj->SetAngle(scar[i].angle);
			obj->SetScale(scar[i].scale);
			obj->Update();
			obj->Render(RS_SUB);
		}
	}

	void BombScar::Set(Vector3 pos, float scale, float angle)
	{
		for (int i = 0; i < SCAR_MAX; i++){
			if (scar[i].isLive) continue;
			scar[i].isLive = true;
			scar[i].pos = pos;
			scar[i].pos.y += 0.3f;
			scar[i].scale = scale;
			scar[i].timer = 600;

			if (angle == -1.0f){
				scar[i].angle = rand() % 314 * 0.01f;
			}
			else{
				scar[i].angle = angle;
			}

			break;
		}
	}



	void Sand_effect(float x, float y, float z)
	{
		Vector3 Pos, Move, Power;

		for (int i = 0; i < 50; i++){
			Pos.x = x + (rand() % 24 - 12);
			Pos.y = y + (rand() % 14 + 6);
			Pos.z = z + (rand() % 24 - 12);
			Move.x = (rand() % 3 - 1.5f)*0.5f;
			Move.y = (rand() % 2 + .0f) * 0.5f;
			Move.z = (rand() % 3 - 1.5f)*0.5f;
			Power.x = .0f;
			Power.y = .001f;
			Power.z = .0f;

			particle->Set3(0, 0, 0.8f, 60, .0f, 30, .45f, &Pos, &Move, &Power, 0, .01f,
				1.0f, 1.0f, 1.0f, 8.0f, 1.001f, RS_COPY);
		}
	}