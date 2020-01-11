#pragma once
#ifndef _MONJU_CLOSABLE_H__
#define _MONJU_CLOSABLE_H__

namespace monju {

	class Closable
	{
		virtual void close() = 0;
	};
}

#endif // !_MONJU_CLOSABLE_H__

