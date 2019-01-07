EXECS=main
MPICC?=mpicc

all: ${EXECS}

main: main.c
	${MPICC} -o main main.c

run:
	mpirun --hostfile mpi_hosts -np 4 main $(arg1) $(arg2)

clean:
	rm ${EXECS}