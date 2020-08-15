#pragma once
#ifndef _MONJU__CL_BUFFER_H__
#define _MONJU__CL_BUFFER_H__

#include "MonjuTypes.h"

namespace monju
{
	/// <summary>
	/// OpenCLのメモリオブジェクト
	/// GPUに実体がある
	/// </summary>
	class _ClBuffer
	{
#pragma region Private Field
	private:
		/// <summary>
		/// メモリのバイト数
		/// </summary>
		size_t _size;
		/// <summary>
		/// OpenCLメモリオブジェクト
		/// 存続期間はインスタンスと同じ
		/// </summary>
		cl_mem _mem;
#pragma endregion

#pragma region Helper
	private:
		cl_mem _createBuffer(cl_context context, size_t size);
		void _releaseBuffer();
#pragma endregion

#pragma region Constructor
	public:
		_ClBuffer(cl_context context, size_t size);
		~_ClBuffer();
#pragma endregion

#pragma region Public Method
	public:
		cl_mem clMem() const;
		size_t size() const;
#pragma endregion

#pragma region Removing derault behavior
		// コピー禁止・ムーブ禁止
	public:
		_ClBuffer(const _ClBuffer&) = delete;
		_ClBuffer(_ClBuffer&&) = delete;
		_ClBuffer& operator=(const _ClBuffer&) = delete;
		_ClBuffer& operator=(_ClBuffer&&) = delete;
#pragma endregion
	};
}

#endif // !_MONJU__CL_BUFFER_H__
