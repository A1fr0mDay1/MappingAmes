Project: CS 327 Project 1 (Mapper)
Part: A, B, & C
Author: John Hartnett
Email: hartn114@iastate.edu

## 1. Features Implemented

This submission completes Parts A, B, and C of the project.

### Part A: `mapper`
* **`mapper.c`**: Main program that reads map data from standard input.
* **`data.c`**: Contains the `validate()` function.
* **`data.h`**: Header file for the `validate()` function.
* **Functionality**: Validates a map data file according to all project rules and errata.
    * Correctly parses both POI (4-field) and Road (6-field) sections.
    * Uses tab (`\t`) as the delimiter.
    * Validates positive POI/Road counts.
    * Validates all ID numbers are digits-only.
    * Validates POI and Road names are not empty.
    * Validates latitude (-90 to +90) and longitude (-180 to +180).

### Part B: `testgraph`
* **`graph.c`**: Implementation of the Graph ADT (nodes, edges, lists).
* **`graph.h`**: Header file for the Graph ADT.
* **`testgraph.c`**: Main program that reads map data from `stdin` and builds a graph.
* **`testgraph.h`**: Contains the `poi_data_t` struct for storing POI info.
* **Functionality**:
    * Parses the `stdin` data file.
    * Creates a `node_t` for each unique POI and road-point ID.
    * Stores POI name, latitude, and longitude in a `poi_data_t` struct on the node.
    * Creates an `edge_t` for each road, storing the road name and distance (weight).
    * Calls `printGraph()` to display the final graph structure.

### Part C: `citydata`
* **`citydata.c`**: Main program that loads map data from a specified file and performs operations based on command-line arguments.
* **Functionality**: Implements all Part C command-line operations.
    * **Usage Statement**: Prints a usage statement if no arguments are provided.
    * **-f <filename>**: (Required) Loads the graph data from the specified file. This program validates the file using the `validate()` function from Part A.
    * **-location <name>**: Finds the Point of Interest by `<name>` and prints its latitude and longitude.
    * **-diameter**: Finds the two POIs that are farthest apart (straight-line distance) and prints their coordinates and the distance in meters.
    * **-distance <name1> <name2>**: Calculates the straight-line (Haversine) distance in meters between two named POIs.
    * **-roaddist <name1> <name2>**: Calculates the shortest path distance in meters between two named POIs using Dijkstra's algorithm.
    * All operations producing output do so in the order they appear on the command line.

## 2. How to Compile and Execute

### Compile
Several targets are available in the `makefile`:

* **Part A**: `make` or `make mapper`
    ```bash
    make
    ```
    This creates the executable file named `mapper`.

* **Part B**: `make testgraph`
    ```bash
    make testgraph
    ```
    This creates the executable file named `testgraph`.

* **Part C**: `make citydata`
    ```bash
    make citydata
    ```
    This creates the executable file named `citydata`.

* **Clean**: `make clean`
    ```bash
    make clean
    ```
    This removes all object files and executables.

### Execute
Parts A and B read from standard input. Part C reads from command-line arguments.

* **Part A (`mapper`)**
    ```bash
    ./mapper < path/to/data.tsv
    ```
    * **Output (Valid)**: `VALID`
    * **Output (Invalid)**: (The 1-based line number of the first error)

* **Part B (`testgraph`)**
    ```bash
    ./testgraph < path/to/data.tsv
    ```
    * **Output**: A formatted printout of the entire graph structure.

* **Part C (`citydata`)**
    This program uses command-line flags. The `-f` flag is required.
    ```bash
    ./citydata -f path/to/data.tsv [options]
    ```
    * **Example (Usage)**:
        `./citydata`
    * **Example (Location)**:
        `./citydata -f path/to/data.tsv -location "Atanasoff Hall"`
    * **Example (Diameter)**:
        `./citydata -f path/to/data.tsv -diameter`
    * **Example (Distance)**:
        `./citydata -f path/to/data.tsv -distance "Atanasoff Hall" "Kildee Hall"`
    * **Example (Road Distance)**:
        `./citydata -f path/to/data.tsv -roaddist "Ames Highschool" "Coffee Place"`

## 3. Known Errors or Issues

None.
