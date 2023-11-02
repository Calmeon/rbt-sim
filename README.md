# Roundabout simulator
Implementation of a multi-agent system to study traffic efficiency on roundabouts

## Repository content

src -> Directory containing full source of an apllication

## What has been done

- Project setup
- Base model
    - Car consisting of the head and tail
    - Transition function in circular motion
        - accelerate
        - brake
        - move (with tails handling)
    - Helper functions for future use
- Entries
    - Generating cars (weighted random)
    - Transition functions
    - Enter functionality (with tails remaining on entries)
- Exits
    - Transition functions
    - Tails remaining on roundabout lane
    - Cars disappearing at the end
- Deterministic model (can set up fixed seed)
- Plots (Spacetime diagram)
    - Output text files from simulation 
    - Folder structure
    - Running python script for plots from main program
    - Reading output.txt in python script
- Multi lanes
    - Calculating indexes on different lanes
    - Choosing lane algorithm
    - Change lane function
    - Enter on different lanes
- Fundamental diagram
    - Cars generation based on density
    - Function to generate data
    - Ploting data
