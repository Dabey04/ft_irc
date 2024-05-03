NAME =				ircserv

BUILD_DIR =			build

CXX =				c++

CXXFLAGS =			-std=c++98 -Wall -Wextra -Werror -g3

SRCS	=			src/main.cpp \
					src/Server.cpp \
					src/Client.cpp

OBJS	=			$(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.cpp=.o)))

all:				$(NAME)

$(NAME):			$(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o:	src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

fclean:				clean
	@rm -f $(NAME)

re:					fclean all

.PHONY:				all clean fclean re