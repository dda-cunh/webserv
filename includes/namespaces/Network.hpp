#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdint.h>
#include <vector>

#include <string>

class Socket;

namespace Network
{
	std::string	iPV4PackedTos(uint32_t const&)			throw();
	uint32_t	sToIPV4Packed(std::string const&)		throw();
}
