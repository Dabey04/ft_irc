#include "../includes/Server.hpp"

bool Server::Signal = false;

Server::Server()
{
	_socket_fd = -1;
	// std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

// Server::Server(const Server &other)
// {

// }

Server::~Server()
{
	// _state = false;
	std::cout << ROUGE << "ircserv off" << REINIT << std::endl;
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

void			Server::closeFds()
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

void			Server::creatSocket()
{
	struct pollfd newPoll;

	this->_socket_add.sin_family = AF_INET;//definir la famille d'adresse ipv4
	this->_socket_add.sin_port = htons(this->_port);//convertit un u_short(port) de l’hôte en ordre d’octet réseau TCP/IP
	this->_socket_add.sin_addr.s_addr = INADDR_ANY;
	// this->_socket_add.sin_addr.s_addr = inet_addr("127.0.0.1");

	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);//creation du socket server

	if (this->_socket_fd == -1)
		throw std::runtime_error("(SERVER) failed to create socket");

	int en = 1;// la valeur de SO_REUSEADDR de setsocket()
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw std::runtime_error("(SERVER) failed to set option on socket");
	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)// definir l'option NonBlaquant pour les sockets
		throw std::runtime_error("(SERVER) failed to set option (O_NONBLOCK) on socket");
	
	// configuration et liaisons
	int	socketAdresslenght = sizeof(this->_socket_add);
	int	bind_adress = bind(this->_socket_fd, (struct sockaddr*) &this->_socket_add, socketAdresslenght);

	if (bind_adress == -1)
		throw std::runtime_error("(SERVER) failed to bind socket");
	if (listen(this->_socket_fd, PENDING_QUEUE_MAXLENGTH) == -1)
		throw std::runtime_error("(SERVER) failed to listen");

	newPoll.fd = _socket_fd;// ajouter le socket du server au pollfd
	newPoll.events = POLLIN;//evenements attendus (entree)
	newPoll.revents = 0;//evenements detectes (sortie)

	_fds.push_back(newPoll);// ajouter le socket du server au pollfd 
}

void	Server::newClient()
{
	Client				client;
	struct pollfd		newPoll;
	struct sockaddr_in	cliaddr;
	socklen_t			sock_len = sizeof(cliaddr);

	int	connectSockFD = accept(_socket_fd, (struct sockaddr*) &cliaddr,
	(socklen_t *) &sock_len);

	if (connectSockFD == -1)
	{
		std::cout << "(SERVEUR) accept() failed" << std::endl;
		return ;
	}
	if (fcntl(connectSockFD, F_SETFL, O_NONBLOCK) == -1)// definir l'option NonBlaquant pour le socket
	{
		std::cout << ("(SERVER) fcntl()failed") << std::endl;
		return ;
	}
	newPoll.fd = connectSockFD;// ajouter le socket du client au pollfd
	newPoll.events = POLLIN;//evenements attendus (entree / lecture)
	newPoll.revents = 0;//evenements detectes (sortie)

	client.setFd(connectSockFD);//set le fd au client
	client.setIpAdd(inet_ntoa(cliaddr.sin_addr));// convertire l'addr ip en string
	_clients.push_back(client);// ajouter le client au vecteur des clients 
	_fds.push_back(newPoll);// ajouter le socket du client au pollfd

	std::cout << VERT << "Client Connected" << REINIT << std::endl;
}

void	Server::mewDataClient(int fd)
{
	char buffer[BUFFER_SIZE];//buffer pour recevoir la data

	memset(buffer, 0, BUFFER_SIZE);
	int	recevBytes = recv(fd, buffer, (BUFFER_SIZE - 1), 0);//recevoir la data
		
	if (recevBytes <= 0)// si le client est deconnecte ou y a une erreur
	{
		std::cout << "(SERVEUR) Failure to receive message from the client" << std::endl;
		clearClient(fd);//supprimer le client
		close(fd);//close le fd du client
	}
	else
	{
		buffer[recevBytes] = '\0';
		std::cout << "Client <" << fd << "> Data: " << buffer << std::endl;
		//ajouter le traitement de la data : Parsing, Commande, execution...
	}
}

void	Server::initServer(char *port, char *pass)
{
	securArg(port, pass);//check error port and pass
	creatSocket();//creation du socket server

	std::cout << VERT << "Server <" << _socket_fd << "> connected" << REINIT << std::endl;
	std::cout << "Waiting to accept a connection..." << std::endl;
	
	while (Server::Signal == false)// le serveur est en marche si pas de signal
	{
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && Server::Signal == false)// surveille si y a un fd qui est pret a etre lu
			throw std::runtime_error("(SERVER) poll() failed");

		for (size_t i = 0; i < _fds.size(); i++)//verifier tout les files descripteurs
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd ==  _socket_fd)
					newClient();//accepter un client
				else
					mewDataClient(_fds[i].fd);//recevoir une data du client
			}
		}	
	}
	closeFds();//close tout les fds a l'arret du server
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
