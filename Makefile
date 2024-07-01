.SILENT:
NAME		= 	webserv

CC 			= 	c++

CFLAGS		= 	-Wall -Wextra -Werror -std=c++98 -Wshadow -g #-fsanitize=address

RM 			= 	rm -rf

INC_DIR		=	includes/

SRC_DIR		=	sources/

OBJ_DIR		=	temp/

NAMES_DIR	=	namespaces/

CLASS_DIR	=	classes/

SRC			=	$(addprefix $(SRC_DIR),	main.cpp \
					$(addprefix $(CLASS_DIR),	ExceptionMaker.cpp \
												Request.cpp \
												Response.cpp \
												TCPSocket.cpp \
												ServerManager.cpp) \
					$(addprefix $(NAMES_DIR),	Utils.cpp \
												Http.cpp \
												Network.cpp))

OBJ_DIRS		=	$(OBJ_DIR)	$(addprefix $(OBJ_DIR), $(CLASS_DIR)) \
								$(addprefix $(OBJ_DIR), $(NAMES_DIR))

OBJ				=	$(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

GREEN		= 	\033[0;32m

RESET		=	\033[0m

SUS			=	\U00000D9E

HAMMER		=	\U0001F528

BROOM		=	\U0001F9F9

$(NAME):		$(OBJ)
				printf '$(HAMMER)\n\t$(GREEN)Compiling $(NAME)$(RESET)\n'
				$(CC) $(CFLAGS) $^ -o $@ -I $(INC_DIR)
				make compiled

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIRS)
				printf '$(HAMMER)\n\t'
				printf "$(GREEN)Compiling $(NAME) object $(RED)%-33.33s\r" $(notdir $@)
				$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

$(OBJ_DIRS):
				mkdir -p $@

all: 			$(NAME)

clean:
				$(RM) $(OBJ_DIR)

fclean:			clean
				printf '$(BROOM)\n$(BROOM)\t$(GREEN)Cleaning project$(RESET)\n'
				$(RM) $(NAME)
				printf '$(BROOM)\t\t\t$(SUS)\n'

re:				fclean	all

compiled:
				printf "															 	\n"
				printf "$(GREEN)	$(NAME)							 			$(RESET)\n"
				printf "$(GREEN)                             _  _             _ $(RESET)\n"
				printf "$(GREEN)                            (_)| |           | |$(RESET)\n"
				printf "$(GREEN)  ____   ___   ____   ____   _ | |  ____   _ | |$(RESET)\n"
				printf "$(GREEN) / ___) / _ \ |    \ |  _ \ | || | / _  ) / || |$(RESET)\n"
				printf "$(GREEN)( (___ | |_| || | | || | | || || |( (/ / ( (_| |$(RESET)\n"
				printf "$(GREEN) \____) \___/ |_|_|_|| ||_/ |_||_| \____) \____|$(RESET)\n"
				printf "$(GREEN)                     |_|                        $(RESET)\n"
				printf "																\n"

.PHONY: 		all fclean re
