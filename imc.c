#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//Indice de masa muscular
double fIMC(int a, float b) {
	return (a / (b * b));
}

//Distribucion por bloques
int main(int argc, char *argv[]) {
	int numIndices, myid, numprocs, i, j;
	char valor[100];

	double startwtime, endwtime; 	 //Tiempo de ejecucion
	int namelen;			 //Nombre del proceso
	int source, dest, tag = 100; 	 //Sends Receives
	char processor_name[MPI_MAX_PROCESSOR_NAME];	
		
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);
	
	startwtime = MPI_Wtime();

	fprintf(stdout, "\n\n\n***************\nProcess %d of %d on %s \n", myid, numprocs, processor_name);
	
	//Leemos el fichero 
	char *buf = NULL;
	FILE *file; size_t len = 0; ssize_t nread;
	int iteracion = 0;

	file = fopen("testIndices", "r");
	if(file) {
		getline(&buf, &len, file);
		char* aux = buf;
		float valores[(int)aux[0]][2];
			
		iteracion = 0;
		while((nread = getline(&buf, &len, file)) > 0) {
				
			//fwrite(buf, 1, nread, stdout);
			char *token = strtok(buf, " ");
							
			for(i = 0; i < 2; i++) {
				valores[iteracion][i] = atof(token);
				
				token = strtok(NULL, " ");
			}
			iteracion++;
	
			if(ferror(file)) {
				fprintf(stdout, "\nError con el fichero");
			}
		}
		
		//mostrando valores
		for(i = 0; i < iteracion; i++) { 
			for(j = 0; j < 2; j++) {
				printf("%.2f ", valores[i][j]);
			}
			printf("\n");	
		}

	fprintf(stdout, "Terminada la lectura del fichero %d \n", iteracion);

	int tamBloque = iteracion / numprocs;
	int restoBloque = -1;
	printf("iteracion  %d numprocs %d", iteracion, numprocs);
	if(iteracion % (numprocs - 1)== 0) {
		restoBloque = 0;
	} else { 
		restoBloque = iteracion % (numprocs - 1);
		fprintf(stdout, "Resto del bloque %d", restoBloque);
	}
	
	fprintf(stdout, "\n Tamano del bloque %d resto del bloque %d, myid %d ", tamBloque, restoBloque, myid);
	
	char* miValor;
	
	if(myid == 0) {
		for(i = 1; i < numprocs; i++) {
			source = i;

			MPI_Recv(&valor, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
			//size_t len = strlen(miValor);
			fprintf(stdout, "\nRecibido de %d  los valores %s", source, valor);

		}
		endwtime = MPI_Wtime();
		printf("\n wall clock time = %f\n", endwtime - startwtime);
		fprintf(stdout, "\n Hola desde 0 ");
			
	} else {
		dest = 0;
		for(i = ((myid - 1) *tamBloque); i < (tamBloque + ((myid - 1) * tamBloque)); i++) {
			printf("\nMostrando elemento %d proceso %d, :  %f", i, myid, fIMC(valores[i][0], valores[i][1]));
		}
                
		for(i = 1; i <= restoBloque; i++) {
			if(myid == i) {
				printf("\nMostrando elemento %d proceso %d, : %f", i + numprocs - 1, myid, fIMC(valores[i][0], valores[i][1])) - 1;
			}
		}
		strcpy(valor, "pepe");
		MPI_Send(&valor, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
	}
	 fclose(file);
	}else {
		printf("\nError");
	}
	MPI_Finalize();
	return 0;
}


