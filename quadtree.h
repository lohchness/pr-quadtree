#ifndef _QUADTREE_H_
#define _QUADTREE_H_
#include<stdio.h>
#include "data.h"

#define QUADRANTS 4

#define WHITE 0
#define BLACK 1
#define GREY 2

#define SW 0
#define NW 1
#define NE 2
#define SE 3

typedef struct point point_t;
typedef struct rectangle rectangle_t;
typedef struct treeNode treeNode_t;

// Initializes point_t with given lon/x and lat/y
point_t *init_point(long double x, long double y);

// Initializes new non empty rect with given botLeft and topRight point_ts.
rectangle_t *init_rect(point_t *newbotLeft, point_t *newtopRight);

// Empty tree node and empty array of tree nodes all set to NULL.
treeNode_t *init_empty_treeNode();

// Given a rectangle and point, returns the quadrant of the rectangle that the point lies in. 
int determineQuadrant(rectangle_t *rect, point_t *point);

// Makes new smaller sized rectangle based on cardinality
rectangle_t *makeSmallerRect(rectangle_t *rect, int quadrant);

// BLACK - leaf node taken by data_t
// GREY - node contains child nodes
// WHITE - empty leaf node 
int treeNodeType(treeNode_t *node);

// Used for stage 3 and 4 alongside determineQuadrant when querying.
void output_direction(FILE *outStream, int quadrant);

// Recursive function to add a point in the quadtree.
void addPoint(treeNode_t *head, point_t *point, data_t *data);

// Similar to A1, read in CSV for start/end lon/lat coordinates and adds to the quadtree.
treeNode_t *build_quadtree(char **argv, FILE *inFile);

// Recursively frees quadtree.
// Since 2 nodes always shares the same data_t, and vice versa (can assume)
// a function is used to check if the data_t has been traversed once already.
void freeQuadtree(treeNode_t *head);

void freeRect(rectangle_t *rect);

// ---------------- STAGE 3 ----------------

// Traverses quadtree. Even if point of the node is not equal to the query point 
// but the node is black, it still returns the point in the node.
void stage3_traverse(treeNode_t *head, point_t *query, FILE *outStream, FILE *outFile);

// Accepts lon/lat coordinates from stdin and searches each node for the query given.
void stage3_query(treeNode_t *root, FILE *inStream, FILE *outFile, FILE *outStream);

// ---------------- STAGE 4 ----------------

// Accepts 2 lon/lat coordinates from stdin for a rectangle and searches each node for the query given.
void stage4_query(treeNode_t *root, FILE *inStream, FILE *outFile, FILE *outStream);

// Appends data_t to array_t.
void arrayAppend(array_t *arr, data_t *data);

// Checks if a rectangle collides with another rectangle
int rect_overlap(rectangle_t *rect1, rectangle_t *rect2);

// Checks if a point lies within a rectangle
int point_in_rect(point_t *point, rectangle_t *rect);

// Traverses quadtree and determines which child nodes the range query is overlapping.
// Adds all points inside the range query to array_t.
void traverse_query_stage4(treeNode_t *head, rectangle_t *query, FILE *outStream, FILE *outFile, array_t *to_output);

#endif