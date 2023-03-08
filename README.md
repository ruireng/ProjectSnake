# IS1200 Computer Hardware Engineering - Mini Project: Snake
<!-- This is a repository for a project in the course IS1200 Computer Hardware Engineering spring term 2022. The repository contains all the files necessary for the program "Snake" to be run on a UNO32 ChipKit. The program was developed with MCB32Tools and was written in C.

This project was made by Roy L. and Erik S, with some collaborations with Mostafa A. Z. The credits are written as comments in the file mipslabwork.c. -->

This is a mini project made during the course IS1200 Computer Hardware Engineering, where the classic game Snake was developed for the UNO32 ChipKIT.  

## How to play the game:
 1. You need to have either a Uno32 ChipKIT or a uC32 ChipKIT development board together with a ChipKIT Basic I/O Shield.
 2. Download the [MCB32 toolchain](https://github.com/is1200-example-projects/mcb32tools/releases/).
 3. Download all of the files from the `Snake` folder in this repository.
 4. Connect the ChipKIT to the computer.
 5. Run the MSYS shell and run the command `. opt/mcb32tools/environment` to enter the cross compiler.
 6. Change directory to the project folder.
 7. Run `make` to compile all of the files.
 8. Install the code on the ChipKIT with `make install TTYDEV=/dev/ttySx`, where x is the COM number of the ChipKIT (Windows). *This step might be a bit tricky to pull off, so if you cannot find the COM number, a brute-forcing way could be to try* `make install TTYDEV=/dev/ttyS1`*,* `make install TTYDEV=/dev/ttyS2`*, and so on until the cross compiler recognizes the ChipKIT.*
 9. You should now be able to play Snake on your ChipKIT.
 
## About this project:
This project was made during the summer of 2023.

## This project was made by:
[Roy L.](https://github.com/ruisnake)  
[Erik S.](https://github.com/Fotosynthesis)
