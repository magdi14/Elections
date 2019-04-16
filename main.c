#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void changeValues (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void shuffleRandon ( int arr1[], int n )
{
    srand ( time(0) );
    for (int i = n-1; i > 0; i--)
    {
        int j = rand() % (i+1);
        changeValues(&arr1[i], &arr1[j]);
    }
}
void CreateInputFile(int cand, int voters)
{
    FILE *filePointer;
    int i, t, j;
    int min = 1, max = cand;
    filePointer = fopen("Voters.txt", "w");
    if ( filePointer == NULL )
    {
        printf( "Voters file failed to open." );
    }
    else
    {
        printf("The file is now opened.\n");
        fprintf(filePointer, "%d \n", cand);
        fprintf(filePointer, "%d \n", voters);
        int arr[cand];
        for (i=0; i<cand; i++)
            arr[i] = i+1;
        for (i=0; i<cand; i++)
            printf("%d ", arr[i]);

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
int main()
{
    CreateInputFile(5,8);
    return 0;
}
