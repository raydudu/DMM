#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REAL_MEGABYTE (1024*1024)
#define MEGABYTE (1024*128-16)
#define NUM_OF_MALLOCS (440*16)

int main(int argc, char *argv[]) {
    void *myblock[NUM_OF_MALLOCS];
    void *bigarea;
    int count = 0;
    int i;

    memset(myblock, 0, sizeof(myblock));

    while (count < NUM_OF_MALLOCS) {
        myblock[count] = malloc(MEGABYTE);
        if (!myblock[count]) {
            fprintf(stderr, "Failed to allocate at %d\n", count + 1);
            break;
        }
        fprintf(stderr,"Currently allocating %d MB, address:%p\n", (count+1) * MEGABYTE / REAL_MEGABYTE, myblock[count]);
        memset(myblock[count],1, MEGABYTE);
        count++;
    }
    fprintf(stderr,"free through one block...\n");
    for(i = 0; i < count ; i+=2){
        free(myblock[i]);
        fprintf(stderr,"block #%d(%p) freed\n",i , myblock[i]);
    }

    fprintf(stderr,"try to alloc 1 MB...\n");
    bigarea = (void *) malloc(REAL_MEGABYTE);
    if(bigarea) {
        memset(bigarea, 1, MEGABYTE);
        free(bigarea);
    }
    fprintf(stderr,"result: %p\n", bigarea);
    fprintf(stderr,"try to alloc big area... %d MB\n", (count-1)/2  * MEGABYTE / REAL_MEGABYTE);
    bigarea = malloc((count-1)/2 *MEGABYTE);
    fprintf(stderr,"result: %p\n", bigarea);
    if(bigarea) {
        memset(bigarea,1, (count-1)/2 *MEGABYTE);
    }

    fprintf(stderr,"\nDone!..\n\n");
    exit(0);
}
