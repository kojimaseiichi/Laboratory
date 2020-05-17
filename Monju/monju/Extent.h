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

		int32_t rows;
		int32_t cols;

		bool operator ==(const Extent& o)
		{
			return rows == o.rows && cols == o.cols;
		}
		int32_t size() const
		{
			return rows * cols;
		}
		int32_t linearId(int32_t row, int32_t col) const
		{
			return row * cols + col;
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

}

#endif // !_MONJU_EXTENT_H__
