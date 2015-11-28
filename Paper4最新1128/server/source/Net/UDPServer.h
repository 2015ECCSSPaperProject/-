#pragma once
//*****************************************************************************************
//
//	�\�P�b�g �T�[�o�[
//
//*****************************************************************************************

//=======================================================================
//	UDP�p
//=======================================================================
class UDPServer
{
private:
	SOCKET		m_sock;
	fd_set		m_fds;
	sockaddr_in m_client_addr[PLAYER_MAX];


public:
	UDPServer();
	~UDPServer();

	void Send(int client, char *data, int size);
	int	 Receive(char *data, int *size);
	void CloseClient(int client){ m_client_addr[client].sin_addr.S_un.S_addr = 0; }

	//�@�Q�b�^�[�E�Z�b�^�[
	ULONG Get_ClientAddr(int i);
};

