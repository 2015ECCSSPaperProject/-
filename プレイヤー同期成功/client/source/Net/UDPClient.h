#pragma once
//*****************************************************************************************************************************
//
//	udp client	
//
//*****************************************************************************************************************************
class UDPClient
{
protected:
	SOCKET		m_sock;
	sockaddr_in m_server;
	void IPloader(char* in);


public:
	UDPClient();
	~UDPClient();

	void Send(void *data, int size);
	int	 Receive(void *data, int size);

	friend class SocketManager;
};
