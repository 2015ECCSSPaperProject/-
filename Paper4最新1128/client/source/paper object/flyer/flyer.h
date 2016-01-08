
//#include "iextreme.h"
//#include "../paper object.h"

#pragma once

enum KIND_PAPER_OBJECT;

class Flyer : public Paper_obj_3DObj
{
public:
	void Initialize( iex3DObj *model, int start_time );
	void Update() override;
	void Render( iexShader *shader = nullptr, char *name = '\0' ) override;

	KIND_PAPER_OBJECT Get_kind();

	//**************************************************

	unsigned int Get_receive_data_size()override;

	void Set_receive_data( char *in )override;



private:
	float high; // —‰º’†‚Ì‚‚³
	int start_time; // —‰ºŠJn •b
};
