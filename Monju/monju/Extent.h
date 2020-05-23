#ifndef _MONJU_EXTENT_H__
#define _MONJU_EXTENT_H__

#include <cstdint>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace monju
{
	class Extent
	{
	public:
		int32_t rows, cols;

		Extent()
		{
			rows = cols = 0;
		}
		Extent(const Extent& o)
		{
			rows = o.rows;
			cols = o.cols;
		}
		Extent& operator =(const Extent& o)
		{
			rows = o.rows;
			cols = o.cols;
		}
		bool operator ==(const Extent& o)
		{
			return rows == o.rows && cols == o.cols;
		}
		int32_t size() const
		{
			return rows * cols;
		}

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("rows", rows);
			ar& boost::serialization::make_nvp("cols", cols);
		}
	};

	class Entry
	{
	public:
		int32_t row, col;

		Entry()
		{
			row = col = 0;
		}
		Entry(const Entry& o)
		{
			row = o.row;
			col = o.col;
		}
		Entry& operator =(const Entry& o)
		{
			row = o.row;
			col = o.col;
		}
		bool operator ==(const Entry& o)
		{
			return row == o.row && col == o.col;
		}
		int32_t linearId(const Extent& extent, const int major) const
		{
			if (major == kRowMajor)
				return row * extent.cols + col;
			return row + col * extent.rows;
		}

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("row", row);
			ar& boost::serialization::make_nvp("col", col);
		}
	};

	class GridExtent
	{
	public:
		Extent grid, matrix;

		GridExtent()
		{
		}
		GridExtent(const GridExtent& o)
		{
			grid = o.grid;
			matrix = o.matrix;
		}
		bool operator ==(const GridExtent& o)
		{
			return grid == o.grid && matrix == o.matrix;
		}
		int32_t size() const
		{
			return grid.size()* matrix.size();
		}

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("grid", grid);
			ar& boost::serialization::make_nvp("matrix", matrix);
		}
	};

	class GridEntry
	{
	public:
		Entry grid, matrix;

		GridEntry() {}
		GridEntry(const GridEntry& o)
		{
			grid = o.grid;
			matrix = o.matrix;
		}
		GridEntry& operator=(const GridEntry& o)
		{
			grid = o.grid;
			matrix = o.matrix;
		}
		bool operator ==(const GridEntry& o)
		{
			return grid == o.grid && matrix == o.matrix;
		}
		int32_t linearId(const GridExtent& extent, const int grid_major, const int matrix_major) const
		{
			return 
				grid.linearId(extent.grid, grid_major) * extent.grid.size() + 
				matrix.linearId(extent.matrix, matrix_major);
		}

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("grid", grid);
			ar& boost::serialization::make_nvp("matrix", matrix);
		}
	};

}

#endif // !_MONJU_EXTENT_H__
