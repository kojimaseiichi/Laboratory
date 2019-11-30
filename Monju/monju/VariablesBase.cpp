#include "VariablesBase.h"

// リソース開放


// 初期化

inline void monju::VariablesBase::create(PlatformContext& context, std::string id)
{
	_p_context = &context;
	_id = id;
}

inline void monju::VariablesBase::release()
{

}
