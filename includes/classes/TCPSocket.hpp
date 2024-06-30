#pragma once

#include <stdint.h>

#include <ostream>

class TCPSocket
{
	public:
		TCPSocket & operator=(TCPSocket const& rhs)				throw();
		TCPSocket(TCPSocket const& src)							throw();
		~TCPSocket()											throw();

		TCPSocket(uint32_t const& ipv4, uint16_t const& port,
				int const& backlog)								throw();

		bool	operator==(TCPSocket const & rhs)	const		throw();

		std::string			address()				const		throw();
		std::string			str()					const		throw();
		uint16_t 			port()					const		throw();
		int 				fd()					const		throw();

		void				badSyscallThrow();
		void				disconnect()						throw();
		void				connect();

	private:
		uint32_t const	_address;
		uint16_t const	_port;
		int const		_backlog;
		int				_fd;


		TCPSocket()												throw();
};

std::ostream &	operator<<(std::ostream &, TCPSocket const&)	throw();
