#pragma once
#ifndef _MONJU__CL_BUFFER_H__
#define _MONJU__CL_BUFFER_H__

#include <stdint.h>
#include <CL/cl.h>

namespace monju
{
	/// <summary>
	/// OpenCL�̃������I�u�W�F�N�g
	/// </summary>
	class _ClBuffer
	{
	private:
		size_t _mem_bytes;
		cl_mem _mem_obj;

	private:
		cl_mem _create_buffer(cl_context context, size_t size);
		void _release_buffer();

	public:
		_ClBuffer(cl_context context, size_t size);
		~_ClBuffer();

	public:
		cl_mem mem_obj() const;
		size_t mem_size() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		_ClBuffer(const _ClBuffer&) = delete;
		_ClBuffer(_ClBuffer&&) = delete;
		_ClBuffer& operator=(const _ClBuffer&) = delete;
		_ClBuffer& operator=(_ClBuffer&&) = delete;
	};
}

#endif // !_MONJU__CL_BUFFER_H__
