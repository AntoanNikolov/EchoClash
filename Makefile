CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC_DIR = src
BIN_DIR = bin
TARGET = $(BIN_DIR)/main

CPP_FILES := $(shell find $(SRC_DIR) -type f -name "*.cpp")

UNAME_S := $(shell uname -s)

# ---------- OS-SPECIFIC SETTINGS ----------
ifeq ($(UNAME_S),Darwin)
	# macOS (Homebrew)
	SFML_PATH = /opt/homebrew/Cellar/sfml/3.0.2
	INCLUDES = -I$(SFML_PATH)/include
	LIBS = -L$(SFML_PATH)/lib \
	       -Wl,-rpath,$(SFML_PATH)/lib \
	       -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
else
	# Linux (apt / dnf / pacman)
	INCLUDES =
	LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
endif

# ---------- RULES ----------
compile:
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CPP_FILES) $(INCLUDES) -o $(TARGET) $(LIBS)

run: compile
	./$(TARGET)

clean:
	rm -f $(TARGET)
