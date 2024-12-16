NAME := IRC
SRC_DIR := src
BUILD_DIR := .build
DEPS_DIR := .deps

SRC := $(SRC_DIR)/Irc.cpp

OBJS    := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC))
DEPS    := $(patsubst %.o, %.d,$(OBJS))

CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98
CPPFLAGS := -MD -MP

RM := rm -f

all: $(NAME) 

FORCE:

$(BUILD_DIR)/%.o: %.cpp
		mkdir -p $(@D)
		$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
			$(CC) $(OBJS) -o $(NAME)

-include $(DEPS)

# **************************************************************************** #

clean:
	$(RM) -r $(BUILD_DIR) $(DEPS_DIR)

# **************************************************************************** #

fclean: clean
	$(RM) -r $(NAME)

# **************************************************************************** #

re: fclean all

# **************************************************************************** # # .PHONY #

.PHONY: all clean fclean re

# *****************************