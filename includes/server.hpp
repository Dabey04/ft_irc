#ifndef SERVER_HPP
#define SERVER_HPP

#include "irc.hpp"

class Server
{
	public:
		~Server();
		Server(char *port, char *pass);
		void Run();

	private:
		Server();
		Server &operator=(const Server &other);
		Server(const Server &other);

		void initSock();
		void SecurArg(const char *port, const char *pass);

		int					_port;
		int					_socket_fd;
		std::string			_pass;
		struct sockaddr_in	socket_adress;
};

#endif