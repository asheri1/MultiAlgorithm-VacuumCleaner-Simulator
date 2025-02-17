# MultiThreaded Cleaning Robot Simulations 

## Objective: 
Implement two advanced algorithms to maximize cleaning efficiency.
The program shall run simulations for all the combinations of house⇔algorithm (the “cartesian product” of the two).

## Functionality: 
The robot uses two distinct algorithms to clean the house with the fewest steps possible, aiming to be as efficient as possible in order to increase the chance that one of the implementations would be effective in some cases and the other in other cases. 
The goal is to optimize the cleaning process and minimize energy consumption, increasing the chances of winning an efficiency competition.

# Implementation of the algorithms:
- Algorithm A: 
    The robot uses two progression methods, the first circular and the second greedy. The circular progress is made by progressing "in rings" around the robot's docking station. If the robot cannot progress because it encounters an obstacle, it locates a point with a radius that is the same or one greater than the radius where it detected the obstacle and moves towards it.
    If the robot detects that it is "trapped" in the walls because the radius cannot be increased,, it concludes that it is no longer possible to move forward in a circular motion and switches to a greedy mode of operation so that it always searches for the nearest point that it needs to clean or that it has not yet been to until there is no dirt or new points left.

- Algorithm B: 
    A greedy algorithm that aims to clean as much as possible and therefore will always look for the closest known dirty spot.

## House Representation
The house is represented as a text file with the following characters:
'W': Walls that the robot cannot pass through.
Characters '0' to '9': Represent the level of dirt, with '0' being clean and '9' being very dirty.
'D': The docking station where the robot begins and ends its cleaning.
Exercise Details


## Constraints
Max Steps: The robot is limited by a maximum number of steps, which is defined in the input file.
Battery Life: The robot's battery life is also limited, necessitating careful management of its energy during cleaning.



## Project Structure

The project consists of:
- **Main Application:** Compiles the main simulator executable.
- **Algorithms:** Two separate algorithms (`Algorithm_A` and `Algorithm_B`) implemented as shared objects (`.so` files).

## Prerequisites

Ensure that you have the following tools installed on your system:
- `g++`: GNU Compiler Collection to compile C++ code.
- `make`: A build automation tool to execute the commands in the Makefile.

## Build Instructions

Build the Main Application and Algorithms: 

Use the make command to compile the main application and both algorithms inside the project's main directory:
`make`

This command will:
Compile the `myrobot` executable using the source files specified in SOURCES.
Compile the `Algorithm_A.so` shared object using the source files specified in SOURCES_ALGO_A.
Compile the `Algorithm_B.so` shared object using the source files specified in SOURCES_ALGO_B.

Clean the Build: To remove all compiled files and clean the build directory, use: `make clean`
This command deletes the myrobot executable and both .so files (Algorithm_A.so and Algorithm_B.so).

In order to compile the files seperatly, use:
`make myrobot`
`make Algorithm_A.so`
`make Algorithm_B.so`

## How to run
./myrobot -house_path=<housepath> -algo_path=<algopath> -num_threads=<NUMBER>
```bash
./myrobot -house_path="house files" -algo_path=so_files
```
if not provided num_threads, num_threads=10 by default.

### Optional
Add an additional command-line argument -summary_only with this flag, 
the simulation will generate only the summary.csv file and error files, 
but will not generate the other output files per house⇔algorithm pairs.

Example: 
```bash
./myrobot -house_path="house files" -algo_path=so_files -num_threads=7 -summary_only
```


## Input File Format
The input file should contain the following information:
1. MaxSteps - The Maximum number of steps allowed.
2. MaxBattery - The battery capacity. 
3. Rows - The number of rows in the home representation.
4. Cols - The number of columns in the home representation.
5. House layout:
   each char represents a dirt level (`0-9`), a wall (`W`), or the docking station (`D` - must have one).

## Output File Format
The program generates an output file which has the following data:
1. NumSteps - The number of steps taken.
2. DirtLeft - Remaining Dirt.
3. Status - FINISHED, WORKING or DEAD.
4. All steps performed:
- 'S' for South
- 'N' for north
- 'E' to the east
- 'W' for West
- 's' to stay
- 'F' to finish

### Input File - example (`inputfile.txt`)
```
1-Small house
MaxSteps=2210
MaxBattery = 60
Rows= 9
Cols=19
WWWWWWWW11119999999
W222WWW22222999D999
W33333333333WW999WW
444444444444499999W
WWW88855555WWWW999W
W6666W666666999W99W
W7777777777799WW99W
W4567WW2888899W999W
99999999999999WWWWW
```
### Output File - example (`output_inputfile.txt`)
```
NumSteps = 2163
DirtLeft = 0
Status = FINISHED
Steps:
NsssssssssEsssssssssEsssssssssEsssssssssSsssssssssWsssNWWSssssssssssssssssssssEsssssssssEssssssWSsssssssssSsssssssssEsssssssssSssssNWNNWssssssssssssssssssssSsssssssssSsssssssssSsssssssssEsssssssssEsssssSsssssNNWNNWssssssssssssssssssssWsssssssssNsssssssssWsssssssssSsssssssssWsssssssssNsssSEEEssssssssssssssssssssSWsssssssssSsssssssssWsssssssssWssssWssssNsssNssNsEsssSEEEssssssssssssssssssssNWWWsssWWsSssSsssSssssSsssssSssssssEssssssEssssWWNNNNEEEEEssssssssssssssssssssESSSSsssssssssEssssSsssssssssSsssssssssWsssssssssENNNNWNNWssssssssssssssssssssESSSSSsssssssssSWsssssssssENNNNWWWWWWWssssNsssNssNsSEEEEEEssssssssssssssssssssWWWWWWWssNsSSsssSssssSsssssEsssssSssssssSssssssNNNNNEEEEEEssssssssssssssssssssWWWWWWWWssSsssSssssSsssssSssssssEssssssSssssssNNNNNEEEEEEEssssssssssssssssssssSSWWWWWSSSsssssssEsssssssEsssssssssNsssssEssssWWWNNNNEEEEEssssssssssssssssssssWWWWSWWWWWsssSssssSsssssSssssssSsssssssEsssssNNNNNEEEEEEEEssssssssssssssssssssSSWWWWWSSSSssssssssEssssssssEsssssssssEsssssWNNWWNNNNEEEEEssssssssssssssssssssSSWWWWWSSSWsSssssssssSsssssssssEsssssssssEsssNNNWNNNNEEEEEssssssssssssssssssssWWWWSWWWWWWsssSssssSssssssssWssssssssNssssNsssEEENEEEEEEEEssssssssssssssssssssSSWWWWWSSEEEsssssEsssssssssWSsssssssssSssssSNNNWWWNNNNEEEEEssssssssssssssssssssSSWWWWWSSSWWsSssssssssSsssssssssWsssssssssNsNNNNNNEEEEEEEEssssssssssssssssssssSSWWWWWSSESSSssssssEsssssssssEsssssssssNWWWWWNNNNNNEEEEEEEssssssssssssssssssssSSWWWWWSSSWWWssSsSWsssssssssWsssssssssWsEEENNNNNNNEEEEEEEEssssssssssssssssssssWWWWSWWWWWWWWsssNssWssSsssSssssEssssSssssssNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSSSWWWWWsssssssWsssssssNssssssWssssNNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSWWWWWWWssSssSsssssssSssssssEsssssNNNNNEEENEEEEEEEEssssssssssssssssssssWWWWSWWWWWWWWWWsssNssSSssssWssssEEESSWsssENNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSSSWWWWWWSssSssssssssWsssssssssWENNNNNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSWWWWWWSWWsssSsssssssSsssssSsssNNNENNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSWWWWWWSWWWssssssSsssssssSssssSNNNEENNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSSSSSWWWWWWWWssssssWsssssssssWENNNEENNNEEEENEEEEEEEEssssssssssssssssssssSSWWWWWSSSSSWWWWWWWWWWsssssssssENNNEENNNEEEENEEEEEEEEF

```

## A representation of the house as the algorithm should refer to it during the run (from ex2/ex3)
```
currentLocation: layout[16][2] = D
number of steps so far = 0        
remainedSteps = 2210
totalDirt = 815
battery = 60
W W W W W W W W W W W W W W W W W W W W W 
W W W W W W W W W 1 1 1 1 9 9 9 9 9 9 9 W 
W W 2 2 2 W W W 2 2 2 2 2 9 9 9 D 9 9 9 W 
W W 3 3 3 3 3 3 3 3 3 3 3 W W 9 9 9 W W W 
W 4 4 4 4 4 4 4 4 4 4 4 4 4 9 9 9 9 9 W W 
W W W W 8 8 8 5 5 5 5 5 W W W W 9 9 9 W W 
W W 6 6 6 6 W 6 6 6 6 6 6 9 9 9 W 9 9 W W 
W W 7 7 7 7 7 7 7 7 7 7 7 9 9 W W 9 9 W W 
W W 4 5 6 7 W W 2 8 8 8 8 9 9 W 9 9 9 W W 
W 9 9 9 9 9 9 9 9 9 9 9 9 9 9 W W W W W W 
W W W W W W W W W W W W W W W W W W W W W
```


## Displaying the house after the algorithm has finished running (from ex2/ex3)
```
currentLocation: layout[16][2] = D
number of steps so far = 2163     
remainedSteps = 47
totalDirt = 0
battery = 7
W W W W W W W W W W W W W W W W W W W W W 
W W W W W W W W W                       W
W W       W W W                 D       W
W W                       W W       W W W 
W                                     W W
W W W W                 W W W W       W W
W W         W                   W     W W 
W W                           W W     W W
W W         W W               W       W W 
W                             W W W W W W
W W W W W W W W W W W W W W W W W W W W W
```


