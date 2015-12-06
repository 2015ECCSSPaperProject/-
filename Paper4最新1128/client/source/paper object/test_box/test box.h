
#pragma once

// Paper_objのテストで使ったクラス
class Test_box : public Paper_obj
{
public:
	void Initialize(iex3DObj *model) override;
	void Update() override;
	void Render(iexShader *shader = nullptr, char *name = '\0') override;

	//**************************************************

	unsigned int Get_receive_data_size()override;

	void Set_receive_data( char *in )override;

};
