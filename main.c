#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void changeValues (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void shuffleRandon ( int array[], int n )
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}
void CreateInputFile(int cand, int voters)
{
    FILE *filePointer;
    int i, j;
    filePointer = fopen("Voters.txt", "w");
    if ( filePointer == NULL )
    {
        printf( "Voters file failed to open." );
    }
    else
    {
        //printf("The file is now opened.\n");
        fprintf(filePointer, "%d \n", cand);
        fprintf(filePointer, "%d \n", voters);
        int arr[cand];
        for (i=0; i<cand; i++)
            arr[i] = i+1;
        /*for (i=0; i<cand; i++)
            printf("%d ", arr[i]);
        */

        for (i=0; i<voters; i++)
        {
            shuffleRandon(arr, cand);
            for (j=0; j<cand; j++)
            {
                fprintf(filePointer, "%d ", arr[j]);
            }
            fprintf(filePointer, "\n");
        }
        fclose(filePointer);
    }
}
int main(int argc , char * argv[])
{

	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;
	int cand, voters;
    		/* tag for messages	*/
		/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(my_rank==0)
    {
        printf("How many candidates ?\n");
        scanf("%d", &cand);
        printf("Enter the number of Voters:\n");
        scanf("%d", &voters);
        CreateInputFile(cand,voters);
    }



    MPI_Finalize();
    return 0;
}
