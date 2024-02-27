# B+ Tree Project

## Setup to run

### Windows:

1. make sure cmake is installed (>=3.26). Check by running
   ```
   cmake --version
   ```
   Otherwise can install from https://cmake.org/download/
2. Make sure MINGW is installed for windows
3. Go to project directory and create a new directory build
   ```
   mkdir build
   cd build
   ```
4. Run these two command to compile and build the exe file
   ```
   cmake -S .. -G "MinGW Makefiles"
   cmake --build .
   ```
5. An exe file should be created, run this exe in console by running:
   ```
   .\B_Plus_Tree_Project.exe or B_Plus_Tree_Project.exe
   ```
   This exe file will run the main.cpp file for internal testing of the B+ Tree functions. To get the experiments results, please follow the steps below.
6. To automate the building and running of the code after changes, can create a batch file with the following commands and just run it everytime
   ```
   cd build
   cmake -S .. -G "MinGW Makefiles"
   cmake --build .
   .\B_Plus_Tree_Project.exe or B_Plus_Tree_Project.exe
   ```

### Mac

1. Install CMake through
   ```bash
   sudo apt-get update
   sudo apt-get install cmake
   ```
   Alternatively you can use brew,
   ```bash
   brew install cmake
   ```
2. Make sure GCC is installed on Mac
3. In the project directory, create dir named 'build' and navigate into it. This is to keep the source directory clean.
   ```bash
   mkdir build && cd build
   ```
4. Run this command to generate build files and build the project
   ```
   cmake .. && make
   ```
5. Run the executable
   ```
   ./B_Plus_Tree_Project
   ```
6. To automate the building and running of the code after changes, can create a .sh file with the following commands and just run it everytime
   ```
   cd build
   cmake ..
   make
   ./B_Plus_Tree_Project
   ```
