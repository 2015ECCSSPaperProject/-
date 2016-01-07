
#pragma once

#include "../../../../share_data/Enum_public.h"

class Score;
class BasePlayer;

class Poster : public Paper_obj_Mesh
{
public:
	Poster();
	~Poster();

	void Initialize(iexMesh *model) override;
	void Update() override;
	void Render(iexShader *shader = nullptr, char *name = '\0') override;

	//**************************************************

	unsigned int Get_receive_data_size();

	void Set_receive_data( char *in );

};



// ポスターを継承したクラス

class Calendar : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};
