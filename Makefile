

###### MacOS SFML Makefile ######
# Replace this with the path you get from `brew info sfml`
SFML_PATH = /opt/homebrew/Cellar/sfml/3.0.2

# Replace "src" with the name of the folder where all your cpp code is
cppFileNames := $(shell find ./src -type f -name "*.cpp")

mac-compile:	
	mkdir -p bin
	g++ $(cppFileNames) -I$(SFML_PATH)/include -o main -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
################################

###### Windows SFML Makefile ######

all: compile link # Windows

# Replace this with the path to your SFML "lib" folder
compile:
	g++ -c main.cpp -I"C:\Users\Antoan\OneDrive\Pictures\Documents\libraries\SFML-3.0.0\include"

link:
	g++ main.o -o main.exe \
	-L"C:\Users\Antoan\OneDrive\Pictures\Documents\libraries\SFML-3.0.0\lib" \
	-lsfml-graphics -lsfml-window -lsfml-system

clean:
	del *.o main.exe

run:
	.\main.exe
