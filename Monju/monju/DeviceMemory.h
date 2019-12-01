#pragma once
#ifndef _MONJU_DEVICE_MEMORY_H__
#define _MONJU_DEVICE_MEMORY_H__

#include <string>
#include <CL/cl.h>
#include <fstream>
#include <map>
#include <boost/dynamic_bitset.hpp>

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
		DeviceContext* _p_dc;
		Device* _p_device;
		// 編集の種類ごとにホストメモリとデバイスメモリを保持
		std::unordered_map<VariableKind, MemAttr> _map_mem;
		boost::dynamic_bitset<>		_read_required;
		boost::dynamic_bitset<>		_write_required;

		// 初期化・生成
	protected:
		DeviceMemory();
		virtual ~DeviceMemory();

		// コピー禁止
	protected:
		DeviceMemory(const DeviceMemory&) = delete;
		DeviceMemory& operator=(const DeviceMemory&) = delete;

		// プロパティ
	public:
		DeviceContext& deviceContext() { return *_p_dc; }
		Device& device() { return *_p_device; }
		boost::dynamic_bitset<> read_required() { return _read_required; }
		boost::dynamic_bitset<> write_required() { return _write_required; }

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
		// 初期化
		void	create(DeviceContext& dc, Device& device);
		// ホストメモリからデバイスメモリへ書き込み（デバイス指定）
		void	writeBuffer(Device& device, boost::dynamic_bitset<> variableKindSet);
		// ホストメモリからデバイスメモリへ書き込み
		void	writeBuffer(boost::dynamic_bitset<> variableKindSet);
		// デバイスメモリからホストメモリへ読み込み（デバイス指定）
		void	readBuffer(Device& device, boost::dynamic_bitset<> variableKindSet);
		// デバイスメモリからホストメモリへ読み込み
		void	readBuffer(boost::dynamic_bitset<> variableKindSet);
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
		clEnqueueWriteBuffer(
			queue,
			mem,
			CL_TRUE,
			0,
			sizeof(T) * size,
			p_data,
			0,
			nullptr,
			nullptr);
	}

	template <typename T>
	void monju::DeviceMemory::_queueReadBuffer(cl_command_queue queue, cl_mem mem, size_t size, T* p_data)
	{
		clEnqueueReadBuffer(
			queue,
			mem,
			CL_TRUE,
			0,
			sizeof(T) * size,
			p_data,
			0,
			nullptr,
			nullptr);
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
		cl_mem mem = clCreateBuffer(_p_dc->getContext(), CL_MEM_READ_WRITE, bytes, nullptr, nullptr);
		MemAttr ma = { m.data(), mem, bytes };
		_map_mem.insert(std::pair<VariableType, MemAttr>(v, ma));
	}
	template<class Matrix>
	inline void DeviceMemory::addMemory(VariableKind kind, Matrix& m)
	{
		_addNewClMem(kind, m);
	}
} // namespace monju

#endif // _MONJU_DEVICE_MEMORY_H__
