#include<winsock2.h>

#include"iextreme.h"
#include"UDPServer.h"

namespace
{
	const int g_port(12010);
}

//*****************************************************************************************
//
//	UDP�T�[�o�[
//
//*****************************************************************************************
//=======================================================================
//	������
//=======================================================================
UDPServer::UDPServer() :
m_sock(INVALID_SOCKET)
{
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(g_port);


	m_sock = socket(AF_INET, SOCK_DGRAM, 0);
	bind(m_sock, (sockaddr*)&server, sizeof(server));


	FD_ZERO(&m_fds);
	FD_SET(m_sock, &m_fds);


	for (int i = 0; i<PLAYER_MAX; ++i)
		m_client_addr[i].sin_addr.S_un.S_addr = 0;
}
UDPServer::~UDPServer()
{
	if (m_sock != INVALID_SOCKET)
		closesocket(m_sock);
}



//=======================================================================
//	�f�[�^���M��M
//=======================================================================		
void UDPServer::Send(int client, char *data, int size)
{
	sendto(m_sock, data, size, 0, (sockaddr*)&m_client_addr[client], sizeof(m_client_addr[client]));
}
int	UDPServer::Receive(char *data, int *size)
{
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set	fd_work;
	memcpy(&fd_work, &m_fds, sizeof(fd_set));


	//	fds�ɐݒ肳�ꂽ�\�P�b�g���ǂݍ��݉\�ɂȂ�܂ő҂��܂�
	int n = select(0, &fd_work, NULL, NULL, &tv);


	enum{ TIME_OUT = 0 };
	if (n <= TIME_OUT)
		return -1;

	int recvsize;
	int	addr_len;
	sockaddr_in addr;

	/*�f�[�^��M*/
	addr_len = sizeof(sockaddr_in);
	recvsize = recvfrom(m_sock, data, *size, 0, (sockaddr*)&addr, &addr_len);
	*size = recvsize;

	if (recvsize <= 0)
		return -1;

	/*�N���C�A���g����*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_client_addr[i].sin_addr.S_un.S_addr == 0)
			continue;

		if (addr.sin_addr.S_un.S_addr == m_client_addr[i].sin_addr.S_un.S_addr)
		{
			m_client_addr[i] = addr;
			return i;
		}
	}

	/*�V�K�����*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_client_addr[i].sin_addr.S_un.S_addr != 0)
			continue;

		m_client_addr[i] = addr;
		return i;
	}
	return -1;
}

ULONG UDPServer::Get_ClientAddr(int i){

	return m_client_addr[i].sin_addr.S_un.S_addr;

}