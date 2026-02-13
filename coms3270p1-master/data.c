#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024 // Maximum length for a line

/**
 * Helper function to check if a string contains only digits.
 * (Rule c)
 * @param str The string to check.
 * @return 1 if all digits, 0 otherwise.
 */
static int is_all_digits(const char *str) {
    if (str[0] == '\0') {
        return 0; // Empty string is not a valid ID
    }
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int validate(void) {
    char line[MAX_LINE_LEN];
    int line_num = 0;
    int nPoi = 0;
    int nRoads = 0;

    // --- 1. READ POI COUNT ---
    if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
        return 1;
    }
    line_num = 1;

    if (sscanf(line, "%d", &nPoi) != 1) {
        return line_num; 
    }
    if (nPoi <= 0) {
        return line_num; // Rule (d)
    }

    // --- 2. VALIDATE POI SECTION ---
    for (int i = 0; i < nPoi; i++) {
        // Read from stdin
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            return line_num + 1; // File ended prematurely
        }
        line_num++;

        if (line[0] == '\n' || line[0] == '\0') {
            return line_num; // Rule (a)
        }

        char poi_id_str[MAX_LINE_LEN];
        char poi_name[MAX_LINE_LEN];
        double lat, lon;

        // This sscanf POIs (4 fields)
        int items_scanned = sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf", poi_id_str, poi_name, &lat, &lon);

        if (items_scanned != 4) {
            return line_num; // Rule (e)
        }
        if (!is_all_digits(poi_id_str)) {
            return line_num; // Rule (c)
        }
        if (strlen(poi_name) == 0) {
            return line_num; // Rule (f)
        }
        if (lat < -90.0 || lat > 90.0) {
            return line_num; // Errata rule (Latitude range)
        }
        if (lon < -180.0 || lon > 180.0) {
            return line_num; // Errata rule (Longitude range)
        }
    } // End POI loop

    // --- 3. READ ROAD COUNT ---
    // Read from stdin
    if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
        return line_num + 1; 
    }
    line_num++;

    if (sscanf(line, "%d", &nRoads) != 1) {
        return line_num; 
    }
    if (nRoads <= 0) {
        return line_num; // Rule (d)
    }

    // --- 4. VALIDATE ROAD SECTION ---
    for (int i = 0; i < nRoads; i++) {
        // Read from stdin
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            return line_num + 1; // File ended prematurely
        }
        line_num++;

        if (line[0] == '\n' || line[0] == '\0') {
            return line_num; // Rule (a)
        }

        // Buffers for the 6-field road format
        char road_id_str[MAX_LINE_LEN];
        char poi_id_str[MAX_LINE_LEN];
        double distance;
        double lat;
        double lon;
        char road_name[MAX_LINE_LEN];

        //6-FIELD sscanf
        int items_scanned = sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf\t%lf\t%[^\n]", road_id_str, poi_id_str, &distance, &lat, &lon, road_name);
        
        // Check for correct 6-field format
        if (items_scanned != 6) {
            return line_num;
        }

        // Rule (c): Check if ID fields are all digits
        if (!is_all_digits(road_id_str) || 
            !is_all_digits(poi_id_str)) {
            return line_num;
        }

        // Rule (b): Check if road name is empty
        if (strlen(road_name) == 0) {
            return line_num;
        }
    } // End Road loop

    // --- 5. ALL CHECKS PASSED ---
    return 0; // 0 indicates a valid file
}