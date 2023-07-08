#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dict.h"
#include "data.h"
#include "quadtree.h"

#define STAGE_1 1
#define STAGE_2 2
#define STAGE_3 3
#define STAGE_4 4

int processArgs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
	int dictType = processArgs(argc, argv);
	
	FILE *inFile = fopen(argv[2], "r");
	assert(inFile);

    FILE *outFile = fopen(argv[3], "w");
    assert(outFile);
    
	switch (dictType) {
		case STAGE_1: // stages 1 and 2 not working due to changes in data and dict. see assignment 1 to see it working
            // list_t *stage1_records = build_dict(inFile);
			// query_dict(stage1_records, stdin, outFile, stdout);
			// free_list(stage1_records);
            break;
		case STAGE_2:
            // list_t *stage2_records = build_dict(inFile);
            // node_t **sorted = construct_sorted(stage2_records);
            // query_index(sorted, stage2_records, stdin, outFile, stdout);
            // free(sorted); // valgrind and edstem doesnt detect not freeing this?
			// free_list(stage2_records);
            break;
        case STAGE_3:
            treeNode_t *stage3_quadtree = build_quadtree(argv, inFile);
            stage3_query(stage3_quadtree, stdin, outFile, stdout);
            freeQuadtree(stage3_quadtree);
            break;
        case STAGE_4:
            treeNode_t *stage4_quadtree = build_quadtree(argv, inFile);
            stage4_query(stage4_quadtree, stdin, outFile, stdout);
            freeQuadtree(stage4_quadtree);
            break;
		default:
            fprintf(stdout, "Entered non valid number.\n");
		break;
	}

    fclose(outFile);
    fclose(inFile);

	return 0;
}

// returns dictionary type
int processArgs(int argc, char *argv[]) {
	return atoi(argv[1]);
}