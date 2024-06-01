.SILENT:
NAME		= 	webserv

CC 			= 	c++

CFLAGS		= 	-Wall -Wextra -Werror -std=c++98 -Wshadow -g

RM 			= 	rm -rf

INC_DIR		=	includes/

SRC_DIR		=	sources/

OBJ_DIR		=	temp/

SRC			=	$(addprefix $(SRC_DIR),	main.cpp)

OBJ 		= 	$(addprefix $(OBJ_DIR), $(notdir $(SRC:.cpp=.o)))

GREEN		= 	\033[0;32m

RESET		=	\033[0m

SUS			=	\U00000D9E

HAMMER		=	\U0001F528

BROOM		=	\U0001F9F9

$(NAME):		$(OBJ)
				printf '$(HAMMER)\n\t$(GREEN)Compiling	$(NAME)$(RESET)\n'
				$(CC) $(CFLAGS) $^ -o $@ -I $(INC_DIR)
				make done

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp | $(OBJ_DIR)
				$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

$(OBJ_DIR):
				mkdir $(OBJ_DIR)

all: 			$(NAME)

clean:
				$(RM) $(OBJ_DIR)

fclean:			clean
				printf '$(BROOM)\n$(BROOM)\t$(GREEN)Cleaning project$(RESET)\n'
				$(RM) $(NAME)
				printf '$(BROOM)\t\t\t$(SUS)\n'

re:				fclean	all

done:
				clear
				make compiled
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
