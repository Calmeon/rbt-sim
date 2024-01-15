# Roundabout simulator
Implementation of a multi-agent system to study traffic efficiency on roundabouts

## Repository content

- src: Directory containing the full source code of the application.
    - main.cpp: Main program where research studies can be created.
    - roundabout.(h/cpp): Main class of the program containing the logic of the system.
    - car.(h/cpp): Class representing a car.
    - agent.(h/cpp): Class representing an agent.
    - settings.(h/cpp): Files where configuration parameters are set.
    - utils.(h/cpp): Utility functions.
    - Makefile: File automating the software building process.
    - test.sh: Bash script to run the program multiple times to check stability.
    - diagrams: Directory with Python files for visualization.
        - fundamental_diagram.py: Program used to create fundamental diagrams, using single or multiple specification data.
        - space_time_diagram.py: Program for creating space-time diagrams.
        - latex_table.py: Program preparing a density report in LaTeX table format.
