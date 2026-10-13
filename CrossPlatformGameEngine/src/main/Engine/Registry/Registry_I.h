#pragma once

#include <cstdint>

#include <string>
#include <unordered_map>

namespace ascen
{

	template<typename Entry, typename Resource>
	class Registry_I
	{
	public:
		using EntryType = Entry;
		using ResourceType = Resource;

		virtual ~Registry_I() = default;

		virtual void reconstruct(const Entry& entry, Resource& resource) = 0;
	};

}
