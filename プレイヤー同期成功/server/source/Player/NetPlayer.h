#pragma once
/*	ネットプレイヤー達	*/

/*前方宣言*/
class BasePlayer;

//********************************************************************************************************************
//		net
//****************************************************************************************************************
class	NetPlayer : public	BasePlayer
{
public:

public:
	NetPlayer(int id);
	~NetPlayer();
	void Initialize(iexMesh *obj);
	void Update();
	void Render();
};
