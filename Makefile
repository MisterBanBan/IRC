# Variables de base
NAME := IRC
SRC_DIR := src
CMDS_DIR := $(SRC_DIR)/commands
BUILD_DIR := .build
DEPS_DIR := .deps
INC_DIR := inc

# Liste des fichiers sources
SRC := $(SRC_DIR)/Irc.cpp \
       $(SRC_DIR)/Server.cpp \
       $(SRC_DIR)/Client.cpp \
	   $(SRC_DIR)/Channel.cpp \
	   $(CMDS_DIR)/invite.cpp \
	   $(CMDS_DIR)/join.cpp \
	   $(CMDS_DIR)/kick.cpp \
	   $(CMDS_DIR)/mode.cpp \
	   $(CMDS_DIR)/nick.cpp \
	   $(CMDS_DIR)/part.cpp \
	   $(CMDS_DIR)/pass.cpp \
	   $(CMDS_DIR)/privmsg.cpp \
	   $(CMDS_DIR)/topic.cpp \
	   $(CMDS_DIR)/user.cpp \
	   $(CMDS_DIR)/cap.cpp \
	   $(CMDS_DIR)/who.cpp \
	   $(CMDS_DIR)/quit.cpp \
	   $(CMDS_DIR)/bot.cpp

# Génération des fichiers objets et des dépendances
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
DEPS := $(patsubst $(BUILD_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

# Compilateur et flags
CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98 -g
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
