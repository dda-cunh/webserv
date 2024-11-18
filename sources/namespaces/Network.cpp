#include "../../includes/webserv.hpp"

namespace Network
{
	//	Return true if theres more data to be processed
	//	fd has to be a multiplexed fd ready for read
	bool	nonBlockRead(int const& fd, ByteArr & curr) throw()
	{
		unsigned char	buff[CLIENT_CHUNK_SIZE];
		long			bytesRead;

		if (fd < 0)
			return (false);
		bytesRead = read(fd, buff, CLIENT_CHUNK_SIZE);
		if (bytesRead > 0)
		{
			curr.insert(curr.end(), buff, buff + bytesRead);
			if (bytesRead < CLIENT_CHUNK_SIZE)
				return (false);
		}
		return (bytesRead != -1 && bytesRead != 0);
	}

	//	Return true if theres more data to be processed
	//	fd has to be a multiplexed fd ready for write
	bool	nonBlockWrite(int const& fd, std::string & data)	throw()
	{
		ssize_t		bytesSent;

		if (fd < 0)
			return (false);
		bytesSent = write(fd, data.c_str(), data.length());
		if (bytesSent > 0)
			data = data.substr(bytesSent);
		if (data.empty() || bytesSent == 0 || bytesSent == -1)
			return (false);
		return (true);
	}

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
			LOG("String isn't in IPV4", Utils::LOG_WARNING);
			return (-1);
		}
		for (int8_t i = 0; i < 4; i++)
		{
			if (!std::getline(ss, octect, '.'))
			{
				LOG("String isn't in IPV4", Utils::LOG_WARNING);
				return (-1);
			}
			try
			{
				octects[i] = sOctectToByte(octect);
			}
			catch (ExceptionMaker const& ex)
			{
				LOG(ex.what(), Utils::LOG_WARNING);
				return (-1);
			}
		}
		return (octects[0] << 24
				| octects[1] << 16
				| octects[2] << 8
				| octects[3]);
	}
}
