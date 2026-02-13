#include <stdio.h> 
#include "data.h"

/**
 * Main entry point for the mapper program.
 *
 * This program validates a map data file read from Standard Input (stdin).
 *
 * It will print "VALID" if the file is correctly formatted.
 * Otherwise, it will print the 1-based line number of the first error encountered to stdout.
 *
 * It prints a usage message if any command-line arguments are provided.
 */
int main(int argc, char *argv[]) {
    
    if (argc != 1) {
        fprintf(stderr, "Usage: %s (reads from stdin)\n", argv[0]);
        return 1;
    }

    int error_line = validate(); 

    // Print the result based on the return value from validate()
    if (error_line == 0) {
        printf("VALID\n");
    } else {
        printf("%d\n", error_line);
    }

    return 0;
}