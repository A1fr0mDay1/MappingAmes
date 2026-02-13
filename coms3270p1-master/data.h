#ifndef DATA_H
#define DATA_H

/**
 * Validates the structure and content of a map data file.
 *
 * Reads a file line-by-line, checking for formatting errors,
 * out-of-range values, and other rule violations as specified
 * in the project description and errata.
 *
 * @param filename The path to the data file to validate.
 * @return 0 if the file is valid.
 * Otherwise, returns the 1-based line number
 * of the first error encountered.
 */
int validate();

#endif // DATA_H