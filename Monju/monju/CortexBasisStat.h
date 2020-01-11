#pragma once
#ifndef _MONJU_CORTEX_BASIS_STAT_H__
#define _MONJU_CORTEX_BASIS_STAT_H__

#include "GridMatrixStorage.h"
#include "Closable.h"
#include "ConcurrencyContext.h"

namespace monju {

	// äÓíÍÇÃèüó¶Ç…ä÷Ç∑ÇÈìùåvó Çï€ë∂
	class CortexBasisStat : public Closable
	{
	private:
		using TStorage = TriangularGridMatrixStorage<int32_t>;

		std::string _id;
		int
			_nodes,
			_units_per_node;
		std::shared_ptr<TStorage> _storage;
		ConcurrencyContext _conc;

	public:
		CortexBasisStat(
			std::string id,
			int nodes,
			int units_per_node
		) : _conc(1)
		{
			_id = id;
			_nodes = nodes;
			_units_per_node = _units_per_node;
		}

		~CortexBasisStat()
		{
			close();
		}

		void persist(MatrixRm<int32_t>& win)
		{
			const auto task = [&sto = _tri_sto](int grow, int gcol, int row, int col) -> void
			{
				sto.addElement(grow, gcol, row, col, 1);
			};

			for (int i = 0; i < win.rows(); i++)
			{
				for (int ii = 0; ii < i; ii++)
				{
					auto f = _conc.threadPool().submit(task, i, ii, static_cast<int>(win(i)), static_cast<int>(win(ii)));
				}
			}
		}

		std::future<MatrixRm<float_t>*> calcPenalty(MatrixRm<float_t>* p)
		{

		}

		void create(std::string dir)
		{
			_storage = std::make_shared<TStorage>(storagePath(dir), _nodes, _units_per_node, _units_per_node);
		}

		void close()
		{
			_conc.close();
			if (_storage != nullptr)
			{
				_storage->close();
				_storage.reset();
			}
		}

	private:
		std::string storagePath(std::string dir) const
		{
			return util_file::combine(dir, _id + "_tri", "gma");
		}
	};
}

#endif // !_MONJU_CORTEX_BASIS_STAT_H__
