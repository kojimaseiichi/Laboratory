#include "DeviceContext.h"

#include "MonjuException.h"
#include "Device.h"
#include "OpenClException.h"

// �R�[���o�b�N�֐�
void CL_CALLBACK notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
		std::cerr << (errinfo != nullptr ? errinfo : "") << std::endl;
}


// �v���b�g�t�H�[�����q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃v���b�g�t�H�[�����擾

// �f�o�C�X���q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃f�o�C�X���擾
// GPU�P�ł���Ζ��Ȃ����A������GPU�𓋍ڂ���ꍇ�͕����̃f�o�C�X���擾����悤�ɂ���B

monju::DeviceContext::DeviceContext()
{
	_context = nullptr;
	_platform_id = nullptr;;
}

monju::DeviceContext::~DeviceContext()
{
	release();
}

cl_platform_id monju::DeviceContext::_findPlatform(int platform_idx)
{
	cl_uint num_platforms = 0;
	cl_platform_id platforms_buff[2];
	cl_int error_code = clGetPlatformIDs(2, platforms_buff, &num_platforms);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	if (platform_idx >= static_cast<int>(num_platforms))
		throw MonjuException();
	// �v���b�g�t�H�[���I��
	return platforms_buff[platform_idx];
}

std::vector<cl_device_id> monju::DeviceContext::_listDevices(cl_platform_id platform_id)
{
	cl_uint num_devices = 0;
	cl_device_id devices_buff[4];
	cl_int error_code = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 4, devices_buff, &num_devices);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return std::vector<cl_device_id>(devices_buff, devices_buff + num_devices);
}

void monju::DeviceContext::_releaseDevices(std::vector<cl_device_id>* p_device_id_vec)
{
	// �f�o�C�X�̊J����clReleaseDevice���g�p���Ă͂����Ȃ��I�I
	// ����API�̓T�u�f�o�C�X�̊J���Ŏg��
	p_device_id_vec->clear();
}

cl_context monju::DeviceContext::_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& device_id_vec)
{
	cl_int errorcode;
	cl_context context = clCreateContext(nullptr, static_cast<cl_uint>(device_id_vec.size()), device_id_vec.data(), notify_error, this, &errorcode);
	if (errorcode != CL_SUCCESS)
		throw MonjuException();
	return context;
}

std::vector<monju::Device*> monju::DeviceContext::_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec)
{
	std::vector<Device*> devices;
	for (auto device_id : device_id_vec)
	{
		Device* p = new Device();
		p->create(_context, device_id);
		devices.push_back(p);
	}
	return devices;
}

void monju::DeviceContext::_freeDeviceVec(std::vector<Device*>* p_device_vec)
{
	for (Device* p : *p_device_vec)
		p->release();
	p_device_vec->clear();
}

// OpenCL�v���b�g�t�H�[��������
// �v���b�g�t�H�[��ID�̓V�X�e���ŗL�ŊO������^������

void monju::DeviceContext::create(int platform_idx)
{
	// id����C���X�^���X�𐶐�
	_platform_id = _findPlatform(platform_idx);
	_device_id_vec = _listDevices(_platform_id);
	_context = _createContext(_platform_id, _device_id_vec);
	_device_vec = _allocDeviceVec(_context, _device_id_vec);
}

// OpenCL�v���b�g�t�H�[�����

void monju::DeviceContext::release()
{
	_freeDeviceVec(&_device_vec);
	if (_context != nullptr)
	{
		cl_int error_code = clReleaseContext(_context);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_context = nullptr;
	}
	_device_id_vec.clear();
	_releaseDevices(&_device_id_vec);
	_platform_id = nullptr;
}

cl_platform_id monju::DeviceContext::getPlatformId() const
{
	return _platform_id;
}

cl_context monju::DeviceContext::getContext() const
{
	return _context;
}

int monju::DeviceContext::numDevices() const
{
	return static_cast<int>(_device_vec.size());
}

monju::Device& monju::DeviceContext::getDevice(int idx) const
{
	return *(_device_vec.at(static_cast<size_t>(idx)));
}

// ���ׂẴf�o�C�X���擾

std::vector<monju::Device*> monju::DeviceContext::getAllDevices() const
{
	return _device_vec;
}
