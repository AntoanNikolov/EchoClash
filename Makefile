###### MacOS SFML Makefile ######

# Replace this with the path you get from `brew info sfml`
SFML_PATH = /Users/1044845/Documents/SFML-3.0.2

# Replace "src" with the name of the folder where all your cpp code is
cppFileNames := $(shell find ./src -type f -name "*.cpp")
mac-compile:
	mkdir -p bin
	g++ -std=c++17 $(cppFileNames) \
	-I$(SFML_PATH)/include \
	-o bin/main \
	-L$(SFML_PATH)/lib \
	-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

mac-run: mac-compile
	./bin/main

mac-clean:
	rm -f main



###### Windows SFML Makefile ######

# Adjust this if your SFML path is different
SFML_PATH = C:\Users\Antoan\OneDrive\Pictures\Documents\libraries\SFML-3.0.0

SRC = src\main.cpp
OUT = bin\main.exe

all: build

build:
	if not exist bin mkdir bin
	g++ -std=c++17 $(SRC) -o $(OUT) ^
	-I"$(SFML_PATH)\include" ^
	-L"$(SFML_PATH)\lib" ^
	-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

run:
	$(OUT)

clean:
	del /Q bin\*.exe 2>nul
