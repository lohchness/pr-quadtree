#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "quadtree.h"

struct point {
	long double x;
	long double y;
};

struct rectangle {
	point_t *topRight;
	point_t *botLeft;
};

// each quadtree node
struct treeNode {
	rectangle_t *rect;  // always defined
	treeNode_t **nodes; // defined if grey
	data_t **data;      // defined if black
	size_t num;         // defined if black
    size_t size;        // defined if black
	point_t *point;     // defined if black
};

// Initializes point_t with given lon/x and lat/y
point_t *init_point(long double x, long double y) {
	point_t *new = malloc(sizeof(point_t));
	assert(new);
	new->x = x;
	new->y = y;
	return new;
}

// Initializes new non empty rect with given botLeft and topRight point_ts.
rectangle_t *init_rect(point_t *newbotLeft, point_t *newtopRight) {
	rectangle_t *new = malloc(sizeof(rectangle_t));
	assert(new);
	new->botLeft = init_point(newbotLeft->x, newbotLeft->y);
	new->topRight = init_point(newtopRight->x, newtopRight->y);
	return new;
}

// Empty tree node and empty array of tree nodes all set to NULL.
treeNode_t *init_empty_treeNode() {
	treeNode_t *new = malloc(sizeof(treeNode_t));
	assert(new);
	new->num = 0;
    new->size = 0;
    new->data = NULL;
	new->point = NULL;
	new->nodes = NULL;
	new->rect = NULL;
	return new;
}

// Given a rectangle and point, returns the quadrant of the rectangle that the point lies in. 
int determineQuadrant(rectangle_t *rect, point_t *point) {

	long double midpoint_x = rect->topRight->x-(rect->topRight->x - rect->botLeft->x)/2;
	long double midpoint_y = rect->topRight->y-(rect->topRight->y - rect->botLeft->y)/2;

	if (point->x <= midpoint_x && point->y < midpoint_y) return SW;
	else if (point->x <= midpoint_x && point->y >= midpoint_y) return NW;
	else if (point->x > midpoint_x && point->y >= midpoint_y) return NE;
	else if (point->x > midpoint_x && point->y < midpoint_y) return SE;
	else {
        fprintf(stdout, "Unhandled exception in determineQuadrant.\n");
        exit(EXIT_FAILURE);
    }
}

// Makes new smaller sized rectangle based on cardinality
rectangle_t *makeSmallerRect(rectangle_t *rect, int quadrant) {
	rectangle_t *new = init_rect(rect->botLeft, rect->topRight);
    assert(new);

    long double midpoint_x = new->topRight->x-(new->topRight->x-new->botLeft->x)/2;
    long double midpoint_y = new->topRight->y-(new->topRight->y-new->botLeft->y)/2;

	switch (quadrant) {
	case SW:
		new->topRight->x = midpoint_x;
		new->topRight->y = midpoint_y;
		break;
	case NW:
		new->topRight->x = midpoint_x;
		new->botLeft->y = midpoint_y;
		break;
	case NE:
		new->botLeft->x = midpoint_x;
		new->botLeft->y = midpoint_y;
		break;
	case SE:
		new->botLeft->x = midpoint_x;
		new->topRight->y = midpoint_y;
		break;
	}
	return new;
}

// BLACK - leaf node taken by data_t
// GREY - node contains child nodes
// WHITE - empty leaf node 
int treeNodeType(treeNode_t *node) {
	assert(node);
	if (node->num>0&&node->data) return BLACK;
	if (node->nodes) return GREY;
	assert(!node->data);
	return WHITE;
}

// Used for stage 3 and 4 alongside determineQuadrant when querying.
void output_direction(FILE *outStream, int quadrant) {
    switch (quadrant) {
        case SW:
            fprintf(outStream," SW");
            break;
        case NW:
            fprintf(outStream," NW");
            break;
        case NE:
            fprintf(outStream," NE");
            break;
        case SE:
            fprintf(outStream," SE");
            break;
    }
}

// Recursive function to add a point in the quadtree.
void addPoint(treeNode_t *head, point_t *point, data_t *data) {
	switch (treeNodeType(head)) {
        // Insert into leaf nodes of tree
        case WHITE:
            head->size++;
            head->data = malloc(sizeof(data_t*)*(head->size));
            assert(head->data);
            head->data[head->num++] = data;
            head->point = init_point(point->x, point->y); // if it aint broke, dont fix it
            assert(head->point);
            break;
        
        // Determine which direction the point will be inserted
        case GREY:
            int index = determineQuadrant(head->rect, point);
            addPoint(head->nodes[index], point, data);
            break;
        
        // Node is already taken.
        case BLACK:
            // if points are exactly the same, realloc data_t** to accomodate for new data_t
            if (head->point->x == point->x &&
            head->point->y == point->y) {
                if (head->num >= head->size) {
                    head->size *= 2;
                    head->data = realloc(head->data, sizeof(data_t*)*(head->size));
                    assert(head->data);
                }
                head->data[head->num++] = data;
                return;
            }

            // Subdivide current head into 4. Define rectangle_t of each of the 4 children.
            head->nodes = malloc(sizeof(treeNode_t*)*QUADRANTS);
            assert(head->nodes);
            for (int quadrant=0; quadrant<QUADRANTS; quadrant++) {
                head->nodes[quadrant] = init_empty_treeNode();
                // each node will have 4 equally sized rectangles.
                head->nodes[quadrant]->rect = makeSmallerRect(head->rect, quadrant);
            }
            
            // Extract data of current node and re-insert into the tree
            data_t **originalDataArray = head->data;
            point_t *originalPoint = head->point;
            size_t originalNum = head->num;

            // reset node values, is now grey
            head->data = NULL;
            head->num = 0;
            head->size = 0;
            head->point = NULL;

            // inserting the initial data before re-inserting the original data_t(s)
            // is cheaper if there are multiple data_ts in the original node
            addPoint(head, point, data);
            // re-insert original data one by one. Realloc will have to run log(n) times.
            for (int i=0; i<originalNum; i++) {
                addPoint(head,originalPoint,originalDataArray[i]);
            }

            // when inserting into a white/black node, new space will be malloc'd.
            free(originalDataArray);
            free(originalPoint);
            break;
	}
    return;
}

// Similar to A1, read in CSV for start/end lon/lat coordinates and adds to the quadtree.
treeNode_t *build_quadtree(char **argv, FILE *inFile) {
	// Root node
	treeNode_t *head = init_empty_treeNode();
	point_t *pointBotLeft = init_point(strtold(argv[4],NULL), strtold(argv[5],NULL));
	point_t *pointTopRight = init_point(strtold(argv[6],NULL), strtold(argv[7],NULL));
	rectangle_t *root_rect = init_rect(pointBotLeft, pointTopRight);
	head->rect = root_rect;

	// delimiters of field.
	char *comma = ",", *quote = "\"";

	char *lineptr = NULL;
	size_t len = 0;

	int readFirstLine = 0;
	while (getline(&lineptr, &len, inFile) > 0) {
		if (!readFirstLine) {
			readFirstLine = 1;
			continue;
		}
        // Each record in CSV has 2 points, start and end lon/lat.
		data_t *data = build_data(lineptr, comma, quote);
        // 2 nodes share the same data_t.
        point_t *startpt = init_point(startLon(data),startLat(data));
        point_t *endpt = init_point(endLon(data),endLat(data));
		addPoint(head,startpt,data);
		addPoint(head,endpt,data);
        free(startpt); // TODO - Change if bothered
        free(endpt);
	}

    free(pointBotLeft);
    free(pointTopRight);
	free(lineptr);

	return head;
}

// Recursively frees quadtree.
// Since 2 nodes always shares the same data_t, and vice versa (can assume)
// a function is used to check if the data_t has been traversed once already.
void freeQuadtree(treeNode_t *head) {
    switch (treeNodeType(head)) {
        case WHITE:
            break;
        case BLACK:
            // Ensure does not free same data twice.
            for (int i=0; i<head->num; i++) {
                free_data_second_pass(head->data[i]);
            }
            free(head->data);
            free(head->point);
            break;
        case GREY:
            for (int i=0; i<QUADRANTS; i++) {
                freeQuadtree(head->nodes[i]);
            }
            free(head->nodes);
            break;
    }
    // Defined in every node
    freeRect(head->rect);
    free(head);
}

void freeRect(rectangle_t *rect) {
    free(rect->botLeft);
    free(rect->topRight);
    free(rect);
}

// ------------------ STAGE 3 ----------------------

// Traverses quadtree. Even if point of the node is not equal to the query point 
// but the node is black, it still returns the point in the node.
void stage3_traverse(treeNode_t *head, point_t *query, FILE *outStream, FILE *outFile) {
	switch (treeNodeType(head)) {
        case BLACK:
            // sort data_t array
            sortDatasFootpath(head->data, head->num);

            // write data_ts to outFile
            for (int i=0; i<head->num; i++) {
                outfile_fields(head->data[i],outFile);
            }
            break;
        case WHITE:
            fprintf(outFile, "\nNOT FOUND");
            fprintf(outStream, " NOT FOUND");
            break;
        case GREY:
            int quadrant = determineQuadrant(head->rect, query);
            output_direction(outStream, quadrant);
            stage3_traverse(head->nodes[quadrant], query, outStream, outFile);
            break;
	}
}

// Accepts lon/lat coordinates from stdin and searches each node for the query given.
void stage3_query(treeNode_t *root, FILE *inStream, FILE *outFile, FILE *outStream) {
	char *lineptr = NULL; // points to query from stdin. getline mallocs required memory.
	size_t lineptrLen = 0;
	ssize_t nread;

	char *delim = " ";

	while((nread = getline(&lineptr, &lineptrLen, inStream)) != -1) {
		assert(lineptr);
		// strcspn and "\r\n" found on StackOverflow
		lineptr[strcspn(lineptr, "\r\n")] = 0; // removes trailing newline
		
        fprintf(outStream, "%s -->", lineptr);
        fprintf(outFile, "%s", lineptr);

		char *lonQuery = strsep(&lineptr, delim);
		char *latQuery = strsep(&lineptr, delim);
		double longitudeQuery = strtold(lonQuery, NULL);
		double latitudeQuery = strtold(latQuery, NULL);

		point_t *toQuery = init_point(longitudeQuery, latitudeQuery);
		stage3_traverse(root, toQuery, outStream, outFile);
        
        free(lonQuery);
        free(toQuery);

        fprintf(outStream, "\n");
        fprintf(outFile, "\n");
    }
	free(lineptr);
}

// ------------------ STAGE 4 ----------------------

// Accepts 2 lon/lat coordinates from stdin for a rectangle and searches each node for the query given.
void stage4_query(treeNode_t *root, FILE *inStream, FILE *outFile, FILE *outStream) {
	char *lineptr = NULL; // points to query from stdin. getline mallocs required memory.
	size_t len = 0;
	ssize_t nread;

	const char *delim = " ";

	while((nread = getline(&lineptr, &len, inStream)) != -1) {
		assert(lineptr);
		// strcspn and "\r\n" found on StackOverflow
		lineptr[strcspn(lineptr, "\r\n")] = 0; // removes trailing newline
		
        fprintf(outStream, "%s -->", lineptr);
        fprintf(outFile, "%s", lineptr);

		char *BotLeft_x = strsep(&lineptr, delim);
		char *BotLeft_y = strsep(&lineptr, delim);
        char *TopRight_x = strsep(&lineptr, delim);
        char *TopRight_y = strsep(&lineptr, delim);

		point_t *pointBotLeft = init_point(strtold(BotLeft_x, NULL), strtold(BotLeft_y, NULL));
        point_t *pointTopRight = init_point(strtold(TopRight_x, NULL), strtold(TopRight_y, NULL));
        rectangle_t *queryRect = init_rect(pointBotLeft, pointTopRight);
        array_t *to_output = arrayCreate(); // Array struct for the purposes of stage 4. See data.c

        traverse_query_stage4(root, queryRect, outStream, outFile, to_output);
    
        sortArrayT(to_output);
        outputArray(to_output, outFile);
        
        freeArray(to_output);
        freeRect(queryRect);

        free(BotLeft_x); // free memory alloc'd by strsep. points to entire line
        free(pointBotLeft);
        free(pointTopRight);

        fprintf(outStream, "\n");
        fprintf(outFile, "\n");
    }
	// fprintf(outStream, "\n"); // newline tests have to be the dumbest thing to be checked
	// fprintf(outFile, "\n");

	free(lineptr);
}

// Checks if a rectangle collides with another rectangle
int rect_overlap(rectangle_t *rect1, rectangle_t *rect2) {
    // needs equalities according to ed post #609
    if (
        rect1->botLeft->x <= rect2->topRight->x &&
        rect1->topRight->x >= rect2->botLeft->x &&
        rect1->topRight->y >= rect2->botLeft->y &&
        rect1->botLeft->y <= rect2->topRight->y
    ) return TRUE;
    return FALSE;
}

// Checks if a point lies within a rectangle
int point_in_rect(point_t *point, rectangle_t *rect) {
    if (
        point->y >= rect->botLeft->y &&
        point->y < rect->topRight->y &&
        point->x <= rect->topRight->x &&
        point->x > rect->botLeft->x
    ) return TRUE;
    return FALSE;
}

// Traverses quadtree and determines which child nodes the range query is overlapping.
// Adds all points inside the range query to array_t.
void traverse_query_stage4(treeNode_t *head, rectangle_t *query, FILE *outStream, FILE *outFile, array_t *to_output) {
    switch(treeNodeType(head)) {
        case WHITE:
            return;
        case GREY:
            for(int quadrant=0; quadrant<QUADRANTS; quadrant++) {
                // if child rects overlaps with query rect
                if (rect_overlap(query, head->nodes[quadrant]->rect)) {
                    int type = treeNodeType(head->nodes[quadrant]);
                    if (type==BLACK || type==GREY) {
                        output_direction(outStream, quadrant);
                    }
                    traverse_query_stage4(head->nodes[quadrant], query, outStream, outFile, to_output);
                }
            }
            break;
        case BLACK:
            // point must be in query rect.
            if (point_in_rect(head->point, query)) {
                for(int i=0; i<head->num; i++) {
                    // duplicate check for head->data_t
                    if (stage4_checkFlag(head->data[i])) {
                        arrayAppend(to_output, head->data[i]);
                    }
                }
            }
            break;
    }
}