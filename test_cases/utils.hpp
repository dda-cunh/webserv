#pragma once

#include <map>
#include <vector>
#include "ServerLocation.hpp"

class ServerLocation;

typedef std::map<std::string, std::string>		StrStrMap;
typedef std::map<int, std::string>				IntStrMap;
typedef std::vector<ServerLocation *>			LocationBlocks;

namespace Network
{
	std::string	iPV4PackedTos(uint32_t const&)		throw();
	uint32_t	sToIPV4Packed(std::string const&)	throw();
}

namespace Http
{
	typedef enum	e_method
	{
		M_UNHANDLED,
		M_DELETE,
		M_POST,
		M_GET,
		__LEN_METHOD
	}	METHOD;
}

typedef enum	e_location_block_type
{
	L_UNHANDLED,
	L_STATIC,
	L_REV_PROXY,
	L_CGI
}	LOCATION_BLOCK_TYPE;

