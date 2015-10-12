#pragma comment( lib, "wsock32.lib" )
#include"iextreme.h"
#include"UDPclient.h"

namespace
{
	const int   g_port(10075);
}


//*****************************************************************************************
//
//	UDP�N���C�A���g
//
//*****************************************************************************************
//=======================================================================
//	������
//=======================================================================
UDPClient::UDPClient(): 
m_sock( INVALID_SOCKET )
{
	ZeroMemory( &m_server, sizeof( m_server ));
	m_server.sin_family = AF_INET;
	m_server.sin_port   = htons( g_port );

	char str[256];
	IPloader(str);

	hostent* host( gethostbyname( str ));
	m_server.sin_addr.S_un.S_addr = *((ULONG*)host->h_addr_list[0] );


	m_sock = socket( AF_INET, SOCK_DGRAM, 0 );


	if( m_sock == INVALID_SOCKET )
		MessageBox( 0, "socket err", null, MB_OK );


	int timeout( 1000*5 );
	if( setsockopt( m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout) ) == SOCKET_ERROR )
		MessageBox( 0, "socket err", null, MB_OK );
}
UDPClient::~UDPClient()
{	
	if( m_sock != INVALID_SOCKET ) 
		closesocket( m_sock );
}
void UDPClient::Send( void *data, int size )
{	
	int byte = sendto( m_sock, (char*)data, size, 0, (sockaddr *)&m_server, sizeof( m_server ));

	if( byte == SOCKET_ERROR )
	{
		printf("send err");
		//MessageBox( 0, "send err", null, MB_OK );
	}
}
int	UDPClient::Receive( void *data, int size )
{
	int recvsize = recv( m_sock, (char*)data, size, 0 );

	if( recvsize == SOCKET_ERROR )
	{		
		printf("recv time out");
		//int error = WSAGetLastError();
		//MessageBox( 0, "recv time out", null, MB_OK );
		return -1;
	}

	return recvsize;
}


//===================================================================================
//  
//===================================================================================
void UDPClient::IPloader( char* in )
{
	FILE *fp;

	if ((fp = fopen("ip.txt", "r")) == NULL)
	{
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	/* (3)�G���[�̏ꍇ�͒ʏ�A�ُ�I������ */
	}
	
	/* (4)�t�@�C���̓ǂ݁i�����j*/
	while (fgets(in, 256, fp) != NULL) 
	{
		/* �����ł�fgets()�ɂ��P�s�P�ʂœǂݏo�� */
		//printf("%s", s);
	}
	fclose(fp);	/* (5)�t�@�C���̃N���[�Y */
}