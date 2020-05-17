#ifndef _MONJU_UNIFORM_BASIS_SHAPE_H__
#define _MONJU_UNIFORM_BASIS_SHAPE_H__

#include "Extent.h"

namespace monju
{
	class UniformBasisShape
	{
	public:
		Extent extent;
		uint32_t nodes;
		uint32_t units;

		bool checkExtent()
		{
			return nodes == extent.size();
		}

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, unsigned int /*version*/)
		{
			ar& boost::serialization::make_nvp("extent", extent);
			ar& boost::serialization::make_nvp("nodes", nodes);
			ar& boost::serialization::make_nvp("units", units);
		}

	};

}

#endif // !_MONJU_UNIFORM_BASIS_SHAPE_H__
