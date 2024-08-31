#include "utils.hpp"

namespace Network
{
	std::string	iPV4PackedTos(uint32_t const& packed)		throw()
	{
		std::stringstream	ss;

		ss << ((packed >> 24) & 0xFF) << '.'
		<< ((packed >> 16) & 0xFF) << '.'
		<< ((packed >> 8) & 0xFF) << '.'
		<< (packed & 0xFF);
		return (ss.str());
	}
}