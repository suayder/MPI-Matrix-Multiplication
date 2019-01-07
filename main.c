#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define RANK_MASTER 0
#define TAG_OPERATION 50

int **allocMatrix(int row, int col){

    int **m = (int**)malloc(row*sizeof(int*));
    for (int i = 0; i<row;i++)
        m[i] = (int*)malloc(col*sizeof(int));

    for(int i=0;i<row;i++){
        for (int j=0;j<col;j++){
            m[i][j] = 1;
        }
    }
    return m;
}

int **readMatrix(const char *filename, int* row, int* col){

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
    fclose(f)
    return m;
}

void print(int **m, int row, int col){
    for(int i=0;i<row;i++){
        for (int j=0;j<col;j++){
            printf("%d ", m[i][j]);
        }
        puts("");
    }
}

int *multiplyVectorByMatrix(int *v, int size, int **m, int row, int col){
    int *result = (int*)malloc(size*sizeof(int));
    for(int i = 0;i<size;i++)
        result[i] = 0;

    for(int i = 0; i<col;i++){
        for(int j=0;j<size;j++){
            result[j]+=(v[i])*(m[i][j]);
        }
    }
    return result;
}

int main(int argc, char const *argv[]){

    MPI_Init(NULL,NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    //---------Master Task------------
    if(world_rank == RANK_MASTER){
        int **matrix1, **matrix2, **result;
        int col1, row1;
        int row2, col2;

        matrix1 = readMatrix(argv[1], &row1,&col1);
        matrix2 = readMatrix(argv[2], &row2,&col2);
        result = allocMatrix(row1,col2);


        int aux = col1;
        col1=row1;
        row1=aux;
        aux = 0;
        //Send matrix
        int i = 1;
        while(aux<row1){
            MPI_Send(&row1, 1, MPI_INT, i,TAG_OPERATION, MPI_COMM_WORLD); //send size row matrix 1
            MPI_Send(&matrix1[aux][0], col1, MPI_INT, i,TAG_OPERATION, MPI_COMM_WORLD);//Manda linha a linha da matriz (iterador aux)
            MPI_Send(&row2, 1, MPI_INT, i,TAG_OPERATION, MPI_COMM_WORLD);//informações para mandar toda a segunda matrix linhas e colunas
            MPI_Send(&col2, 1, MPI_INT, i,TAG_OPERATION, MPI_COMM_WORLD);
            for(int j = 0; j<row2;j++)
                MPI_Send(&matrix2[j][0], col2, MPI_INT, i,TAG_OPERATION, MPI_COMM_WORLD);//Segunda matriz linha a linha
            
            aux++; i++;
            if(aux==world_size) i =1;
        }
        aux=0;
        i = 1;
        while(aux<row1){
            MPI_Recv(&result[aux][0], col2, MPI_INT, i, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            aux++; i++;
            if(aux==world_size) i =1;
        }
        printf("Resultado: \n");
        print(result,row1,col2);
    }
    //---------- Slave task-------------
    else{
        int sizevector;
        MPI_Recv(&sizevector, 1, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *vector = (int*) malloc(sizevector*sizeof(int));
        MPI_Recv(&vector[0], sizevector, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        
        int col, row, **matrix2;
        MPI_Recv(&row, 1, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&col, 1, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        matrix2 = allocMatrix(row,col);

        for(int i = 0; i<row;i++){
            MPI_Recv(&matrix2[i][0], col, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int *result = multiplyVectorByMatrix(vector, sizevector, matrix2, row, col);

        MPI_Send(&result[0], sizevector, MPI_INT, RANK_MASTER,TAG_OPERATION, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}
