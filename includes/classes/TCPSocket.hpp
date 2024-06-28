#pragma once

#include <cstdint>
#include <stdint.h>

#include <ostream>

class TCPSocket
{
	public:
		~TCPSocket();

		TCPSocket(uint64_t const&, uint32_t const&,
				int const&)	throw();

		bool	operator==(TCPSocket const & rhs)	const;

		std::string const	address()				const;
		uint16_t const		port()					const;
		int const			fd()					const;
		void				badSyscallThrow();
		void				connect();

	private:
		uint64_t const	_address;
		uint32_t const	_port;
		int const		_backlog;
		int				_fd;

		TCPSocket();
		TCPSocket(TCPSocket const& src);
		TCPSocket & operator=(TCPSocket const& rhs);
};

std::ostream &	operator<<(std::ostream &, TCPSocket const&);