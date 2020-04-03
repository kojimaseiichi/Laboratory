#pragma once
#ifndef _MONJU_DEVICE_MEMORY_H__
#define _MONJU_DEVICE_MEMORY_H__

#include "MonjuTypes.h"
#include "OpenClException.h"

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
			void* p;
			cl_mem mem;
			size_t bytes;
		};

		// �f�[�^
	protected:
		Device* _p_device;
		// �ҏW�̎�ނ��ƂɃz�X�g�������ƃf�o�C�X��������ێ�
		std::unordered_map<VariableKind, MemAttr>	_map_mem;
		std::set<VariableKind>			_read_required;
		std::set<VariableKind>			_write_required;

		// �������E����
	public:
		DeviceMemory(Device& device);
		~DeviceMemory();

		// �R�s�[�֎~�E���[�u�֎~
	protected:
		DeviceMemory(const DeviceMemory&) = delete;
		DeviceMemory(DeviceMemory&&) = delete;
		DeviceMemory& operator=(const DeviceMemory&) = delete;
		DeviceMemory& operator=(const DeviceMemory&&) = delete;
		

		// �v���p�e�B
	public:
		Device& device() { return *_p_device; }
		std::set<VariableKind> read_required() { return _read_required; }
		std::set<VariableKind> write_required() { return _write_required; }

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

		void	_writeBuffer(Device& device, VariableKind kind);
		void	_readBuffer(Device& device, VariableKind kind);

		// ���z�֐�
	public:
		// ���\�[�X�̊J���i�I�u�W�F�N�g�̃��C�t�T�C�N���Ǘ��̋��ʊ֐��j
		virtual void	release();

		// ���J�֐�
	public:
		// �z�X�g����������f�o�C�X�������֏������݁i�f�o�C�X�w��j
		void	writeBuffer(Device& device, VariableKind kind);
		// �z�X�g����������f�o�C�X�������֏������݁i�f�o�C�X�w��j
		void	writeBuffer(Device& device, std::set<VariableKind>& variableKindSet);
		// �z�X�g����������f�o�C�X�������֏�������
		void	writeBuffer(std::set<VariableKind>& variableKindSet);
		// �z�X�g����������f�o�C�X�������֏�������
		void	writeBuffer(VariableKind kind);
		// �f�o�C�X����������z�X�g�������֓ǂݍ��݁i�f�o�C�X�w��j
		void	readBuffer(Device& device, VariableKind kind);
		// �f�o�C�X����������z�X�g�������֓ǂݍ��݁i�f�o�C�X�w��j
		void	readBuffer(Device& device, std::set<VariableKind>& variableKindSet);
		// �f�o�C�X����������z�X�g�������֓ǂݍ���
		void	readBuffer(std::set<VariableKind>& variableKindSet);
		// �f�o�C�X����������z�X�g�������֓ǂݍ���
		void	readBuffer(VariableKind kind);
		// �f�o�C�X����������z�X�g�������֓ǂݍ��݂��K�v
		void	requireRead(VariableKind v);
		void	requireRead(std::set<VariableKind>& variablesToRead);
		// �z�X�g����������f�o�C�X�������֏������݂��K�v
		void	requireWrite(VariableKind v);
		void	requireWrite(std::set<VariableKind>& variablesToWrite);
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
		size_t bytes = sizeof(typename Matrix::Scalar) * m.size();
		cl_int error_code;
		cl_mem mem = clCreateBuffer(_p_device->getClContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, bytes, m.data(), &error_code);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		MemAttr ma = { m.data(), mem, (size_t)bytes };
		_map_mem.insert(std::pair<VariableKind, MemAttr>(v, ma));
	}
	template<class Matrix>
	void DeviceMemory::addMemory(VariableKind kind, Matrix& m)
	{
		_addNewClMem(kind, m);
	}
} // namespace monju

#endif // _MONJU_DEVICE_MEMORY_H__
