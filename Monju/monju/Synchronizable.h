#pragma once
#ifndef _MONJU_SYNCHRONIZABLE_H__
#define _MONJU_SYNCHRONIZABLE_H__

#include <shared_mutex>

namespace monju {

	/* 前方宣言 */
	class Synchronizable;
	class ReadGuard;
	class WriteGuard;

	/// <summary>同期クラス</summary>
	class Synchronizable
	{
	private:
		std::shared_timed_mutex _shared_timed_mutex;

		friend class ReadGuard;
		friend class WriteGuard;
	};

	/// <summary>オブジェクトの生存中にリードロック</summary>
	class ReadGuard
	{
	private:
		std::shared_lock<std::shared_timed_mutex> _read_shared_lock;

	public:
		ReadGuard(Synchronizable* p) :
			_read_shared_lock(p->_shared_timed_mutex)
		{
		}
		ReadGuard(Synchronizable& r) :
			_read_shared_lock(r._shared_timed_mutex)
		{
		}
	};

	/// <summary>オブジェクトの生存中にライトロック</summary>
	class WriteGuard
	{
	private:
		std::unique_lock<std::shared_timed_mutex> _write_unique_lock;

	public:
		WriteGuard(Synchronizable* p) :
			_write_unique_lock(p->_shared_timed_mutex)
		{
		}
		WriteGuard(Synchronizable& r) :
			_write_unique_lock(r._shared_timed_mutex)
		{
		}
	};
}

#endif // _MONJU_SYNCHRONIZABLE_H__