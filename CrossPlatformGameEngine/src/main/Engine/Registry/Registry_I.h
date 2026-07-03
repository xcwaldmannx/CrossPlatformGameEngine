#pragma once

#include <cstdint>

namespace ascen
{

	template<typename T>
	class Registry_I
	{
	public:
		virtual ~Registry_I() = default;

		virtual uint64_t registerResource(const T& data) = 0;
		virtual void reconstruct() = 0;
	};

}
