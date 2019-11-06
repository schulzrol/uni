float **alloc_sparse_matrix(int rows, int cols)
{
    float **matrix;
    matrix = malloc(rows * sizeof(float*)); // vysvětlete velikost
    //matrix = malloc(rows * sizeof(*matrix)); // vysvětlete velikost
    if (matrix == NULL)
        return NULL;
    int i;
    for (i = 0; i < rows; i++)
    {
        matrix[i] = malloc(cols * sizeof(float)); // vysvětlete velikost
	//matrix[i] = malloc(cols * sizeof(*matrix[i])); // vysvětlete velikost
        if (matrix[i] == NULL)
        {
            for (; i >= 0; i--)
                free(matrix[i]); // vysvětlete důvod
            free(matrix); // vysvětlete důvod
            return NULL;
        }
    }
    return matrix;
    // vysvětlete návratový kód
}

// ???
void free_sparse_matrix(float **matrix)
{
    // doplňte kód uvolnění matice z paměti
}
 
void foo()
{
    float **mat;
    mat = alloc_sparse_matrix(4000, 3000);
    if (mat == NULL)
        return; // co se stalo?
    mat[0][3] = 42;
    free_sparse_matrix(mat);
}