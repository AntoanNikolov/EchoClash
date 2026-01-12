# Project Details

## What is this project coded in?

This project is coded in c++ using the SFML 3 graphics library.  

## What is the core gameplay of this game?

The core gameplay will be a stationary center turret defending against waves of enemies. The turret can rotate 360 degrees in a 2D plane, with enemies randomly spawning around the turret. The turret must successfully shoot and destroy the enemies before they reach the center.  

## What custom features are added?

- The first and most important is the echolocation feature. While normally enemies are always visible in these types of games, in this game the enemies will not be visible at all times. The player must use echolocation--sending a wave of sound in a direction so that it reflects of an object--to locate enemies.  
- The second feature is custom enemies. While most turret shooters do incorporate some form of custom enemy such as as a faster enemy or enemy with more health, this game will incorporate custom enemies that directly interact with the echolocation feature. Mechanics of these enemies are to be determined.  
- The last feature is the addition of voice controls, which will control the echolocation. Based on how loud your voice is when inputting, a corresponding echolocation wave will be send out, with the size and range being scaled with the noise level of the input.  

## How will the voice controls be implemented?

This is a tentative proposal at the moment.  

The voice controls will be implemented using Teachable Machine to recognize the noise level of an input. The machine will be trained on data from different decibel points, and categorized into different ranges, each one with it's own assigned noise level.  

---  

# SFML 3 Project

This is a SFML 3 project that builds and runs on **macOS** and **Windows** using **Makefiles**.

---

## Requirements

### Common
- **C++17 or newer**
- **SFML 3.x**
- **Make**
- **g++ or clang++**

---

## macOS Setup

### 1. Install SFML (Homebrew)
```bash
brew install sfml
```

- Verify the version:

```bash
brew info sfml
```

#### This project assumes SFML is located at:

``` /opt/homebrew/Cellar/sfml/3.0.2 ``` 

- If your path is different, update SFML_PATH in the Makefile.

#### Make sure to add the path to the "include" folder to your project/ide settings
### 2. Build and Run

From the project root:  

``` make mac-run```  

Or separately:  
```bash
make mac-compile
./bin/main
```
## Windows Setup
### Work in Progress