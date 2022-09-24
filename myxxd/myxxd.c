#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1

int BYTESPACES;

/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
    if (argc > 2) {
        printf("Usage: %s [-b|-bits]\n", argv[0]);
        exit(BAD_NUMBER_ARGS);
    }

    if (argc == 2 && (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
        *bits = TRUE;
    } else {
        *bits = FALSE;
    }

    return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) {
    int counter = 0;
    BYTESPACES = 0;
    printf(" ");
    for (int i = 0; i < size; i++) {
        printf("%02x", data[i]);
        counter++;
        if ((counter % 2 == 0) && (i < size - 1)) {
            printf(" ");
            BYTESPACES++;
        }

    }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
    int spacer = 39 - (size * 2) - BYTESPACES;

    for (int i = 0; i < spacer; i++) {
        if (size == 16) {
            break;
        }
        printf(" ");
    }
    for (int i = 0; i < size; i++) {
        int caster = (int) data[i];
        if (caster > 127){
            printf(".");
        }
        else {
            printf("%c", data[i]);
        }
    }
}

void printDataAsBits(unsigned char *data, size_t size){
    BYTESPACES = 0;
    int binaryStorage [size][8];
    printf(" ");

    for (int ROW = 0; ROW < size; ROW++){
        int caster = (int) data[ROW];
        for (int COL = 0; COL < 8; COL++){
            if (caster % 2 == 1) {
                binaryStorage [ROW][COL] = 1;
            }
            else {
                binaryStorage [ROW][COL] = 0;
            }
            caster = caster / 2;
        }

    }

    for (int ROW = 0; ROW < size; ROW++){
        for(int COL = 7; COL >= 0; COL--){
            printf("%d", binaryStorage[ROW][COL]);
        }
        printf(" ");
        BYTESPACES++;
    }
}

void readAndPrintInputAsHex(FILE *input) {
    unsigned char data[16];
    int numBytesRead = fread(data, 1, 16, input);
    unsigned int offset = 0;
    while (numBytesRead != 0) {
        printf("%08x:", offset);
        offset += numBytesRead;
        printDataAsHex(data, numBytesRead);
        printf("  ");
        printDataAsChars(data, numBytesRead);
        printf("\n");
        numBytesRead = fread(data, 1, 16, input);
    }
}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE *input) {
    unsigned char data[6];
    int numBytesRead = fread(data, 1, 6, input);
    unsigned int offset = 0;
    while (numBytesRead != 0) {
        printf("%08x:", offset);
        offset += numBytesRead;
        printDataAsBits(data, numBytesRead);
        printf("  ");
        printDataAsChars(data, numBytesRead);
        printf("\n");
        numBytesRead = fread(data, 1, 6, input);
    }
}

int main(int argc, char **argv) {
    int bits = FALSE;
    FILE *input = parseCommandLine(argc, argv, &bits);

    if (bits == FALSE) {
        readAndPrintInputAsHex(input);
    } else {
        readAndPrintInputAsBits(input);
    }
    return 0;
}
