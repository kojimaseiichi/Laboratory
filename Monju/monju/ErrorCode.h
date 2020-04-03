#ifndef _MONJU_ERROR_CODE_H__
#define _MONJU_ERROR_CODE_H__

namespace monju
{
	enum class ErrorCode
	{
		Success = 0,
		Error = 1,
		WeakPointerExpired = 2
	};

}

#endif // !_MONJU_ERROR_CODE_H__
