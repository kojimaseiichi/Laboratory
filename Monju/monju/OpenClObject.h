#pragma once
#ifndef _MONJU_OPEN_CL_OBJECT_H__
#define _MONJU_OPEN_CL_OBJECT_H__

#include <CL/cl.h>

namespace monju
{
	class OpenClObject
	{
	protected:

	protected:
		OpenClObject()
		{

		}
		~OpenClObject();

		virtual void dispose() = 0;
	};

}

#endif // !_MONJU_OPEN_CL_OBJECT_H__
