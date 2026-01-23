# EchoClash

## Game Details 

### What is the core gameplay of this game?

The core gameplay will be a stationary center turret defending against waves of enemies. The turret can rotate 360 degrees in a 2D plane, with enemies randomly spawning around the turret. The turret must successfully shoot and destroy the enemies before they reach the center.  

### What are the core features?

- The first and most important is the echolocation feature. While normally enemies are always visible in these types of games, in this game the enemies will not be visible at all times. The player must use echolocation--sending a wave of sound in a direction so that it reflects off an object to locate enemies.  
- The second feature is custom enemies. While most turret shooters do incorporate some form of custom enemy such as as a faster enemy or enemy with more health, this game will incorporate custom enemies that directly interact with the echolocation feature. Mechanics of these enemies are to be determined.  
- The last feature is the addition of voice controls, which will control the echolocation. Based on how loud your voice is when inputting, a corresponding echolocation wave will be sent out, with the size and range being scaled with the noise level of the input.

### Necessary tools

- **Teachable Machine** for voice controls implementation
- **SFML 3** for graphics implementation

---

## Requirements

### Common
- **C++17 or newer**
- **SFML 3.x**
- **Make**
- **g++ or clang++**

---

## macOS

### 1. Install SFML (Homebrew)
```bash
brew install sfml
```

- Find sfml's path:

```bash
brew info sfml
```

#### SFML will be somewhere like:

``` /opt/homebrew/Cellar/sfml/3.0.2 ``` 

- Update SFML_PATH accordingly in the Makefile.

### 2. Build and Run

From the project root:  

``` make run```  

Or separately:  
```bash
make compile
./bin/main
```

## Linux Setup
### 1. Install SFML
#### Ubuntu / Debian
``` sudo apt update ```  
``` sudo apt install libsfml-dev``` 
#### Arch Linux
``` sudo pacman -S sfml ```
#### Fedora
``` sudo dnf install SFML-devel ```

    On Linux, SFML is installed in standard system locations (/usr/include, /usr/lib), so no manual path configuration is required. It will be found automatically.
### 2. Build and Run

From the project root:  

``` make run```  

Or separately:  
```bash
make compile
./bin/main
```

## Windows Setup (WORK IN PROGRESS)
### IMPORTANT: 
There is currently no way to run the game directly from the repo that we have figured out, so this is merely a substitute. You will have to remove all instances of sprites and the use of custom textures in order for it to work.
### THIS SETUP MIGHT NOT WORK FOR EVERYONE, OR EVEN AT ALL
### 1. Install Visual Studio
- Follow this link: https://visualstudio.microsoft.com/downloads/
- Make sure you select "Desktop development with C++" during installation
- after installing, add ```main.cpp``` from this repo to a new C++ project
### 2. Install SFML using vcpkg
#### Installing vcpkg
- Open PowerShell or Command Prompt.
- Clone the repository: ```git clone https://github.com/Microsoft/vcpkg.git.```
- Run the bootstrap script: ```.\vcpkg\bootstrap-vcpkg.bat```
#### Installing SFML
Run:
```bash
.\vcpkg install sfml[graphics,window,audio,network]:x64-windows
```
in Windows Powershell or Command Prompt