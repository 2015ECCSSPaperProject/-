
#include "paperQueue.h"

// ������
void PaperQueue::Clear()
{
	// ��ɂȂ�܂Ń|�b�v
	for( ;; )
	{
		// �󂾂�����A��
		if( EventQueue.empty() )
		{
			return;
		}// 

		EventQueue.pop();

	}

}

void PaperQueue::Push( PaperData pData )
{
	// ���̃C�x���g���L���[�փv�b�V��!
	EventQueue.push( pData );
}

PaperData PaperQueue::Update()
{
	PaperData sendData;
	sendData.ID = -1;	//�_�~�[
	sendData.from = -1;

	// �󂾂�����A��
	if( EventQueue.empty() )
	{
		return sendData;
	}// -1�̃_�~�[��Ԃ�

	// �擪�̃f�[�^�𑗂�
	sendData = EventQueue.front();

	// �f�[�^�𑗂�����|�b�v
	EventQueue.pop();

	// ����Ԃ�
	return sendData;
}
