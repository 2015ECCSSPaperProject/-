
#pragma once

class Score;
class BasePlayer;

class Poster : public Paper_obj
{
public:
	Poster();
	~Poster();

	void Initialize(iex3DObj *model) override;
	void Update() override;
	void Render(iexShader *shader = nullptr, char *name = '\0') override;

	//**************************************************

	unsigned int Get_receive_data_size();

	void Set_receive_data( char *in );

};
