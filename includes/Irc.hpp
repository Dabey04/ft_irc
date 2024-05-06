#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <csignal>
#include <cstring>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <exception>
#include <arpa/inet.h>
#include <limits.h>
#include <unistd.h>
#include <cstdlib> //pour atoi()
#include <poll.h>
  #include <fcntl.h>//pour fnctl()

#include "Server.hpp"
#include "Client.hpp"

/* +++ COLORS +++ */
#define REINIT "\033[0m"
#define NOIR "\033[30m"
#define ROUGE "\033[31m"
#define VERT "\033[32m"
#define JAUNE "\033[33m"
#define BLEU "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BLANC "\033[37m"

#define PENDING_QUEUE_MAXLENGTH 1 // a changer => nombre de client qui peuvent se connecte
#define BUFFER_SIZE 1024

/* +++ Global +++ */
extern bool _state;

#endif