# Go Game (C++ / SFML)

A lightweight, feature-rich implementation of the classic board game **Go (Weiqi)**, built using **C++** and **SFML**.  
This project features a full 19x19 board, save/load functionality, and a customizable user interface.

<!-- Note: Replace the path below with a real screenshot path if you upload one -->
<!-- ![Screenshot](path/to/screenshot.png) -->

---

## Features

### Core Gameplay
- **19x19 Board** – Full standard board size with star points.
- **2-Player Local Mode** – Play Black vs. White on the same device.

### Interactions
- **Ghost Stones** – Preview moves before placing them.
- **Rule Enforcement**:
    - Detects occupied spots
    - Suicide rule prevention
    - Ko rule detection

### Game State Management
- **Undo / Redo** – Revert accidental moves.
- **Save / Load System** – Save your progress to a file and resume later.

### Customization
- Multiple **board themes** (Wood, Ocean, Galaxy)
- Different **stone styles** (Glass, Flat, Star)
- Adjustable **volume** for background music & sound effects

---

## System Requirements

- **OS:** Windows 10 / 11 (64-bit)

### Dependencies (for building)
- C++17 compiler (MinGW-w64 recommended)
- CMake 3.20+
- SFML 2.6.1

---

## How to Run

### **Method 1: Pre-built Executable (Easiest)**
1. Download the latest release.
2. Unzip the folder.
3. Run `GoGame_CS160.exe` inside the **cmake-build-debug** (or **bin**) folder.

**Important:** Do *not* move the `.exe` outside its folder.  
> It needs the `assets/` directory to run.

---

### **Method 2: Build from Source**

1. Install **SFML 2.6.1** and set it up (e.g., `C:/SFML`).
2. Clone this repository:
   ```bash
   git clone https://github.com/lemon4life/GoGame_CS160.git
   cd GoGame_CS160
3. Build using CMake

```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH="C:/SFML" ..
cmake --build .
````

Copy the `assets/` folder and SFML `.dll` files into your build directory.

Run the executable.

---

## Controls

* **Left Click:** Place stone / Interact with UI
* **Esc / Exit Button:** Quit game or return to the main menu

---

## Authors

* **Hoàng Nguyên Anh**
* **Nguyễn Phan Minh Nhật**

---

## License

This project is for educational purposes (**CS160**).
