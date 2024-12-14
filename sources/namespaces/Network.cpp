#include "../../includes/webserv.hpp"

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

	static uint8_t	sOctectToByte(std::string const& octect)
	{
		char	*end_ptr;
		long	value;

		value = std::strtol(octect.c_str(), &end_ptr, 10);
		if (octect.c_str() == end_ptr)
			throw (ExceptionMaker("Invalid octect string"));
		if (value < 0 || value > 255)
			throw (ExceptionMaker("String isn't an octect"));
		return (value);
	}

	uint32_t	sToIPV4Packed(std::string const& ipv4S)	throw()
	{
		std::stringstream	ss(ipv4S);
		std::string			octect;
		uint8_t				octects[4];

		if (Utils::sCountChar(ipv4S, '.') != 3)
		{
			LogFeed::getInstance().buff("String isn't in IPV4", Utils::LOG_WARNING);
			return (-1);
		}
		for (int8_t i = 0; i < 4; i++)
		{
			if (!std::getline(ss, octect, '.'))
			{
				LogFeed::getInstance().buff("String isn't in IPV4", Utils::LOG_WARNING);
				return (-1);
			}
			try
			{
				octects[i] = sOctectToByte(octect);
			}
			catch (ExceptionMaker const& ex)
			{
				LogFeed::getInstance().buff(ex.what(), Utils::LOG_WARNING);
				return (-1);
			}
		}
		return (octects[0] << 24
				| octects[1] << 16
				| octects[2] << 8
				| octects[3]);
	}
}
