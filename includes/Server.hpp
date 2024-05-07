#ifndef SERVER_HPP
#define SERVER_HPP

#include "Irc.hpp"
#include "Client.hpp"

class Client;

class Server
{
	public:
		static bool	Signal;//variable static

		Server();
		// Server(const Server &other);
		~Server();

		// Server &operator=(const Server &other);

		// void			Run();

		void			securArg(const char *port, const char *pass);
		void			creatSocket();
		void			initServer(char *port, char *pass);
		void			newClient();
		void			mewDataClient(int fd);
		static void		signalHandler(int signum);

		void			closeFds();
		void			clearClient(int fd);

	private:
		int							_port;//
		int							_socket_fd;//
		static bool					_signal;//
		std::vector<Client>			_clients;//
		std::vector<struct pollfd>	_fds;//
		std::string					_pass;
		struct sockaddr_in			_socket_add;
};

#endif