#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"
#include "ConvLambdaInputDevice.h"
#include "ConvLambdaInput.h"

monju::ConvLambdaInputDevice::ConvLambdaInputDevice(std::weak_ptr<ClMachine> clMachine, ConvLambdaInput* pInput, std::weak_ptr<MatrixRm<float_t>> lambda)
{
	_clMachine = clMachine.lock();

	_image = pInput->image().lock();
	_lambda = pInput->lambda().lock();
	_cpt = pInput->cpt().lock();

	_clInput = std::make_shared<ClMemory>(_clMachine, _image->size() * sizeof(decltype(_image)::element_type::Scalar));
	_clCpt = std::make_shared<ClMemory>(_clMachine, _cpt->size() * sizeof(decltype(_cpt)::element_type::Scalar));
	_clLambda = std::make_shared<ClMemory>(_clMachine, _lambda->size() * sizeof(decltype(_lambda)::element_type::Scalar));

	_clVariableSet = std::make_unique<ClVariableSet>();
	_clVariableSet->add<MatrixRm<float_t>>(_image, VariableKind::IMAGE, _clInput);
	_clVariableSet->add<MatrixCm<float_t>>(_cpt, VariableKind::W, _clCpt);
	_clVariableSet->add<MatrixRm<float_t>>(_lambda, VariableKind::lambda, _clLambda);
}

monju::ConvLambdaInputDevice::~ConvLambdaInputDevice()
{

}

monju::ClVariableSet& monju::ConvLambdaInputDevice::clVariableSet()
{
	return *_clVariableSet;
}
