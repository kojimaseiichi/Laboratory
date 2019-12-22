#pragma once
#ifndef _MONJU_DEVICE_MEMORY_H__
#define _MONJU_DEVICE_MEMORY_H__

#include <string>
#include <CL/cl.h>
#include <fstream>
#include <map>
#include <unordered_set>

#include "OpenClException.h"
#include "MonjuTypes.h"
#include "GridMatrixStorage.h"
#include "DeviceContext.h"
#include "Device.h"
#include "VariableKind.h"
#include "util_misc.h"

namespace monju {

	// 特定のデバイス（GPU）のメモリを管理
	// デバイスメモリとホストメモリ間のデータ転送を管理
	// DeviceKernelと連携してカーネルの実行を実現する
	class DeviceMemory
	{
	protected:
		// OpenCLメモリオブジェクト
		struct MemAttr
		{
			float* p;
			cl_mem mem;
			size_t bytes;
		};

		// データ
	protected:
		Device* _p_device;
		// 編集の種類ごとにホストメモリとデバイスメモリを保持
		std::unordered_map<VariableKind, MemAttr>	_map_mem;
		std::unordered_set<VariableKind>			_read_required;
		std::unordered_set<VariableKind>			_write_required;

		// 初期化・生成
	public:
		DeviceMemory(Device& device) :
			_read_required(),
			_write_required()
		{
			_p_device = &device;
		}
		virtual ~DeviceMemory();

		// コピー禁止
	protected:
		DeviceMemory(const DeviceMemory&) = delete;
		DeviceMemory& operator=(const DeviceMemory&) = delete;

		// プロパティ
	public:
		Device& device() { return *_p_device; }
		std::unordered_set<VariableKind> read_required() { return _read_required; }
		std::unordered_set<VariableKind> write_required() { return _write_required; }

		// ヘルパ
	protected:
		// メモリ転送　CPU -> GPU
		template <typename T>
		void	_queueWriteBuffer(cl_command_queue queue, cl_mem mem, size_t size, const T* p_data);
		// メモリ転送　GPU -> CPU
		template <typename T>
		void	_queueReadBuffer(cl_command_queue queue, cl_mem mem, size_t size, T* p_data);
		// メモリ転送　CPU <-> GPU
		template <class Matrix>
		void	_queueTransferBuffer(cl_command_queue queue, cl_mem mem, Matrix& matrix, bool write);
		// メモリ転送　行列を追加し、対応するGPUメモリを割り当てる
		template <class Matrix>
		void	_addNewClMem(VariableKind v, Matrix& m);
		// 保持している行列を削除し、対応するGPUメモリを開放する
		void	_clearAllClMem();
		// 保持している行列データから、指定した変数の行列データを取得する。
		bool	_findClMem(VariableKind v, MemAttr* out);
		// メモリ一貫性をチェック（ホストメモリとデバイスメモリの両方が変更されている状態）
		bool	_checkReadWriteConflict();


		// 仮想関数
	public:
		// リソースの開放（オブジェクトのライフサイクル管理の共通関数）
		virtual void	release();

		// 公開関数
	public:
		// ホストメモリからデバイスメモリへ書き込み（デバイス指定）
		void	writeBuffer(Device& device, std::unordered_set<VariableKind> variableKindSet);
		// ホストメモリからデバイスメモリへ書き込み
		void	writeBuffer(std::unordered_set<VariableKind> variableKindSet);
		// デバイスメモリからホストメモリへ読み込み（デバイス指定）
		void	readBuffer(Device& device, std::unordered_set<VariableKind> variableKindSet);
		// デバイスメモリからホストメモリへ読み込み
		void	readBuffer(std::unordered_set<VariableKind> variableKindSet);
		// デバイスメモリからホストメモリへ読み込みが必要
		void	requireRead(VariableKind v);
		// ホストメモリからデバイスメモリへ書き込みが必要
		void	requireWrite(VariableKind v);
		// ホストメモリからデバイスメモリへすべてのメモリオブジェクトを書き込み
		void	flushWrite();
		// デバイスメモリからホストメモリへすべてのメモリオブジェクトを読み込む
		void	flushRead();
		template <class Matrix>
		void	addMemory(VariableKind kind, Matrix& m);
		// 指定した変数のメモリオブジェクトを取得
		cl_mem	getMemory(VariableKind v);
	};

	template <typename T>
	void monju::DeviceMemory::_queueWriteBuffer(cl_command_queue queue, cl_mem mem, size_t size, const T* p_data)
	{
		cl_int error_code = clEnqueueWriteBuffer(
			queue,
			mem,
			CL_TRUE,
			0,
			size,
			p_data,
			0,
			nullptr,
			nullptr);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}

	template <typename T>
	void monju::DeviceMemory::_queueReadBuffer(cl_command_queue queue, cl_mem mem, size_t size, T* p_data)
	{
		cl_int error_code = clEnqueueReadBuffer(
			queue,
			mem,
			CL_TRUE,
			0,
			size,
			p_data,
			0,
			nullptr,
			nullptr);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}
	template<class Matrix>
	void DeviceMemory::_queueTransferBuffer(cl_command_queue queue, cl_mem mem, Matrix& matrix, bool write)
	{
		if (write)
			_queueWriteBuffer<typename Matrix::Scalar>(queue, mem, matrix.size(), matrix.data());
		else
			_queueReadBuffer<typename Matrix::Scalar>(queue, mem, matrix.size(), matrix.data());
	}
	template<class Matrix>
	void DeviceMemory::_addNewClMem(VariableKind v, Matrix& m)
	{
		int bytes = sizeof(typename Matrix::Scalar) * m.size();
		cl_int error_code;
		cl_mem mem = clCreateBuffer(_p_device->getClContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, bytes, m.data(), &error_code);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		MemAttr ma = { m.data(), mem, (size_t)bytes };
		_map_mem.insert(std::pair<VariableKind, MemAttr>(v, ma));
	}
	template<class Matrix>
	inline void DeviceMemory::addMemory(VariableKind kind, Matrix& m)
	{
		_addNewClMem(kind, m);
	}
} // namespace monju

#endif // _MONJU_DEVICE_MEMORY_H__
