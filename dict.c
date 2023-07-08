#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "data.h"
#include "dict.h"

// each node of linked list
struct node {
    node_t *next;
    data_t *data;
};

// linked list of dataset
struct list {
    node_t *head;
    node_t *foot;
    int num;
};

// ------------------ STAGE 1 ----------------------

// builds a linked list where each row from inFile 
// is a node storing data of each field
list_t *build_dict(FILE *inFile) {
    list_t *records = make_empty_list();

    char *lineptr = NULL;
    size_t lineBufferLength = 0;

    // delimiters of field.
    char *comma = ",", *quote = "\"";
    
    // while (fgetc(inFile) != '\n');

    int first = 0;
    // adds each field of inFile in the linked list as data_t
    while (getline(&lineptr, &lineBufferLength, inFile) != -1) {
        if (!first) {
            first = 1;
            continue;
        }
        
        data_t *values = build_data(lineptr, comma, quote);
        insert_at_foot(records, values);
        records->num++;
    }
    free(lineptr);
    lineptr = NULL;
    
    return records;
}

// accepts input from stdin and searches each node for a matching address
void query_dict(list_t *records, FILE *inStream, FILE *outFile, FILE *outStream) {
    char *lineptr = NULL; // points to query from stdin. getline mallocs required memory.
    size_t lineptrLen = 0;
    ssize_t nread;

    int first = 1;
    // loop through inputs from stdin to get query
    while((nread = getline(&lineptr, &lineptrLen, inStream)) != -1) {
        assert(lineptr!=NULL);
        // strcspn and "\r\n" found on StackOverflow
        lineptr[strcspn(lineptr, "\r\n")] = 0; // removes trailing newline

        stage1_outfile(outFile, lineptr, &first);
        int numFound = 0;
        // traverse list nodes and compares query against address
        node_t *curr, *prev;
        curr = records->head;
        while (curr) {
            prev = curr;
            curr = curr->next;
            if (compareAddress(lineptr, prev->data)) {
                outfile_fields(prev->data, outFile);
                numFound++;
            }
        }
        stage1_stdout(outStream, lineptr, numFound);
    }
    free(lineptr);    
}

// outputs query to outfile
void stage1_outfile(FILE *outFile, char *lineptr, int *first) {
    if (*first) {
        fprintf(outFile, "%s", lineptr); // no newline for first entry
        *first = 0;
    } else fprintf(outFile, "\n%s", lineptr);
}

// outputs query to stdout
void stage1_stdout(FILE *outStream, char *lineptr, int numFound) {
    if (!numFound) fprintf(outStream, "%s --> NOTFOUND\n", lineptr);
    else fprintf(outStream, "%s --> %d\n", lineptr, numFound);
}


/* The code below for linked lists is derived from Prof Moffat's COMP10002 textbook. */


// initializes empty linked list
list_t *make_empty_list() {
    list_t *list = malloc(sizeof(list_t));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    list->num = 0;
    return list;
}

// inserts new node with values into the bottom of linked list.
void insert_at_foot(list_t *list, data_t *values) {
    node_t *new = malloc(sizeof(node_t));
    assert(new!=NULL);
    new->data = values;
    new->next = NULL;
    if (list->foot == NULL) {
        list->head = list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
}

// traverses list and free memory used by each node and their data.
void free_list(list_t *list) {
    node_t *curr = list->head, *prev;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free_node_data_strs(prev->data); // free strings
        free(prev->data); // free pointer to data
        free(prev); // free node itself
        prev = NULL;
    }
    free(list);
    list = NULL;
}

// ------------------ STAGE 2 ----------------------

// constructs an array of pointers to each node in the struct
// and sorts the pointers according to the grade1in value
// in the data in each node.
node_t **construct_sorted(list_t *records) {
    node_t **to_sort = malloc(sizeof(node_t*)*records->num);
    assert(to_sort);
    int num = 0;
    // populate array of pointers with pointers to each node
    node_t *curr = records->head, *prev;
    while (curr) {
        prev = curr;
        to_sort[num++] = prev;
        curr = prev->next;
        
    }
    // quicksort(to_sort, records->num);
    return to_sort;
}

// in an array of pointers, swap the 2 elements 
// being pointed by ptr1 and ptr2
void swap_node(node_t **ptr1, node_t **ptr2) {
    node_t *temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

// outputs to outfile for stage 2
void stage2_outfile(FILE *outFile, node_t *node, char *to_find, int *first) {
    if (*first) {
        fprintf(outFile, "%s", to_find);
        *first = 0;
    } else fprintf(outFile, "\n%s", to_find);
}

// outputs to stdout for stage 2
void stage2_stdout(FILE *outStream, char *to_find, node_t *node) {
    fprintf(outStream, "%s --> %.1lf\n", to_find, getDataGrade(node->data));
}

// queries the sorted array of pointers according to grade1in and searches for each query in stdin
void query_index(node_t **ptrarray, list_t *records, FILE *inStream, FILE *outFile, FILE *outStream) {
    char *lineptr = NULL; // points to query from stdin. getline mallocs required memory.
    size_t lineptrlen = 0;
    ssize_t nread;

    int first = 1;

    node_t *curr;

    // loop through inputs from stdin to get query
    while((nread = getline(&lineptr, &lineptrlen, inStream)) != -1) {
        assert(lineptr!=NULL);
        // strcspn and "\r\n" found on StackOverflow
        lineptr[strcspn(lineptr, "\r\n")] = 0; // removes trailing newline
        
        // returns pointer to node with closest grade1in value
        double to_find = atof(lineptr);
        curr = approxBinarySearch(ptrarray, to_find, records->num);

        // output query
        stage2_outfile(outFile, curr, lineptr, &first);
        outfile_fields(curr->data, outFile);
        stage2_stdout(outStream, lineptr, curr);
    }
    free(lineptr); // frees malloc'd memory allocated by getline
}

// iterative binary search for grade1in value of data of node.
// returns pointer to node with closest grade1in value to "find"
node_t *approxBinarySearch(node_t **ptrarray, double find, int num) {
    int left = 0, right = num-1;

    double diff = 0, currgrade1in, middiff, leftdiff, rightdiff;
    node_t *closest = NULL;

    while (left<=right) {
        int mid = left + (right - left)/2;
        currgrade1in = getDataGrade(ptrarray[mid]->data);
        // difference of mid, left and right compared with "find"
        middiff = fabs(find-currgrade1in);
        leftdiff = fabs(find-getDataGrade(ptrarray[left]->data));
        rightdiff = fabs(find-getDataGrade(ptrarray[right]->data));
    
        if (find == currgrade1in) {
            return ptrarray[mid];
        }
        
        // as mid becomes more accurate,
        // check neighbors for closest approx. 
        // Closest may not have been in same partition.
        if (middiff < diff || diff == 0) {
            diff = middiff;
            closest = ptrarray[mid];
        }
        if (leftdiff < diff) {
            diff = leftdiff;
            closest = ptrarray[left];
        }
        if (rightdiff < diff) {
            diff = rightdiff;
            closest = ptrarray[right];
        }
        
        // cuts size in half to search remaining pointers
        if (find < currgrade1in) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    return closest;
}


node_t *getListNode(list_t *records) {
    return records->head;
}
node_t *getNodeNext(node_t *node) {
    return node->next;
}
data_t *getNodeData(node_t *node) {
    return node->data;
}