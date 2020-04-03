#pragma once
#ifndef _MONJU__CL_BUFFER_H__
#define _MONJU__CL_BUFFER_H__

#include "MonjuTypes.h"

namespace monju
{
	// OpenCL�̃�����
	//  �z�X�g�������𗘗p���Ȃ�
	class _ClBuffer
	{
	private:
		size_t _size;
		cl_mem _mem;	// ����\��

		cl_mem _createBuffer(cl_context context, size_t size);
		void _releaseBuffer();

	public:
		_ClBuffer(cl_context context, size_t size);
		~_ClBuffer();
		cl_mem clMem() const;
		size_t size() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		_ClBuffer(const _ClBuffer&) = delete;
		_ClBuffer(_ClBuffer&&) = delete;
		_ClBuffer& operator=(const _ClBuffer&) = delete;
		_ClBuffer& operator=(_ClBuffer&&) = delete;

	};
}

#endif // !_MONJU__CL_BUFFER_H__
