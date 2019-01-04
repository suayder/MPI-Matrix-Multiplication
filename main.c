//thiagombrodrigues@uft.edu.br
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define RANK_MASTER 0
#define TAG_OPERATION 50

int **getMatrix(int row, int col){

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

int main(){

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
        int col1 = 3, row1 = 3;
        int row2 = 3, col2 = 3;

        matrix1 = getMatrix(row1,col1);
        matrix2 = getMatrix(row2,col2);
        result = getMatrix(row1,col2);

        matrix1[0][0] = 1;
        matrix1[0][1] = 2;
        matrix1[0][2] = 5;
        matrix1[1][0] = 3;
        matrix1[1][1] = 4;
        matrix1[1][2] = 3;
        matrix1[2][0] = 8;
        matrix1[2][1] = 4;
        matrix1[2][2] = 5;

        matrix2[0][0] = 1;
        matrix2[0][1] = 3;
        matrix2[0][2] = 5;
        matrix2[1][0] = 2;
        matrix2[1][1] = 4;
        matrix2[1][2] = 8;
        matrix2[2][0] = 7;
        matrix2[2][1] = 9;
        matrix2[2][2] = 0;


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

        matrix2 = getMatrix(row,col);

        for(int i = 0; i<row;i++){
            MPI_Recv(&matrix2[i][0], col, MPI_INT, RANK_MASTER, TAG_OPERATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int *result = multiplyVectorByMatrix(vector, sizevector, matrix2, row, col);

        MPI_Send(&result[0], sizevector, MPI_INT, RANK_MASTER,TAG_OPERATION, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}