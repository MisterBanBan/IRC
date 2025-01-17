# Variables de base
NAME := IRC
SRC_DIR := src
BUILD_DIR := .build
DEPS_DIR := .deps
INC_DIR := inc

#yes
# Liste des fichiers sources
SRC := $(SRC_DIR)/Irc.cpp \
       $(SRC_DIR)/Server.cpp \
	   $(SRC_DIR)/Channel.cpp 

# Génération des fichiers objets et des dépendances
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
DEPS := $(patsubst $(BUILD_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

# Compilateur et flags
CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98
CPPFLAGS := -MD -MP -I $(INC_DIR)

# Commande de suppression
RM := rm -f

# Cible par défaut
all: $(NAME)

# Règle de compilation des fichiers objets
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Règle de linkage
$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

# Inclusion des fichiers de dépendances
-include $(DEPS)

# Cible de nettoyage
clean:
	$(RM) -r $(BUILD_DIR) $(DEPS_DIR)

# Cible de nettoyage complet
fclean: clean
	$(RM) -r $(NAME)

# Cible de re-compilation
re: fclean all

# Déclaration des cibles phony
.PHONY: all clean fclean re
