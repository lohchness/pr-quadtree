#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "data.h"

struct data {
    int footpath_id;   // assume always present. 0 means info not present/applicable
    char *address;    // may be 0 length, can contain spaces
    char *clue_sa;    // may be 0 length, can contain spaces
    char *asset_type; // may be 0 length, can contain spaces
    double deltaz;
    double distance;
    double grade1in;
    int mcc_id;        // assume always present. 0 means info not present/applicable
    int mccid_int;
    double rlmax;
    double rlmin;
    char *segside;    // may be 0 length, can contain spaces
    int statusid;      // assume always present. 0 means info not present/applicable
    int streetid;      // assume always present. 0 means info not present/applicable
    int street_group;
    double start_lat;
    double start_lon;
    double end_lat;
    double end_lon;

    // stage 4
    int firstpass_free;
    int stage4_output_flag;
};

// for storing stage 4 nodes during a range search. From Week 3 workshops.
struct array {
    data_t **datas;
    size_t num;
    size_t size;
};

// inserts each field into each variable in data_t.
// we also know the types of the field beforehand.
data_t *build_data(char *lineptr, char *comma, char *quote) {
    data_t *d = init_empty_data();

	d->footpath_id = atoi(strsep(&lineptr, comma));

    parseStringField(&lineptr, &d->address, comma, quote);
    parseStringField(&lineptr, &d->clue_sa, comma, quote);
    parseStringField(&lineptr, &d->asset_type, comma, quote);

	d->deltaz = atof(strsep(&lineptr, comma));
	d->distance = atof(strsep(&lineptr, comma));
	d->grade1in = atof(strsep(&lineptr, comma));
	d->mcc_id = atoi(strsep(&lineptr, comma));
	d->mccid_int = atoi(strsep(&lineptr, comma));
	d->rlmax = atof(strsep(&lineptr, comma));
	d->rlmin = atof(strsep(&lineptr, comma));

	parseStringField(&lineptr, &d->segside, comma, quote);

	d->statusid = atoi(strsep(&lineptr, comma));
	d->streetid = atoi(strsep(&lineptr, comma));
	d->street_group = atoi(strsep(&lineptr, comma));
	d->start_lat = atof(strsep(&lineptr, comma));
	d->start_lon = atof(strsep(&lineptr, comma));
	d->end_lat = atof(strsep(&lineptr, comma));
	d->end_lon = atof(strsep(&lineptr, comma));
    
    return d;
}

// checks if the start of the string has a quote.
void parseStringField(char **lineptr, char **data, char *comma, char *quote) {
    if (**lineptr == *quote) {
        // consume first quote
        strsep(lineptr, quote);
        // captures string up to next quote
        *data = strdup(strsep(lineptr, quote));
		// consume next comma
		strsep(lineptr, comma);
	}
	else {
        *data = strdup(strsep(lineptr, comma));
	}
    assert(*data!=NULL);
}

// outputs data of a node to outFile
void outfile_fields(data_t *data, FILE *outFile) {
    fprintf(outFile, "\n--> footpath_id: %d || ", data->footpath_id);
    fprintf(outFile, "address: %s || ", data->address);
    fprintf(outFile, "clue_sa: %s || ", data->clue_sa);
    fprintf(outFile, "asset_type: %s || ", data->asset_type);
    fprintf(outFile, "deltaz: %.2lf || ", data->deltaz);
    fprintf(outFile, "distance: %.2lf || ", data->distance);
    fprintf(outFile, "grade1in: %.1lf || ", data->grade1in);
    fprintf(outFile, "mcc_id: %d || ", data->mcc_id);
    fprintf(outFile, "mccid_int: %d || ", data->mccid_int);
    fprintf(outFile, "rlmax: %.2lf || ", data->rlmax);
    fprintf(outFile, "rlmin: %.2lf || ", data->rlmin);
    fprintf(outFile, "segside: %s || ", data->segside);
    fprintf(outFile, "statusid: %d || ", data->statusid);
    fprintf(outFile, "streetid: %d || ", data->streetid);
    fprintf(outFile, "street_group: %d || ", data->street_group);
    fprintf(outFile, "start_lat: %lf || ", data->start_lat);
    fprintf(outFile, "start_lon: %lf || ", data->start_lon);
    fprintf(outFile, "end_lat: %lf || ", data->end_lat);
    fprintf(outFile, "end_lon: %lf || ", data->end_lon);
}

// free strings allocated to data of a node
void free_node_data_strs(data_t *data) {
    free(data->address);
    free(data->clue_sa);
    free(data->asset_type);
    free(data->segside);
    data->address = NULL;
    data->clue_sa = NULL;
    data->asset_type = NULL;
    data->segside = NULL;
}

// creates a new data_t. 
// initializes values to 0 and mallocs space for strings
data_t *init_empty_data() {
    data_t *d = malloc(sizeof(data_t));
    assert(d!=NULL);

    d->footpath_id = 0;
    d->address = NULL;
    d->clue_sa = NULL;
    d->asset_type = NULL;
    d->deltaz = 0.0;
    d->distance = 0.0;
    d->grade1in = 0.0;
    d->mcc_id = 0;
    d->mccid_int = 0;
    d->rlmax = 0.0;
    d->rlmin = 0.0;
    d->segside = NULL;
    d->statusid = 0;
    d->streetid = 0;
    d->street_group = 0;
    d->start_lat = 0.0;
    d->start_lon = 0.0;
    d->end_lat = 0.0;
    d->end_lon = 0.0;

    d->firstpass_free = 0;
    d->stage4_output_flag = 0;

    return d;
}

// checks if the query is the same as an address in a node
int compareAddress(char *query, data_t *data) {
    assert(data->address!=NULL);
    if (data->address[0] == '\0') return 0;
    if (strcmp(query, data->address) == 0) return 1;
    return 0;
}

// ------------------ STAGE 2 ----------------------

// returns grade1in from data_t, only accessible in data.c
double getDataGrade(data_t *data) {
    return data->grade1in;
}

// compares grade1ins of 2 data_t's for quicksort
int cmp_grade1in(data_t *ptr1, data_t *ptr2) {
    if (ptr1->grade1in > ptr2->grade1in) return 1;
    if (ptr1->grade1in < ptr2->grade1in) return -1;
    return 0;
}

// ------------------ STAGE 3 ----------------------

// Sort array of data_t * by footpath_id
void sortDatasFootpath(data_t **datas, int num) {
    quicksort(datas, num, qs_compareFootpath);
    // This qsort from C nearly works but again, if it aint broke, dont fix it
    // qsort(datas, num, sizeof(data_t*), compareFootpath);
}

// No 2 data_ts will have the same footpath_id, guaranteed with stage4_checkFlag()
int qs_compareFootpath(data_t *a, data_t *b) {
    return (b->footpath_id < a->footpath_id) ? 1 : -1;
    // return ((*(data_t *)b).footpath_id > (*(data_t *)a).footpath_id ? 1 : -1);
}

/* The code below for quicksort is derived from Prof Moffat's COMP10002 textbook. */

// recursive quicksort 
void quicksort(data_t **ptrarray, int num, int compareFunc(data_t *a, data_t *b)) {
    if (num<=1) return;
    int first_eq, first_gt;
    data_t *pivot = ptrarray[rand()%num];
    partition(ptrarray, num, pivot, &first_eq, &first_gt, compareFunc);
    quicksort(ptrarray, first_eq, compareFunc);
    quicksort(ptrarray+first_gt, num-first_gt, compareFunc);
}

// partitions the array of pointers into less than, equal to and greater
// than the pivot's grade1in value.
void partition(data_t **ptrarray, int num, data_t *pivot,
        int *first_equal, int *first_greater, int compareFunc(data_t *a, data_t *b)) {
    int next=0, first_eq=0, first_gt=num, outcome;

    // breaks when no elements left in unknown area.
    // elements in this partition are sorted.
    while (next<first_gt) {
        if ((outcome = compareFunc((*(ptrarray+next)), pivot)) == 1) {
            first_gt--; // 1 element added to "greater than" area
            swap_data((ptrarray+next), (ptrarray+first_gt));
        } else if (outcome == -1) {
            swap_data((ptrarray+first_eq), (ptrarray+next));
            first_eq++; // 1 element is added to "less than" area
            next++;
        } else {
            next++; // equal to, no change
        }
    }
    *first_equal = first_eq;
    *first_greater = first_gt;
}

void swap_data(data_t **ptr1, data_t **ptr2) {
    data_t *temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

// ------------------ STAGE 4 ----------------------

// Initializes empty array_t. From week 3 workshops.
array_t *arrayCreate() {
    array_t *arr = malloc(sizeof(array_t));
	assert(arr);
	arr->size = INIT_ARRAY_T_SIZE;
	arr->datas = malloc(sizeof(data_t*)*arr->size);
	assert(arr->datas);
	arr->num = 0;
	return arr;
}

// Appends data_t to array_t.
void arrayAppend(array_t *arr, data_t *data) {
    // Ensures array size.
    if (arr->num == arr->size) {
		arr->size <<= 1;
		arr->datas = realloc(arr->datas,sizeof(data_t*)*arr->size);
        assert(arr->datas);
	}
    arr->datas[arr->num] = data;
    arr->num++;
}

// Sorts array_t datas by footpath_id. Quadtree cannot see contents
// of array_t, so it will need to be passed to Data
void sortArrayT(array_t *array) {
    sortDatasFootpath(array->datas,array->num);
}

// Outputs array to to out and resets duplicate check flag for next query
void outputArray(array_t *array, FILE *out) {
    for (int i=0; i<array->num; i++) {
        outfile_fields(array->datas[i], out);
        array->datas[i]->stage4_output_flag = 0;
    }
}

void freeArray(array_t *array) {
    free(array->datas);
    free(array);
}

long double startLat(data_t *data) {
    return data->start_lat;
}
long double startLon(data_t *data) {
    return data->start_lon;
}
long double endLat(data_t *data) {
    return data->end_lat;
}
long double endLon(data_t *data) {
    return data->end_lon;
}
int footpathID(data_t *data) {
    return data->footpath_id;
}

// Each record always has 2 lon/lats for start and end and 2 nodes share the same data_t.
// When freeing, free on the second traversal, otherwise 
// the program will access already freed memory.
void free_data_second_pass(data_t *data) {
    if (data->firstpass_free==0) {
        data->firstpass_free = 1;
        return;
    }
    free_node_data_strs(data);
    free(data);
}

// If data_t for range query in stage 4 has not been traversed,
// return TRUE. Protects against outputting duplicate data_ts.
int stage4_checkFlag(data_t *data) {
    if (data->stage4_output_flag == 0) {
        data->stage4_output_flag = 1;
        return TRUE;
    }
    return FALSE;
}