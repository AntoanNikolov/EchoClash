# EchoClash

## Game Details 

### Core Gameplay

A stationary center turret defending against waves of enemies. The turret can rotate 360 degrees in a 2D plane, with enemies randomly spawning around the turret. The turret must successfully shoot and destroy the enemies before they reach the center.  

### Core Mechanics

- Echolocation: In this game, enemies will not be visible at all times. The player must use echolocation--sending a wave of sound in a direction so that it reflects off an object to locate enemies.  
- Waves: Enemies will spawn in waves, with their difficulty being based upon the player's loundess (frequency of echolocation)

### Necessary tools

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

```bash
make run
```  

Or separately:  
```bash
make compile
./bin/main
```

## Linux Setup
### 1. Install SFML
You can install SFML using either your system package manager or Homebrew. The Makefile will automatically detect which one you're using.
### Option A: System Package Manager (Not Recommended)
#### Ubuntu / Debian
```bash
sudo apt update
```
```bash
sudo apt install libsfml-dev
```
#### Arch Linux
```bash
sudo pacman -S sfml
```
#### Fedora
```bash
sudo dnf install SFML-devel
```
- On Linux, SFML installed via system package managers is placed in standard system locations (/usr/include, /usr/lib), so no manual path configuration is required.
### Option B: Homebrew (Recommended!)
If you prefer using Homebrew on Linux:
```bash
brew install sfml
```
The Makefile will automatically detect the Homebrew installation and configure the appropriate paths and rpath settings.
2. Build and Run
From the project root:
```bash
make run
```
Or separately:
```bash
make compile
./bin/main
```
The build system will automatically detect whether you're using system packages or Homebrew and configure itself accordingly.

## Windows Setup  
Good luck lol.
