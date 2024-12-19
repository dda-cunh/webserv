#pragma once

#include <stdint.h>
#include <string>

namespace Network
{
	std::string	iPV4PackedTos(uint32_t const&)			throw();
	uint32_t	sToIPV4Packed(std::string const&)		throw();
}
