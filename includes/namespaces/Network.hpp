#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdint.h>
#include <vector>

#include <string>

typedef std::vector<unsigned char>	ByteArr;
class Socket;

namespace Network
{
	std::string	iPV4PackedTos(uint32_t const&)			throw();
	uint32_t	sToIPV4Packed(std::string const&)		throw();
	bool		nonBlockWrite(int const&, std::string &)	throw();
	bool		nonBlockRead(int const&, ByteArr &)		throw();
}
