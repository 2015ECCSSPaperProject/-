#include	"iExtreme.h"

//*****************************************************************************
//
//		�V�F�[�_�[�֘A
//
//*****************************************************************************

//*****************************************************************************
//	�������E���
//*****************************************************************************
//------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------
iexShader::iexShader(char* filename, bool iscfx)
{
	LPDEVICE	lpDevice = iexSystem::GetDevice();
	//	�t�@�C�����`�F�b�N
	char fname[256];
	if (iscfx)
	{
		sprintf(fname, "%s", filename);
	}
	else
	{
	if( filename[ strlen(filename)-3 ] != '.' ) sprintf( fname, "%s.fx", filename );
	else sprintf(fname, "%s", filename);
	}


	// �V�F�[�_�̓ǂݍ���
	HRESULT	hr;
	LPD3DXBUFFER pErr=NULL;
	//D3DXMACRO;
	//LPD3DXINCLUDE;
	// �R���p�C���ς݂̃t�@�C����ǂݍ��ށH
	if (iscfx) hr = D3DXCreateEffectFromFile(lpDevice, fname, NULL, NULL, D3DXSHADER_SKIPVALIDATION, NULL, &m_pShader, &pErr);
	else hr = D3DXCreateEffectFromFile(lpDevice, fname, NULL, NULL, 0, NULL, &m_pShader, &pErr);
	
	if( FAILED(hr) )
	{
		char	szBuffer[512];
		sprintf(szBuffer,"\terrors: %s\n", (char*)pErr->GetBufferPointer() );
		OutputDebugString(szBuffer);
		return;
	}

	//	��{�ϐ��̓ǂݍ���
	m_hmWVP      = m_pShader->GetParameterByName( NULL, "Projection" );
	m_htexDecale = m_pShader->GetParameterByName( NULL, "Texture" );
}

//------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------
iexShader::~iexShader()
{
	m_pShader->Release();
}

//*****************************************************************************
//	�p�����[�^�ݒ�
//*****************************************************************************

void iexShader::SetMatrix( Matrix* mat ){ m_pShader->SetMatrix( m_hmWVP, mat ); }
void iexShader::SetMatrix( Matrix& mat ){ m_pShader->SetMatrix( m_hmWVP, &mat ); }

void iexShader::SetTexture( Texture2D* tex ){ m_pShader->SetTexture( m_htexDecale, tex ); }
void iexShader::SetTexture( Texture2D& tex ){ m_pShader->SetTexture( m_htexDecale, &tex ); }

void iexShader::SetValue( char* name, iex2DObj* obj ){ m_pShader->SetTexture( name, obj->GetTexture() ); }
void iexShader::SetValue( char* name, iex2DObj& obj ){ m_pShader->SetTexture( name, obj.GetTexture() ); }

void iexShader::SetValue( char* name, Texture2D* tex ){ m_pShader->SetTexture( name, tex ); }
void iexShader::SetValue( char* name, Texture2D& tex ){ m_pShader->SetTexture( name, &tex ); }

void iexShader::SetValue( char* name, Matrix* mat ){ m_pShader->SetMatrix( name, mat ); }
void iexShader::SetValue( char* name, Matrix& mat ){ m_pShader->SetMatrix( name, &mat ); }

void iexShader::SetValue( char* name, D3DXVECTOR4* v ){ m_pShader->SetVector( name, v ); }
void iexShader::SetValue( char* name, D3DXVECTOR4& v ){ m_pShader->SetVector( name, &v ); }

void iexShader::SetValue( char* name, Vector3* v ){ m_pShader->SetFloatArray( name, (float*)v, 3 ); }
void iexShader::SetValue( char* name, Vector3& v ){ m_pShader->SetFloatArray( name, (float*)&v, 3 ); }


void iexShader::SetValue( char* name, float f ){ m_pShader->SetFloat( name, f ); }
void iexShader::SetValue( char* name, int d ){ m_pShader->SetInt( name, d ); }
void iexShader::SetValue( char* name, DWORD d ){ m_pShader->SetValue( name, &d, 4 ); }





