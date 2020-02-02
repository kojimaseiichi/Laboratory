#include "DeviceContext.h"

// �R�[���o�b�N�֐�
void CL_CALLBACK notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
		std::cerr << (errinfo != nullptr ? errinfo : "") << std::endl;
}


// �v���b�g�t�H�[�����q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃v���b�g�t�H�[�����擾

// �f�o�C�X���q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃f�o�C�X���擾
// GPU�P�ł���Ζ��Ȃ����A������GPU�𓋍ڂ���ꍇ�͕����̃f�o�C�X���擾����悤�ɂ���B

std::vector<cl_device_id> monju::DeviceContext::_listDevices(cl_platform_id platform_id)
{
	cl_uint num_devices = 0;
	cl_device_id devices_buff[4];
	cl_int error_code = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 4, devices_buff, &num_devices);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return std::vector<cl_device_id>(devices_buff, devices_buff + num_devices);
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
