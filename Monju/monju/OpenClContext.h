#pragma once
#ifndef _MONJU_OPEN_CL_CONTEXT_H__
#define _MONJU_OPEN_CL_CONTEXT_H__

#include "ClPlatformId.h"
#include "ClContext.h"
#include "ClCommandQueues.h"
#include <memory>

namespace monju
{
	class OpenClContext
	{
	private:
		int _platformId;
		std::shared_ptr<ClPlatformId> _clPlatformId;	// âï˙ó\íË
		std::shared_ptr<ClContext> _clContext;			// âï˙ó\íË
		std::vector<std::shared_ptr<ClCommandQueues> > _clQueueArr;	// âï˙ó\íË

		void _createPlatformId();
		void _createContext();
		void _createCommandQueue();
		void _releasePlatformId();
		void _releaseContext();
		void _releaseCommandQueue();
	public:
		OpenClContext(int platformId);
		~OpenClContext();
		std::weak_ptr<ClPlatformId> clPlatformId() const;
		std::weak_ptr<ClContext> clContext() const;
		std::weak_ptr<ClCommandQueues> clCommandQueue(int n) const;

		// ÉRÉsÅ[ã÷é~ÅEÉÄÅ[Éuã÷é~
	public:
		OpenClContext(const OpenClContext&) = delete;
		OpenClContext(OpenClContext&&) = delete;
		OpenClContext& operator=(const OpenClContext&) = delete;
		OpenClContext& operator=(OpenClContext&&) = delete;
	};
}

#endif // !_MONJU_OPEN_CL_CONTEXT_H__

