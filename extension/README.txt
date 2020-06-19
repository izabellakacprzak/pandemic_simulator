Welcome to group 28's C Project Extension!

We Hope you enjoy using our program.

This file outlines:
-Installation
-Operation
-Output Legend
-How to use the config file
-Troubleshooting

Installation:
Go into the directory with this file on a terminal and type "make" or "make all".
You can uninstall object files by typing "make clean", but please note that this will also delete any GIFs created by the program from this directory.

Operation:
When you first run the program, you will be asked whether you want default or configured values.
Type either 'default' or 'config' as prompted depending on whether you want your customised variables.
The program will ask if you want terminal or GIF output.
Terminal output doesn't look as nice but you can run thousands of turns quickly.
When in this mode, you are repeatedly prompted to run a number of turns and the simulation runs this number of turns before printing out the board.
Enter 'q' to quit this mode and close the program at any time.
GIF output creates a nice animated gif of the opening turns.
You are asked to input the number of turns included in the GIF, and the program generates the GIF for you.
The generated GIF will be called pandemic.gif and will be in the current directory, it will be overwritten if you run the program again so move it out if you want to keep your GIF.

Output Legend:
Terminal:
'-' Empty Cell
'S' Empty Cell that is a social space
'H' Healthy human
'L' Latent human (no symptoms or chance of death)
'I' Sick human

GIF:
WHITE  Empty Cell
YELLOW Empty Cell that is a social space
GREEN  Healthy human
BLUE   Latent human (no symptoms or chance of death)
RED    Sick human 

How to use the config file:
The config file contains many variables and numbers next to them, you can change the numbers to change the simulations parameters.
Currently there are comments above each variable explaining what they do, you can add your own comments with a line starting with '//'.
If you enter a non-numerical value for or fail to include a variable, a default value will be usid in your simulation instead.

Here are all the variables you can change:
columns
rows
population
initially_infected
latency_period
infection_rate
fatality_rate
recovery_rate
social_spaces
immunity
quarantine

Troubleshooting:
We have not had the time to fully error-proof the main code when using user inputted data. We recommend avoiding the following as they will likely cause the program to crash.
Making the population higher than the number of cells available
Making initially_infected higher than population
Making social_spaces more than the number of available cells
Making the latency period less than 0
Making either of the grid dimensions larger than 1000 in GIF mode (This one may not crash the program but it will run extremely slowly)

One key point to note is that the program only works as intended if you score us 100% for the extension. Users who fail to do this often have a worse experience with the program.
