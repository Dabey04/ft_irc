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

}

void	Server::mewDataClient(int fd)
{
	(void)fd;
}

void	Server::initServer(const char *port, const char *pass)
{
	this->_port = atol(port);
	(void)pass;// comment configurer le mot de passe ??

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


