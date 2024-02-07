
// Halil Ertan Ömer Köstekcioglu Turgay Korkut Yaman Perker

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DebugHdr.h"


// Defines a function to read contents of a file into a buffer
void read_to_buffer(const char *filename) {
    // Opens the file "myprog.c" for reading and assigns it to file pointer
    FILE *file = fopen("myprog.c", "r");

    // Checks if the file was successfully opened
    if (file == NULL) {
        // Prints an error message and exits if the file could not be opened
        printw("Error: Unable to open myprog.c\n");
        endwin(); // Ends the window session (typically used in curses library)
        exit(1); // Exits the program with a status code of 1 (indicating an error)
    }

    // Declares a character array to hold each line from the file
    char line[MAX_LINE_LENGTH];
    // A counter to keep track of the current line number
    int current = 0;

    // Reads each line from the file until end of file is reached
    while (fgets(line, sizeof(line), file) != NULL) {
        // Copies the line into the buffer, ensuring not to exceed its size
        strncpy(Buffer[current].line, line, sizeof(Buffer[current].line) - 1);
        // Ensures the last character in the buffer is null-terminated
        Buffer[current].line[sizeof(Buffer[current].line) - 1] = '\0';
        // Sets up a link to the next line
        Buffer[current].link = current + 1;
        // Increments the line counter
        current++;
    }

    // Sets the link of the last line in the buffer to -1
    if (current > 0) {
        Buffer[current - 1].link = -1;
    }
    // Stores the number of lines read into a global variable 'head'
    head = current;
    // Closes the file
    fclose(file);
}


// Function to add a new line with directives to the buffer at a specified position
void add_to_buffer(const char *directive, int cursor_position) {
    // Checks if the buffer is full
    if (head >= MAX_LINE_LENGTH) {
        printw("Error: Buffer is full. Cannot add more directives.\n");    //Prints a message on UI
        return;
    }

    // Counts leading whitespaces (spaces and tabs) in the current line
    int whitespace_count = 0;
    while(Buffer[cursor_position].line[whitespace_count] == ' ' || Buffer[cursor_position].line[whitespace_count] == '\t') {
        whitespace_count++;
    }

    // Shift existing lines to make room for the new directives
    for (int i = head; i > cursor_position; --i) {
        // Copy the line from the previous position to the current position
        strncpy(Buffer[i].line, Buffer[i - 1].line, MAX_LINE_LENGTH);
        // Ensures null-termination of the line
        Buffer[i].line[MAX_LINE_LENGTH - 1] = '\0';
        // Updates the link to the next line
        Buffer[i].link = i + 1;
    }

    // Formats and inserts the new directive with a newline character in this part
    char formatted_directive[MAX_LINE_LENGTH];
    // Creates a formatted directive with proper indentation
    snprintf(formatted_directive, MAX_LINE_LENGTH, "%*s%s\n", whitespace_count, "", directive);
    // Copies the formatted directive to the buffer
    strncpy(Buffer[cursor_position + 1].line, formatted_directive, MAX_LINE_LENGTH - 1);
    // Ensure null-termination of the line
    Buffer[cursor_position + 1].line[MAX_LINE_LENGTH - 1] = '\0'; // Ensure null-termination
    // Updates the link to the next line
    Buffer[cursor_position + 1].link = cursor_position + 2;

    // Increment the head as we have added a new line
    head++;
}


// Function to write the buffer to the temp file
void write_to_file() {
    //Opens temp file for writing
    FILE *file = fopen("temp", "w");

    //Checks if the file is opened without any issue
    if (file == NULL) {
        printw("Error: Unable to open temp file for writing.\n");   //Prints a message on UI
        endwin();   //Ends curses mode
        exit(1);    //Terminates the program
    }

    //Iterates over each line in buffer and writes each line to the opened temp file
    for (int i = 0; i < head; i++) {
        fprintf(file, "%s", Buffer[i].line);
    }

    //Closes the file
    fclose(file);
}

void process_directives() {
    FILE *inputFile = fopen("temp", "r");
    FILE *outputFile = fopen("expanded.c", "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error: Unable to open files for processing.\n");
        exit(1);
    } //test open file

    char line[MAX_LINE_LENGTH];
    char *foundDirective; //array
    int inLoop = 0;
    char lhsVar;

    while (fgets(line, sizeof(line), inputFile) != NULL) {
        // Determine epmty spaces before ligne
        int indentation = 0;
        while (line[indentation] == ' ' || line[indentation] == '\t') {
            indentation++;
        }

        if (strstr(line, "while(") || strstr(line, "for(")) { //while, for loop içinde mi
            inLoop = 1;
        } else if (strstr(line, "}") && inLoop) {
            inLoop = 0;
        }

        foundDirective = strstr(line, "@var ");
        if (foundDirective != NULL) {
            for (int j = 0; j < indentation; j++) {
                fputc(' ', outputFile); // Add spaces
            }
            char varName = foundDirective[5]; //@var a 5.karakter
            add_TT(varName);
            fprintf(outputFile, "add_TT('%c');\n", varName);
            continue;
        }

        foundDirective = strstr(line, "@trace ");
        if (foundDirective != NULL) {
            for (int j = 0; j < indentation; j++) {
                fputc(' ', outputFile); // Add empty spaces
            }
            char varName = foundDirective[7]; //@trace a 7.karakter

            fprintf(outputFile, "display_TT('%c');\n", varName);
            continue;
        }

        if (strchr(line, '=')) {
            if (sscanf(line, " %c =", &lhsVar) == 1) {
                fprintf(outputFile, "%s", line); // Write the original line

                for (int i = 0; i < nTr; i++) {
                    if (lhsVar == Traces[i].name) {
                        for (int j = 0; j < indentation; j++) {
                            fputc(' ', outputFile); // Add empty spaces
                        }
                        update_TT(lhsVar,Traces[i].value);
                        fprintf(outputFile, "update_TT('%c', %c);\n", lhsVar, lhsVar);
                        break;
                    }
                }
            }
        } else {
            fprintf(outputFile, "%s", line); // Write other lines directly
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}//close file after use or error might occur



// Main function
int main() {
    // Variable declarations
    int ch, row, col, x, y; // For storing character input, screen dimensions, and cursor positions
    int current_line = 0; // Variable to track the current line being edited

    // Initialize NCURSES
    initscr(); // Start curses mode
    cbreak(); // Disable line buffering, pass key presses directly to the program
    keypad(stdscr, TRUE); // Enable function keys
    noecho(); // Disable echoing of characters typed by the user
    curs_set(1); // Make the cursor visible

    // Read the file into the buffer
    read_to_buffer("myprog.c"); // Function call to read file contents into a buffer

    // Main loop
    while (1) {
        clear(); // Clear the screen
        getmaxyx(stdscr, row, col);  // Get the size of the screen

        int current = 0; // Variable to track the current line in the buffer
        int line_num = 0; // Variable to track the line number
        // Loop to display each line from the buffer
        while (current != -1 && current < head) {
            // Highlight the current line
            if (current == current_line) {
                attron(A_STANDOUT); // Turn on attribute for standout (highlight)
                move(current, 0);  // Move cursor to the beginning of the current line
                addch('>');  // Add a marker ('>') to highlight the current line
                attroff(A_STANDOUT); // Turn off the standout attribute
            }

            // Print each line from the buffer
            mvprintw(current, 1, "%s", Buffer[current].line);  // Print each line with an offset for the marker
            current = Buffer[current].link; // Move to the next line in the buffer
            line_num++; // Increment line number
        }
        refresh(); // Update the screen with the changes

        // Wait for user input and process it
        ch = getch(); // Get character input from user
        switch (ch) { // Switch based on the character input
            case 'w': // If 'w' is pressed
                if (current_line > 0) current_line--; // Move up one line
                break;
            case 's': // If 's' is pressed
                if (current_line < head - 1) current_line++; // Move down one line
                break;
            case 'i': // If 'i' is pressed
                echo(); // Enable echoing of characters
                curs_set(1); // Make cursor visible
                char directive[MAX_LINE_LENGTH]; // Declare a variable to store the directive
                move(line_num, 0);  // Move cursor to the bottom of the screen
                getyx(stdscr, y, x);  // Get current cursor position
                getstr(directive); // Read a string from the user
                add_to_buffer(directive, current_line); // Add the user's input to the buffer
                noecho(); // Disable echoing of characters
                curs_set(0); // Make cursor invisible
                if (current_line < head - 1) current_line++; // Move down one line
               // process_directives(); // Process directives
                break;
            case 'x': // If 'x' is pressed
                // Save changes and exit
                write_to_file(); // Write the buffer to a file
                process_directives(); // Process directives
                endwin(); // End curses mode
                return 0; // Exit the program
            case 'd' : // If 'd' is pressed
                // Write changes and wait for another input
                write_to_file(); // Write the buffer to a file
                process_directives(); // Process directives
                refresh(); // Refresh the screen
                getch(); // Wait for another character input
                break;
        }
    }

    endwin(); // End curses mode
    return 0; // Exit the program
}
