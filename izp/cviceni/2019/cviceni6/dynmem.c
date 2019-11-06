#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* alloc_array(int size){
	int* tmp = malloc(size*sizeof(*tmp));
	return tmp;
}

void dealloc_array(int *array){
	free(array);
}

void fill_array(FILE *pFile, int *array, int size){
	for (int i = 0; i < size; i++} {
		fscanf(pFile, "%d ", array[i]);
	}
}

int main(){

	int* my_array;

	my_array = alloc_array(int size);

	return 0;
}
