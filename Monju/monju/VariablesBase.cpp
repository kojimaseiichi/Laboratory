#include "VariablesBase.h"

// ���\�[�X�J��


// ������

inline void monju::VariablesBase::create(PlatformContext& context, std::string id)
{
	_p_context = &context;
	_id = id;
}

inline void monju::VariablesBase::release()
{

}
