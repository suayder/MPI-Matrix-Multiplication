#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define ROW 100
#define COL 100
#define FILENAME "array1.txt"

int main()
{
    srand(time(NULL));
    FILE *f = fopen(FILENAME, "w");

    if(f!=NULL){
        fprintf(f, "%d %d\n", ROW, COL);
        for(int i=0; i<ROW; i++){
            for(int j=0; j<COL; j++){
                fprintf(f, "%d ", ((rand()%2!=0)?ROW+rand()%ROW:ROW-rand()%ROW));
            }
            fprintf(f, "\n");
        }

        fclose(f);
    }

    return 0;
}
