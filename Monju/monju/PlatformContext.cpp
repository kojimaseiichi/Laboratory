#include "PlatformContext.h"

// �v���p�e�B�t�@�C���ւ̃p�X

// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������

// �v���b�g�t�H�[�������t�@�C������ǂݍ���

monju::PlatformContext::PlatformContext()
{
	_work_folder = "";
	_platform_id = 0;
}

monju::PlatformContext::~PlatformContext()
{
	_release();
}

// �v���b�g�t�H�[�������t�@�C������ǂݍ��ݗ��p�\�Ƃ���

// �v���b�g�t�H�[�������t�@�C���ɕۑ�����

// ���[�N�X�y�[�X�f�B���N�g��

// �f�o�C�X�R���e�L�X�g�i�v�Z�����j

monju::DeviceContext& monju::PlatformContext::deviceContext() const
{
	return *_pDeviceContext;
}
