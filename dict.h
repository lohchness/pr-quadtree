#ifndef _DICT_H_
#define _DICT_H_
#include <stdio.h>

// needed to insert data_t into foot of linked list
#include "data.h"

typedef struct node node_t;
typedef struct list list_t;

// ------------------ STAGE 1 ----------------------

// builds a linked list where each row from infoFile 
// is a node storing data of each field.
list_t *build_dict(FILE *infoFile);

// accepts input from stdin and searches each node for a matching address
void query_dict(list_t *records, FILE *inStream, FILE *outFile, FILE *outStream);

// outputs query to outfile
void stage1_outfile(FILE *outFile, char *buf, int *first);

// outputs query to stdout
void stage1_stdout(FILE *outStream, char *lineptr, int numFound);

// initializes empty linked list
list_t *make_empty_list();

// inserts new node with values into the bottom of linked list
void insert_at_foot(list_t *list, data_t *values);

// traverses list and free memory used by each node and their data.
void free_list(list_t *list);

// ------------------ STAGE 2 ----------------------

// constructs an array of pointers to each node in the struct
// and sorts the pointers according to the grade1in value
// in the data in each node.
node_t **construct_sorted(list_t *records);

// in an array of pointers, swap the 2 elements 
// being pointed by ptr1 and ptr2
void swap_node(node_t **ptr1, node_t **ptr2);

// outputs to outfile for stage 2
void stage2_outfile(FILE *outFile, node_t *node, char *to_find, int *first);

// outputs to stdout for stage 2
void stage2_stdout(FILE *outStream, char *to_find, node_t *node);

// queries the sorted array of pointers according to grade1in and searches for each query in stdin
void query_index(node_t **ptrarray, list_t *records, FILE *inStream, FILE *outFile, FILE *outStream);

// iterative binary search for grade1in value of data of node.
// returns pointer to node with closest grade1in value to "find"
node_t *approxBinarySearch(node_t **ptrarray, double find, int num);



node_t *getListNode(list_t *records);
node_t *getNodeNext(node_t *node);
data_t *getNodeData(node_t *node);


#endif