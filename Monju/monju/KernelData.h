#ifndef _MONJU_KERNEL_DATA_H__
#define _MONJU_KERNEL_DATA_H__

#include <CL/cl_platform.h>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace monju
{
	// OpenCLÉJÅ[ÉlÉãÇÃà¯êî
#pragma pack(push, 1)
	class cell_addr
	{
	public:
		cl_int grid_row;
		cl_int grid_col;
		cl_int cell_index;

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("grid_row", grid_row);
			ar& boost::serialization::make_nvp("grid_col", grid_col);
			ar& boost::serialization::make_nvp("cell_index", cell_index);
		}
	};
#pragma pack(pop)

}

#endif // !_MONJU_KERNEL_DATA_H__
