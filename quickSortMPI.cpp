#include <iostream>
#include <mpi.h>
#include <climits>
using namespace std;

void inserisco(int*,int);
void riempiRandom(int*,int);
void quick(int*,int,int);

int main(int arg,char *arc[]){
  int* arrayIntero;
  int dimensioneArray, sottoSequenzaPerProcessore;
  int processore, processoriSize, pivot;
  double inizioTempo, fineTempo;
  int iteratoreRicorda = 0, iteratoreIndici = 0;
  int* indici;
  int* limiti;
  int* ricorda;
  int* array;
  
  MPI_Status status;
  MPI_Init(&arg,&arc);
  MPI_Comm_size(MPI_COMM_WORLD, &processoriSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &processore);
	
  if(processore == 0){
    cout << "Inserisci la dimenzione: ";
    cin >> dimensioneArray;
    if(dimensioneArray < 0)
      dimensioneArray = processoriSize;
    if((dimensioneArray % processoriSize) != 0)
      dimensioneArray = ((dimensioneArray/processoriSize)+1)*processoriSize;
    arrayIntero = new int[dimensioneArray];
    //inserisco(arrayIntero, dimensioneArray);
    riempiRandom(arrayIntero, dimensioneArray);
    inizioTempo = MPI_Wtime();
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&dimensioneArray, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  sottoSequenzaPerProcessore = dimensioneArray/processoriSize;
  array = new int[sottoSequenzaPerProcessore];

  MPI_Scatter(arrayIntero, sottoSequenzaPerProcessore, MPI_INT,
              array, sottoSequenzaPerProcessore, MPI_INT,
              0, MPI_COMM_WORLD);
  quick(array, 0, sottoSequenzaPerProcessore-1);
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Gather(array,sottoSequenzaPerProcessore,MPI_INT,
             arrayIntero,sottoSequenzaPerProcessore,MPI_INT,
             0,MPI_COMM_WORLD);
  if(processore == 0){
    indici = new int[processoriSize];
    limiti = new int[processoriSize];
    ricorda = new int[dimensioneArray];

    for(int i = 0; i < processoriSize; i++) 
      indici[i] = i*sottoSequenzaPerProcessore; 
    for(int i = 0; i < processoriSize-1; i++)
      limiti[i] = indici[i+1]; 
    limiti[processoriSize-1] = dimensioneArray; 
    while(iteratoreRicorda < dimensioneArray){
      int passa = INT_MAX;
      for(int i = 0; i < processoriSize; i++){
        if(arrayIntero[indici[i]] < passa && 
            indici[i] < limiti[i]){ 
          passa = arrayIntero[indici[i]];
          iteratoreIndici = i;
        }
      } 
      ricorda[iteratoreRicorda++] = passa; 
      indici[iteratoreIndici]++; 
    }
  } 
  MPI_Barrier(MPI_COMM_WORLD);
  if(processore == 0){
    fineTempo = MPI_Wtime();
    cout << "Tempo impiegato: " << fineTempo-inizioTempo << endl;
    delete[] arrayIntero;
    delete[] ricorda;
    delete[] indici;
    delete[] limiti;
  }
  delete[] array;

  MPI_Finalize();
  return 0;
}

void riempiRandom(int* arrayIntero,int dimensioneArray){
	for(int i=0; i<dimensioneArray; i++)
		arrayIntero[i]=rand()%dimensioneArray;
}

void inserisco(int* arrayIntero, int dimensioneArray){
	cout << "Inserisci " << dimensioneArray << " numeri." <<endl;
	for(int i=0; i<dimensioneArray; i++)
		cin >> arrayIntero[i];
	cout << "array riempito correttamente." << endl;
}

void quick(int* array, int sinistra, int destra){
  int i = sinistra;
  int j = destra;
  int temp;
  int pivot = array[(sinistra + destra) / 2]; 

  while (i <= j){
    while (array[i] < pivot) 
	   i++;
    while (array[j] > pivot)
	   j--;
    if (i <= j){
      temp = array[i];
      array[i] = array[j];
      array[j] = temp;
      i++;
      j--;
    }
  }

  if (sinistra < j){ quick(array, sinistra, j); }
  if (i < destra){ quick(array, i, destra); }
}
