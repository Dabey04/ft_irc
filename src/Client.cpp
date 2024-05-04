#include "../includes/Client.hpp"

Client::Client()
{
	// std::cout << VERT << "Default constructor are called" << REINIT << std::endl;
}

Client::~Client()
{
}

// Client &Client::operator=(const Client &other)
// {
// }

int	Client::getFd()
{
	return (this->_fd);
}

void	Client::setFd(int fd)
{
	this->_fd = fd;
}

void	Client::setIpAdd(std::string ipadd)
{
	this->_ip_add = ipadd;
}

