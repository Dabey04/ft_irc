#include "../includes/Irc.hpp"

bool _state = true;

void handle_sigInt(int sig)
{
	(void)sig;
	_state = false;
}

int main(int ac, char **av)
{
	signal(SIGINT, handle_sigInt);
	try
	{
		if (ac == 3)
		{
			Server Serv;
			Serv.initServer(av[1], av[2]);
		}
		else
			throw std::invalid_argument("The ircserv needs port and password for get ready");

	}
	catch (const std::exception& e)
	{
		std::cerr << ROUGE << "Error: " << e.what() << REINIT << std::endl;
		//close les fds
	}
}