//Ler matrizes
#include <stdio.h>
#include <stdlib.h>


void print(int **m, int row, int col){
    printf("(%d, %d)\n",row, col);
    for(int i=0;i<row;i++){
        for (int j=0;j<col;j++){
            printf("%d ", m[i][j]);
        }
        puts("");
    }
}

int **getMatrix(const char *filename, int* row, int* col){

    FILE *f;
    f = fopen(filename, "r");

    if(f==NULL)
        exit (1);

    fscanf(f, "%d %d", row, col);
    printf("(%d, %d)\n", *row, *col);

    int **m = (int**)malloc((*row)*sizeof(int*));
    for (int i = 0; i<*row;i++)
        m[i] = (int*)malloc((*col)*sizeof(int));

    for(int i=0; i<*row; i++){
        for (int j=0;j<*col;j++){
            fscanf(f, "%d ", m[i]+j);
        }
    }
    return m;
}

int main(int argc, char const *argv[])
{
    int row, col;
    int **matrix = getMatrix(argv[1], &row, &col);
    print(matrix, row, col);

    return 0;
}
