#ifndef _DATA_H_
#define _DATA_H_
#include <stdio.h>

#define INIT_ARRAY_T_SIZE 1

#define FALSE 0
#define TRUE 1

typedef struct data data_t;
typedef struct array array_t;

// ------------------ STAGE 1 ----------------------

// inserts each field into each variable in data_t.
// we also know the types of the field beforehand.
data_t *build_data(char *lineptr, char *comma, char *quote);

// checks if the start of the string has a quote.
void parseStringField(char **lineptr, char **data, char *comma, char *quote);

// checks if the query is the same as an address in a node
int compareAddress(char *query, data_t *data);

// outputs data of a node to outFile
void outfile_fields(data_t *data, FILE *outFile);

// free strings allocated to data of a node
void free_node_data_strs(data_t *data);

// creates a new data_t. 
// initializes values to 0 and mallocs space for strings
data_t *init_empty_data();

// ------------------ STAGE 2 ----------------------

// returns grade1in from data_t, only accessible in data.c
double getDataGrade(data_t *data);

// compares grade1ins of 2 data_t's for quicksort
int cmp_grade1in(data_t *ptr1, data_t *ptr2);

// ------------------ STAGE 3 ----------------------

// Sort array of data_ts by footpath_id
void sortDatasFootpath(data_t **datas, int num);

// No 2 data_ts will have the same footpath_id, guaranteed with stage4_checkFlag()
int qs_compareFootpath(data_t *a, data_t *b);

// recursive quicksort 
void quicksort(data_t **ptrarray, int num, int compareFunc(data_t *a, data_t *b));

// partitions the array of pointers into less than, equal to and greater
// than the pivot's grade1in value.
void partition(data_t **ptrarray, int num, data_t *pivot,
    int *first_equal, int *first_greater, int compareFunc(data_t *a, data_t *b));

void swap_data(data_t **ptr1, data_t **ptr2);

// ------------------ STAGE 4 ----------------------

// Initializes empty array_t. From week 3 workshops.
array_t *arrayCreate();

// Appends data_t to array_t.
void arrayAppend(array_t *arr, data_t *data);

// Sorts array_t datas by footpath_id.
void sortArrayT(array_t *array); 

// Outputs array to to out and resets duplicate check flag for next query
void outputArray(array_t *array, FILE *out);

void freeArray(array_t *array);

long double startLat(data_t *data);
long double startLon(data_t *data);
long double endLat(data_t *data);
long double endLon(data_t *data);
int footpathID(data_t *data);

// Each record always has 2 lon/lats for start and end and 2 nodes share the same data_t.
// When freeing, free on the second traversal, otherwise 
// the program will access already freed memory.
void free_data_second_pass(data_t *data);

// If data_t for range query in stage 4 has not been traversed,
// return TRUE. Protects against outputting duplicate data_ts.
int stage4_checkFlag(data_t *data);

#endif