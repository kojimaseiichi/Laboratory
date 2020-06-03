#pragma once
#ifndef _MONJU_EXTENT_H__
#define _MONJU_EXTENT_H__

#include "MonjuTypes.h"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace monju
{
	// ‘O•ûéŒ¾
	class Extent;
	class Entry;
	class GridExtent;
	class GridEntry;

	class Extent
	{
	public:
		int32_t rows, cols;

		Extent();
		Extent(int32_t rows, int32_t cols);
		Extent(const Extent& o);
		Extent& operator =(const Extent& o);
		bool operator ==(const Extent& o);
		void set(int32_t rows, int32_t cols)
		{
			this->rows = rows;
			this->cols = cols;
		}
		int32_t size() const;
		bool isSquare() const;
		bool contains(const Entry& e) const;
		bool contains(const int32_t row, const int32_t col) const;
		int32_t linearId(const int32_t row, const int32_t col) const;

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

		Entry();
		Entry(int32_t row, int32_t col);
		Entry(const Entry& o);
		Entry& operator =(const Entry& o);
		bool operator ==(const Entry& o);
		int32_t linearId(const Extent& extent, const int major) const;

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("row", row);
			ar& boost::serialization::make_nvp("col", col);
		}
	};

	// TODO LayerShape -> LayerExtent
	class LayerShape
	{
	public:
		Extent nodes, units;

		LayerShape();
		LayerShape(const LayerShape& o);
		LayerShape(Extent nodes, Extent units);
		LayerShape(int32_t nodes_rows, int32_t nodes_cols, int32_t units_rows, int32_t units_cols);
		LayerShape& operator=(const LayerShape& o);
		bool operator ==(const LayerShape& o);
		Extent flatten();
		GridExtent asGridExtent();

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("nodes", nodes);
			ar& boost::serialization::make_nvp("units", units);
		}
	};

	class GridExtent
	{
	public:
		Extent grid, matrix;

		GridExtent();
		GridExtent(const GridExtent& o);
		GridExtent(Extent grid, Extent matrix);
		GridExtent(const int grid_rows, const int grid_cols, const int mat_rows, const int mat_cols);
		GridExtent& operator=(const GridExtent& o);
		bool operator ==(const GridExtent& o);
		int32_t size() const;
		bool contains(const GridEntry& e) const;
		bool contains(const int32_t grid_row, const int32_t grid_col, const int32_t row, const int32_t col) const;
		void setCpt(const LayerShape& x, const LayerShape& y)
		{
			grid.set(y.nodes.size(), x.nodes.size());
			matrix.set(y.units.size(), x.nodes.size());
		}
		Extent flattenRm() const
		{
			return Extent(grid.rows, grid.cols * matrix.size());
		}
		Extent flattenCm() const
		{
			return Extent(grid.rows * matrix.size(), grid.cols);
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

		GridEntry();
		GridEntry(const GridEntry& o);
		GridEntry(Entry grid, Entry matrix);
		GridEntry& operator=(const GridEntry& o);
		bool operator ==(const GridEntry& o);
		int32_t linearId(const GridExtent& extent, const int grid_major, const int matrix_major) const;

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
