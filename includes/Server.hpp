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
		Server(char *port, char *pass);
		Server(const Server &other);
		~Server();

		Server &operator=(const Server &other);

		void	Run();

		void			initServer(const char *port, const char *pass);
		void			creatSocket();
		void			newClient();
		void			mewDataClient(int fd);
		static void		signalHandler(int signum);

		void			CloseFds();
		void			clearClient(int fd);

		void			securArg(const char *port, const char *pass);


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