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

	// ����̃f�o�C�X�iGPU�j�̃��������Ǘ�
	// �f�o�C�X�������ƃz�X�g�������Ԃ̃f�[�^�]�����Ǘ�
	// DeviceKernel�ƘA�g���ăJ�[�l���̎��s����������
	class DeviceMemory
	{
	protected:
		// OpenCL�������I�u�W�F�N�g
		struct MemAttr
		{
			float* p;
			cl_mem mem;
			size_t bytes;
		};

		// �f�[�^
	protected:
		DeviceContext* _p_dc;
		Device* _p_device;
		// �ҏW�̎�ނ��ƂɃz�X�g�������ƃf�o�C�X��������ێ�
		std::unordered_map<VariableKind, MemAttr> _map_mem;
		boost::dynamic_bitset<>		_read_required;
		boost::dynamic_bitset<>		_write_required;

		// �������E����
	protected:
		DeviceMemory();
		virtual ~DeviceMemory();

		// �R�s�[�֎~
	protected:
		DeviceMemory(const DeviceMemory&) = delete;
		DeviceMemory& operator=(const DeviceMemory&) = delete;

		// �v���p�e�B
	public:
		DeviceContext& deviceContext() { return *_p_dc; }
		Device& device() { return *_p_device; }
		boost::dynamic_bitset<> read_required() { return _read_required; }
		boost::dynamic_bitset<> write_required() { return _write_required; }

		// �w���p
	protected:
		// �������]���@CPU -> GPU
		template <typename T>
		void	_queueWriteBuffer(cl_command_queue queue, cl_mem mem, size_t size, const T* p_data);
		// �������]���@GPU -> CPU
		template <typename T>
		void	_queueReadBuffer(cl_command_queue queue, cl_mem mem, size_t size, T* p_data);
		// �������]���@CPU <-> GPU
		template <class Matrix>
		void	_queueTransferBuffer(cl_command_queue queue, cl_mem mem, Matrix& matrix, bool write);
		// �������]���@�s���ǉ����A�Ή�����GPU�����������蓖�Ă�
		template <class Matrix>
		void	_addNewClMem(VariableKind v, Matrix& m);
		// �ێ����Ă���s����폜���A�Ή�����GPU���������J������
		void	_clearAllClMem();
		// �ێ����Ă���s��f�[�^����A�w�肵���ϐ��̍s��f�[�^���擾����B
		bool	_findClMem(VariableKind v, MemAttr* out);
		// ��������ѐ����`�F�b�N�i�z�X�g�������ƃf�o�C�X�������̗������ύX����Ă����ԁj
		bool	_checkReadWriteConflict();


		// ���z�֐�
	public:
		// ���\�[�X�̊J���i�I�u�W�F�N�g�̃��C�t�T�C�N���Ǘ��̋��ʊ֐��j
		virtual void	release();

		// ���J�֐�
	public:
		// ������
		void	create(DeviceContext& dc, Device& device);
		// �z�X�g����������f�o�C�X�������֏������݁i�f�o�C�X�w��j
		void	writeBuffer(Device& device, boost::dynamic_bitset<> variableKindSet);
		// �z�X�g����������f�o�C�X�������֏�������
		void	writeBuffer(boost::dynamic_bitset<> variableKindSet);
		// �f�o�C�X����������z�X�g�������֓ǂݍ��݁i�f�o�C�X�w��j
		void	readBuffer(Device& device, boost::dynamic_bitset<> variableKindSet);
		// �f�o�C�X����������z�X�g�������֓ǂݍ���
		void	readBuffer(boost::dynamic_bitset<> variableKindSet);
		// �f�o�C�X����������z�X�g�������֓ǂݍ��݂��K�v
		void	requireRead(VariableKind v);
		// �z�X�g����������f�o�C�X�������֏������݂��K�v
		void	requireWrite(VariableKind v);
		// �z�X�g����������f�o�C�X�������ւ��ׂẴ������I�u�W�F�N�g����������
		void	flushWrite();
		// �f�o�C�X����������z�X�g�������ւ��ׂẴ������I�u�W�F�N�g��ǂݍ���
		void	flushRead();
		template <class Matrix>
		void	addMemory(VariableKind kind, Matrix& m);
		// �w�肵���ϐ��̃������I�u�W�F�N�g���擾
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
