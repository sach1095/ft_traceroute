# Compilation
NAME		=	ft_ping
CC			=	clang
FLAGS		=	-Wall -Werror -Wextra -g3 -fsanitize=address
HEADER		=	-I$(PATH_INC)

# Color Code and template code
GREY = \033[2:49;39m
RED = \033[1;31m
GREEN = \033[3;32m
YELLOW = \033[3;49;33m
BLUE = \033[3;4;34m
BLUEE = \033[3;34m
PURPLE = \033[3;35m
CYAN = \033[3;36m
RESET = \033[0m
ICONOK = \033[1;32m
ICONKO = \033[1;31m
CO_DELET = \033[3;31m
CO_A_OUT = \033[96m
WHITE = \033[1;37m


# Path directories
PATH_INC	=	include
PATH_SRC	=	src
PATH_OBJ	=	obj

# Source
SRC			=	$(addprefix $(PATH_SRC)/, main.c init.c processHost.c sendPing.c printMethod.c utils.c)

OBJ			=	$(addprefix $(PATH_OBJ)/, $(SRC:.c=.o))

# Headers
INC_FILES	= lib.h

INC			=	$(addprefix $(PATH_INC)/, $(INC_FILES))

# Rules
all			:	$(NAME)

$(NAME)	:	$(OBJ) $(INC)
	@ $(CC) $(FLAGS) $(HEADER) $(OBJ) -o $(NAME)
	@ printf "\033[2K\r$(BLUE)$(NAME)$(RESET)$(BLUEE): $(ICONOK)Compiled [√]$(RESET)\n"

$(PATH_OBJ)/%.o		: 	%.c    $(INC)
	@ mkdir -p $(dir $@)
	@ printf "\033[2K\r$(PURPLE)$<: $(CYAN)loading..$(RESET)"
	@ $(CC) $(FLAGS) $(HEADER) -c  -o $@ $<

clean			:
				@ rm -rf $(OBJ)

fclean	:	clean
	@ rm -rf $(NAME)
	@ printf "$(CYAN)'$(NAME)'$(RESET) and all .o has been $(RED)deleted. 🗑️\n$(RESET)"

re		:	fclean all

run		:
	@ make && ./$(NAME)

.PHONY: all compil clean fclean re run
