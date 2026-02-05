CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC_DIR = src
BIN_DIR = bin
TARGET = $(BIN_DIR)/main

# Only compile game_main.cpp for the main executable
GAME_FILE = $(SRC_DIR)/game_main.cpp

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
    # Linux - try Homebrew first, fall back to system
    HOMEBREW_SFML = $(shell brew --prefix sfml 2>/dev/null)
    ifneq ($(HOMEBREW_SFML),)
        # Linux Homebrew
        INCLUDES = -I$(HOMEBREW_SFML)/include
        LIBS = -L$(HOMEBREW_SFML)/lib \
               -Wl,-rpath,$(HOMEBREW_SFML)/lib \
               -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
    else
        # Linux system packages (apt / dnf / pacman)
        INCLUDES =
        LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
    endif
endif

# ---------- RULES ----------
compile:
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(GAME_FILE) $(INCLUDES) -o $(TARGET) $(LIBS)

run: compile
	./$(TARGET)

clean:
	rm -f $(TARGET) $(BIN_DIR)/test_suite

test:
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -DTESTING $(SRC_DIR)/game_test.cpp $(INCLUDES) -o $(BIN_DIR)/test_suite $(LIBS)
	./$(BIN_DIR)/test_suite

.PHONY: compile run clean test