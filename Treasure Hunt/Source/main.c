#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int* split(char* line, char* tok, int amount){                        /* Splits a given string into array by a token. */
    int i = 0;
    char* p = strtok(line, tok);
    int* array = (int*)malloc(sizeof(int) * amount);
    while(p != NULL){
        array[i++] = atoi(p);   p = strtok(NULL, tok);
    }
    return array;
}
int** readMatrix(FILE* mapFile, int row, int col){             /* Returns a 2d array for holding the matrix elements. */
    char* line = (char*)malloc(col * sizeof(char*));
    int**mapMatrix = (int**)malloc(sizeof(int*) * row);
    int i, r = 0;
    for(i = 0; i < row; i++)
        mapMatrix[i] = (int*)malloc(sizeof(int) * col);
    while(fgets(line, 1000, mapFile) != NULL)
        mapMatrix[r++] = split(line, " ", col);
    free(line);
    return mapMatrix;
}
int matrixMul(int** map, int r, int c, int** key, int n){
    int result = 0, kr = (n-1)/2, kc = (n-1)/2, i, j;                    /* kr, kc : row, column counters for the key.*/
    for(i = -n/2; i < n/2+1; i++){
        for(j = -n/2; j < n/2+1; j++)
            result += (map[r + i][c + j]) * (key[kr + i][kc + j]);}
    return result;
}
int treasureHunt(int** map, int** key, int x, int y, int n, FILE* output, int mapLength, int mapWidth) {
    int result = matrixMul(map, x, y, key, n);
    result = (result % 5 < 0) ? result+5 : result;  /*If result is negative, the correct modulus result is being done.*/

    if (result % 5 == 0) {  /* BASE CASE, In case of Match */
        fprintf(output,"%d,%d:%d", x, y, matrixMul(map, x, y, key, n));
        return 0;}
    else {  /* RECURSIVE CASES: */
        fprintf(output,"%d,%d:%d\n", x, y, matrixMul(map, x, y, key, n));
        if (result % 5 == 1)                                    /* Direction: UP */
            x = (x - n < 0) ? x + n : x - n;                             /* (If upper is bound, Down.)*/
        if (result % 5 == 2)                                    /* Direction: DOWN */
            x = (x + n > mapLength) ? x - n : x + n;                     /* (If Lower is bound, Up.)*/
        if (result % 5 == 3)                                    /* Direction: RIGHT */
            y = (y + n > mapWidth) ? y - n : y + n;                      /* (If right is bound, Left.)*/
        if (result % 5 == 4)                                    /* Direction: LEFT */
            y = (y - n < 0) ? y + n : y - n;                            /* (If left is bound, Right.)*/
        return treasureHunt(map, key, x, y, n, output, mapLength, mapWidth);}
}
int main(int argc, char** argv) {
    /*Getting info about Map and Key Matrices from commandline.*/
    int* sizeOfMap = split(argv[1],"x", 2);                     /* First argument -size of map- is stored in an array */
    int row = sizeOfMap[0], col = sizeOfMap[1], degree = atoi(argv[2]);  /* The rows, columns of Matrices are assigned*/
    FILE *mapMatrix = fopen(argv[3], "r");                                                 /* Reading Map Matrix txt. */
    FILE *keyMatrix = fopen(argv[4], "r");                                                 /* Reading Key Matrix txt. */
    FILE *output = fopen(argv[5], "w");                       /* Creating output file according to the argument name. */
    int** map = readMatrix(mapMatrix, row, col);                               /* Map Matrix Created and Initialized. */
    int** key = readMatrix(keyMatrix, degree, degree);                          /*Key Matrix Created and Initialized. */

    /* Recursive function called and treasure hunting started. */
    treasureHunt(map, key, (degree-1)/2, (degree-1)/2, degree, output, row, col);

    /*Freeing all the allocated memory and closing the opened files.*/
    int i;
    for(i = 0; i < row; i++)
        free(map[i]);
    for(i = 0; i < degree; i++)
        free(key[i]);
    free(key); free(map); free(sizeOfMap);
    fclose(mapMatrix); fclose(keyMatrix); fclose(output);
    return 0;
}