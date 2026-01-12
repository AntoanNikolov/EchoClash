###### MacOS SFML Makefile ######

# Replace this with the path you get from `brew info sfml`
SFML_PATH = /Users/1044845/Documents/SFML-3.0.2

# Replace "src" with the name of the folder where all your cpp code is
cppFileNames := $(shell find ./src -type f -name "*.cpp")

compile:
	mkdir -p bin
	g++ -std=c++17 $(cppFileNames) \
	-I$(SFML_PATH)/include \
	-o bin/main \
	-L$(SFML_PATH)/lib \
	-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

run: mac-compile
	./bin/main

clean:
	rm -f bin/main
