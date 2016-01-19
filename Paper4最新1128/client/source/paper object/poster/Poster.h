
#pragma once

class Score;
class BasePlayer;

enum KIND_PAPER_OBJECT;

class Poster : public Paper_obj_Mesh
{
public:
	Poster();
	~Poster();

	void Initialize(iexMesh *model) override;
	void Update() override;
	void Subclass_render( iexShader *shader = nullptr, char *name = '\0' ) override;

	virtual	KIND_PAPER_OBJECT Get_kind();

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

class Magazin : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Money : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Seisho : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shinbun : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Sign : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Toilet_paper : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Zasshi : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shoji : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};
