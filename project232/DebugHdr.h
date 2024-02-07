
// Halil Ertan Ömer Köstekcioglu Turgay Korkut Yaman Perker
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE_LENGTH 80
#define MAX_NUM_lINE 100


struct buf {
    char line[MAX_NUM_lINE];
    int link;
};

// Maximum number of code lines and traced variables

#define MAX_TRACED_VARS 20

// Buffer for code lines and traced variables
struct buf Buffer[MAX_LINE_LENGTH];
int head = 0;

// Traced variable information
struct tr {
    char name;
    int value;
};

struct tr Traces[MAX_TRACED_VARS];
int nTr = 0;


// Function to add a traced variable to the Traces table
void add_TT(char var) {
    //Adds a traced variable to the Traces array if the maximum number of traced variables (MAX_TRACED_VARS) has not been reached.
    if (nTr >= MAX_TRACED_VARS) {
        printf("Error: Maximum number of traced variables reached.\n");
        return;
        //If nTr exceed maximum traced variable tell to user, you cannot add more.
    }
    // Generates an entry in the Traces table and inserts the variable name into it.
    Traces[nTr].name = var;
    Traces[nTr].value = 0; //Initializes the value of the traced variable to 0
    nTr++;                 //Add nTr to 1
}

// Function to display the value of a traced variable
void display_TT(char var) {
    for (int i = 0; i < nTr; i++) {                 //Uses for controlling every elements of Traces
        if (Traces[i].name == var) {                //Checks the name of the variable in the Traces table
            printf("%c: %d", var, Traces[i].value); //Prints the name and value like (name: value)
            getchar();                              //Enter a character for continue
            return;
        }
    }
}

// Function to update the value of a traced variable
void update_TT(char var, int val) {
    for (int i = 0; i < nTr; i++) { //Uses for controlling every elements of Traces
        if (Traces[i].name == var) {//Checks the name of the variable in the Traces table
            Traces[i].value = val;  //Updates the value of traced variable to the new value val.
            return;
        }
    }
}







