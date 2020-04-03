#include "PlatformContext.h"

#include "DeviceContext.h"

std::string monju::PlatformContext::full_path_to_prpperties_json(std::string work_folder) const
{
	boost::filesystem::path bwf = boost::filesystem::path(work_folder);
	boost::filesystem::path full_path = bwf / "properties.json";
	return full_path.string();
}

// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������

void monju::PlatformContext::_saveJson(const std::string work_folder) const
{
	boost::property_tree::ptree root;
	// root
	root.put("work_folder", work_folder);
	root.put("kernel_folder", _kernel_folder.string());
	{
		// deivce
		boost::property_tree::ptree device;
		device.put("platform_id", _platform_id);
		root.add_child("device", device);
	}
	boost::property_tree::write_json(full_path_to_prpperties_json(work_folder), root);
}

// �v���b�g�t�H�[�������t�@�C������ǂݍ���

void monju::PlatformContext::_loadJson(const std::string work_folder)
{
	boost::property_tree::ptree root;
	boost::property_tree::read_json(full_path_to_prpperties_json(work_folder), root);
	// root
	_work_folder = work_folder;
	_kernel_folder = root.get<std::string>("kernel_folder");
	{
		// deivce
		auto device = root.get_child("device");
		_platform_id = device.get<int>("platform_id");
	}
}

void monju::PlatformContext::_createDeviceContext(int platform_id)
{
	_pDeviceContext = new DeviceContext();
	_pDeviceContext->create(platform_id);
}

void monju::PlatformContext::_release()
{
	if (_pDeviceContext != nullptr)
	{
		_pDeviceContext->release();
		delete _pDeviceContext;
		_pDeviceContext = nullptr;
	}
}

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

void monju::PlatformContext::open(std::string work_folder)
{
	_work_folder = work_folder;
	_loadJson(work_folder);
	_createDeviceContext(_platform_id);
}

void monju::PlatformContext::create(std::string work_folder, std::string kernel_folder, int platform_id)
{
	_work_folder = work_folder;
	_kernel_folder = kernel_folder;
	_platform_id = platform_id;
	_saveJson(work_folder);
	_createDeviceContext(_platform_id);
}

// �v���b�g�t�H�[�������t�@�C���ɕۑ�����

void monju::PlatformContext::close()
{
	_saveJson(_work_folder.string());
	_release();
}

// ���[�N�X�y�[�X�f�B���N�g��

std::string monju::PlatformContext::workspaceDir()
{
	return _work_folder.string();
}

std::string monju::PlatformContext::kernelDir()
{
	return _kernel_folder.string();
}

// �v���b�g�t�H�[�������t�@�C������ǂݍ��ݗ��p�\�Ƃ���

// �v���b�g�t�H�[�������t�@�C���ɕۑ�����

// ���[�N�X�y�[�X�f�B���N�g��

// �f�o�C�X�R���e�L�X�g�i�v�Z�����j

monju::DeviceContext& monju::PlatformContext::deviceContext() const
{
	return *_pDeviceContext;
}
