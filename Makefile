# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shurtado <shurtado@student.42barcelona.fr  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/03 04:55:12 by shurtado          #+#    #+#              #
#    Updated: 2024/08/11 12:51:03 by shurtado         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = gnl_test
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
DFLAGS  = -g3 -fsanitize=address -fsanitize=leak
BUFFER  = 512

SRCS    = get_next_line.c get_next_line_utils.c main.c
OBJS    = $(SRCS:.c=.o)

# Colors
GREEN   = \033[0;32m
RED     = \033[0;31m
YELLOW  = \033[0;33m
BLUE    = \033[0;34m
RESET   = \033[0m

all: CFLAGS += -D BUFFER_SIZE=$(BUFFER)
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Compiled $(NAME) with BUFFER_SIZE=$(BUFFER)$(RESET)"

%.o: %.c get_next_line.h
	$(CC) $(CFLAGS) -D BUFFER_SIZE=$(BUFFER) -c $< -o $@

# Debug build with sanitizers
debug: CFLAGS += $(DFLAGS)
debug: BUFFER = 42
debug: CFLAGS += -D BUFFER_SIZE=$(BUFFER)
debug: fclean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)_debug
	@echo "$(BLUE)Compiled $(NAME)_debug with BUFFER_SIZE=$(BUFFER) + sanitizers$(RESET)"

# Run tests with default BUFFER_SIZE
test: all
	@./$(NAME)

# Run tests with sanitizers
test_debug: debug
	@./$(NAME)_debug

# Run with valgrind for memory leaks
test_valgrind: all
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 2>&1

# Run tests with specific BUFFER_SIZE values
test_bs1:
	$(CC) $(CFLAGS) -D BUFFER_SIZE=1 $(SRCS) -o $(NAME)_bs1
	@./$(NAME)_bs1
	@rm -f $(NAME)_bs1

test_bs4:
	$(CC) $(CFLAGS) -D BUFFER_SIZE=4 $(SRCS) -o $(NAME)_bs4
	@./$(NAME)_bs4
	@rm -f $(NAME)_bs4

test_bs42:
	$(CC) $(CFLAGS) -D BUFFER_SIZE=42 $(SRCS) -o $(NAME)_bs42
	@./$(NAME)_bs42
	@rm -f $(NAME)_bs42

test_bs9999:
	$(CC) $(CFLAGS) -D BUFFER_SIZE=9999 $(SRCS) -o $(NAME)_bs9999
	@./$(NAME)_bs9999
	@rm -f $(NAME)_bs9999

test_bs100000:
	$(CC) $(CFLAGS) -D BUFFER_SIZE=100000 $(SRCS) -o $(NAME)_bs100k
	@./$(NAME)_bs100k
	@rm -f $(NAME)_bs100k

# Run ALL buffer size tests
test_all: test_bs1 test_bs4 test_bs42 test_bs9999 test_bs100000
	@echo "$(GREEN)All buffer size tests completed!$(RESET)"

# Valgrind with specific buffer sizes (most critical for edge cases)
test_valgrind_bs1:
	$(CC) $(CFLAGS) -g -D BUFFER_SIZE=1 $(SRCS) -o $(NAME)_vg1
	@valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)_vg1 2>&1
	@rm -f $(NAME)_vg1

test_valgrind_bs42:
	$(CC) $(CFLAGS) -g -D BUFFER_SIZE=42 $(SRCS) -o $(NAME)_vg42
	@valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)_vg42 2>&1
	@rm -f $(NAME)_vg42

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(NAME)_debug $(NAME)_bs* $(NAME)_vg*
	rm -rf /tmp/gnl_test

re: fclean all

.PHONY: all clean fclean re test test_debug test_valgrind \
        test_bs1 test_bs4 test_bs42 test_bs9999 test_bs100000 \
        test_all test_valgrind_bs1 test_valgrind_bs42 debug
