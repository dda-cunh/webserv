#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;

uint32_t		ConfigParser::parseHost(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			//	GET 2ND WORD
			//	DETERMINE IF ':' IS PRESENT
			//	IF YES, GET VALUE AT LEFT SIDE
			//	ELSE, DETERMINE IF ITS AN IP ADDR
			//	RETURN IP IF YES, RETURN DEFAULT OTHERWISE
		}
	}

	return (Network::sToIPV4Packed(DEFAULT_HOST));
}

uint16_t	ConfigParser::parsePort(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			//	SAME AS PARSEHOST, BUT CHECK FOR PORT INSTEAD
		}
	}

	return (DEFAULT_PORT);
}

std::string	ConfigParser::parseServerName(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("server_name") == 0)
		{
			//	RETURN SECOND WORD
		}
	}

	return (DEFAULT_SERVER_NAME);
}