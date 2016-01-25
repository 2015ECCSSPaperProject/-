
#pragma once

class Score;
class BasePlayer;

enum KIND_PAPER_OBJECT;

class Poster : public Paper_obj_Mesh
{
public:
	Poster();
	~Poster();

	void Initialize( iexMesh *model, int point ) override;
	void Subclass_update() override;
	void Subclass_render( iexShader *shader = nullptr, char *name = '\0' ) override;

	virtual	KIND_PAPER_OBJECT Get_kind();

	//**************************************************

	unsigned int Get_receive_data_size();

	void Set_receive_data( char *in );

	// オーバーライド
	void Rend();
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

class Huusenn : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();

	void Initialize( iexMesh *model, int point, int start_time );

	void Subclass_update() override;

private:
	int start_time;
};
