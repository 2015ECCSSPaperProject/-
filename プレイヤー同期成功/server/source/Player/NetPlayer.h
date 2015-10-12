#pragma once
/*	ネットプレイヤー達	*/


//	操作デスク
struct	CONTROL_DESC{
	BYTE	moveFlag; //　上下左右に動かすフラグ

};

/*前方宣言*/
class BasePlayer;

//********************************************************************************************************************
//		net
//****************************************************************************************************************
class	NetPlayer : public	BasePlayer
{
private:
	int		m_id;

public:
	NetPlayer(int id);
	void	Update(void);

	//	更新（操作）
	void	Control_Update(const CONTROL_DESC& _ControlDesc);

};
