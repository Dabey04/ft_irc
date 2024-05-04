#include "../includes/Server.hpp"

bool Server::Signal = false;

Server::Server()
{
	_socket_fd = -1;
	// std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

Server::~Server()
{
	_state = false;
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
}

Server::Server(char *port, char *pass)
{
	securArg(port, pass);//check error port and pass
}

void	Server::clearClient(int fd)
{
	//supprimer le client du vecteur des fds
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break ;
		}
	}
	//supprimer le client du vecteur des clients
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break ;
		}
	}
}

void		Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal received !" << std::endl;
	Server::Signal = true;
}
void			Server::CloseFds()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Client <" << _clients[i].getFd() << "> Disconnected" << std::endl;
		close(_clients[i].getFd());
	}
	if (_socket_fd != -1)
	{
		std::cout << "Server <" << _socket_fd << "> Disconnected" << std::endl;
		close(_socket_fd);
	}
}

void	Server::initServer(const char *port, const char *pass)
{
	(void)port;
	(void)pass;

	// this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// if (this->_socket_fd == -1)
	// 	throw std::runtime_error("(SERVER) echec d'initialisation du socket");

	// this->_socket_add.sin_family = AF_INET;
	// this->_socket_add.sin_port = htons(this->_port);
	// this->_socket_add.sin_addr.s_addr = INADDR_ANY;

	// // configuration et liaisons
	// int	socketAdresslenght = sizeof(this->_socket_add);
	// int	bind_adress = bind(this->_socket_fd, (struct sockaddr*) &this->_socket_add, socketAdresslenght);

	// if (bind_adress == -1)
	// 	throw std::runtime_error("(SERVER) echec de liaison au socket");
	// if (listen(this->_socket_fd, PENDING_QUEUE_MAXLENGTH) == -1)
	// 	throw std::runtime_error("(SERVER) echec de l'ecoute des connexion entrante\n");
	// std::cout << VERT << "ircserv on" << REINIT << std::endl;
}

void			Server::creatSocket()
{
	// struct pollfd newPoll;


	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_socket_fd == -1)
		throw std::runtime_error("(SERVER) echec d'initialisation du socket");

	this->_socket_add.sin_family = AF_INET;
	this->_socket_add.sin_port = htons(this->_port);
	this->_socket_add.sin_addr.s_addr = INADDR_ANY;

	// configuration et liaisons
	int	socketAdresslenght = sizeof(this->_socket_add);
	int	bind_adress = bind(this->_socket_fd, (struct sockaddr*) &this->_socket_add, socketAdresslenght);

	if (bind_adress == -1)
		throw std::runtime_error("(SERVER) echec de liaison au socket");
	if (listen(this->_socket_fd, PENDING_QUEUE_MAXLENGTH) == -1)
		throw std::runtime_error("(SERVER) echec de l'ecoute des connexion entrante\n");
	std::cout << VERT << "ircserv on" << REINIT << std::endl;
}

void Server::securArg(const char *port, const char *pass)
{
	int			i = 0;
	std::string	sPort = port;
	std::string	sPass = pass;

	if (sPort.empty() || sPass.empty())
		throw std::invalid_argument("The arguments are empty");
	while (port[i])
	{
		if (!isdigit(port[i]))
			throw std::invalid_argument("The connection port needs just digit");
		i++;
	}

	long int	tmp = atol(sPort.c_str());
	if (tmp > INT_MAX)
		throw std::overflow_error("Alert: Int overflow");
	else if (tmp < 0)
			throw std::invalid_argument("The connection port can't take a negative value");
	else if (tmp == 0)
		throw std::invalid_argument("The connection port can't take zero for value");
	this->_port = tmp;
	this->_pass = sPass;
}


void Server::Run()
{
	// initServer();
	socklen_t	addrlen = sizeof(this->_socket_add);
	int			connectedSockFd = accept(this->_socket_fd, (struct sockaddr*) &this->_socket_add, (socklen_t *) &addrlen);
	
	if (connectedSockFd == -1)
		throw std::runtime_error("(SERVER) Echec d'etablissement de la conexion");
	std::cout << VERT << "SERVEUR CONNECTED AU CLIENT IRSSI" << REINIT << std::endl;
	// while (_state)
	// {
		//reception d'un message
		char	buffer[BUFFER_SIZE] = {0};
		int	receivedBytes = recv(connectedSockFd, buffer, BUFFER_SIZE, 0);
		
		if (receivedBytes == -1)
		{
			std::cout << "(SERVEUR) echec de reception du message du client" << std::endl;
			exit(1);	
		}
		std::cout << "Client : " <<  buffer << std::endl;

		//envoie de message pour le test
		const char message []  = "Bonjour client, je suis le serveur.";
		int	sentBytes = send(connectedSockFd, (message), strlen(message) , 0);

		if (sentBytes == -1)
		{
			std::cout << "(SERVEUR) echec d'envoie du message" << std::endl;
			exit(1);	
		}
		//fermeture des sockets et liberations 
		close(connectedSockFd);
		close(this->_socket_fd);
	// }
}


