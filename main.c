#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int * checkSecRound(int arr[], int sze)
{
    int i, pos1, pos2, largest1, largest2;
    largest1 = largest2 = -1;
    int *res = (int*)malloc(4*sizeof(int));
    for(i=0;i<4; i++)
        res[i] = -1;
    for (i=0; i<sze; i++)
    {
        if(arr[i]>=largest1)
        {
            largest2 = largest1;
            pos2 = pos1;
            largest1 = arr[i];
            pos1 = i;
        }
    }
    res[0] = largest1;
    res[1] = pos1+1;
    if(largest1 == largest2)
    {
        res[2] = largest2;
        res[3] = pos2+1;
    }
    return res;
}
int** MatAlloc(int rows, int cols) //allocate the array and takes the value from user
{
    int* linear = (int*)malloc(rows * cols * sizeof(int));
    int** mat = (int**)malloc(rows*sizeof(int*));
    int i, j;
    for(i=0; i<rows; i++)
    {
        mat[i] = &linear[i*cols];
    }
    for(j = 0 ; j < cols*rows ; j++)
    {
        scanf("%d", &linear[j]);
    }
    return mat;
}
int** MatAllocRaw(int rows, int cols)   //just allocate the array
{
    int *Linear = (int*)malloc(rows* cols * sizeof(int));
    int** mat = (int**)malloc(rows*sizeof(int*));
    int i;
    for(i = 0 ; i < rows ; i++)
    {
        mat[i] = &(Linear[i*cols]);
    }
    return mat;
}
void print2DMat(int** Mat,int n, int m)
{
    int i, j;
    for(i = 0 ; i < n ; i++)
    {
        for(j = 0 ; j < m ; j++)
        {
            printf("%d ",Mat[i][j]);
        }
        printf("\n");
    }
}
void printArray(int* arr, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        printf("%d ", arr[i]);
    }
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
void CreateInputFile(cand, voters)
{
    int i, j;
    FILE *filePointer;
    filePointer = fopen("Voters.txt", "w");
    if ( filePointer == NULL )
    {
        printf( "Voters file failed to open." );
    }
    else
    {
        //printf("The file is now opened.\n");
        fprintf(filePointer, "%d\n", cand);
        fprintf(filePointer, "%d\n", voters);
        int arr[cand];
        for (i=0; i<cand; i++)
            arr[i] = i+1;

        for (i=0; i<voters; i++)
        {
            shuffleRandon(arr, cand);
            for (j=0; j<cand; j++)
            {
                fprintf(filePointer, "%d", arr[j]);
            }
            fprintf(filePointer, "\n");
        }
        fclose(filePointer);
    }
}
int** GetVotersToSecondRound(int** allCand, int n, int m, int fCand, int seCand)
{
    int** SecRoundVoters = MatAllocRaw(n, 2);
    int i, j;
    int k, l;
    k=l=0;
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            if(allCand[i][j] == fCand)
            {
                SecRoundVoters[k][l] = allCand[i][j];
                l++;
                if(l>=2)
                {
                    k++;
                    l=0;
                }
            }
            else if(allCand[i][j] == seCand)
            {
                SecRoundVoters[k][l] = allCand[i][j];
                l++;
                if(l>=2)
                {
                    k++;
                    l=0;
                }
            }
            else
                continue;
        }
    }
    return SecRoundVoters;

}
int main(int argc , char * argv[])
{

	int my_rank;		/* rank of process	*/
	int p, i, j, k, move, position;
	int source;		/* rank of sender	*/
	int dest;		/* rank of receiver	*/
	int tag = 0;
	int cand, voters, portion, masterPortion;
	int* LocalCountVoters;
	int* CountVoters;
	int SecRound = 0;
	float percent = 0.0;
	MPI_Status status;

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
        CountVoters = (int*)malloc(cand * sizeof(int));
        for (i=0; i<cand; i++)
            CountVoters[i] = 0;
        CreateInputFile(cand, voters);
        FILE *filePointer;
        filePointer = fopen("Voters.txt", "r");
        int** localVoters;
        if (filePointer == NULL)
        {
            printf( "Voters file failed to open!" );
        }
        fseek(filePointer, 4, SEEK_SET);
        portion = voters/p;
        //printf("%d\n", portion);
        masterPortion = portion;
        if (voters%p!=0)
        {
            //printf("reminder %d", voters%p);
            masterPortion += (voters%p);
            int masterMove = 0;
            //printf("%d", masterPortion);
            //printf("Hello here\n");
            localVoters = MatAllocRaw(masterPortion, cand);
            for(i=0; i<masterPortion; i++)
            {
                for(j=0; j<cand; j++)
                {
                    fscanf(filePointer, "%1d", &localVoters[i][j]);
                }
            }
            //printf("position %d in process %d \n", 4, my_rank);   //Master process starting Position if the #of processes not divisable by voters
            //print2DMat(localVoters, masterPortion, cand);           //Votes in master process if the #of processes not divisable by voters
            masterMove = (cand+1)*masterPortion;
            position = 4 + masterMove;
            move = (cand+1)*portion;
            MPI_Send(&portion, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            MPI_Send(&cand, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            MPI_Send(&position, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            for (i=2; i<p; i++)
            {
                MPI_Send(&portion, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                MPI_Send(&cand, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                position+=move;
                MPI_Send(&position, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            }

        }
        else
        {
            //printf("Hello here mn gwa\n");
            localVoters = MatAllocRaw(masterPortion, cand);
            for(i=0; i<masterPortion; i++)
            {
                for(j=0; j<cand; j++)
                {
                    fscanf(filePointer, "%1d", &localVoters[i][j]);
                }
            }
            //printf("position %d in process %d \n", 4, my_rank);   //starting position when voters and processes are divisable
           // print2DMat(localVoters, masterPortion, cand);           //Voter in master process when voters and processes are divisable
            move = (cand+1)*masterPortion;
            position = 4 + move;
            for (i=1; i<p; i++)
            {
                MPI_Send(&portion, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                MPI_Send(&cand, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                MPI_Send(&position, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                position+=move;
            }
        }

        int firstPhase;
        for (i=0; i<masterPortion; i++)
        {
            firstPhase = localVoters[i][0];
            CountVoters[firstPhase-1]++;
        }
        printf("Votes from process %d\n", my_rank);
        printArray(CountVoters, cand);
        printf("\n");

        int* temp = (int*)malloc(cand * sizeof(int));
        for(i=1; i<p; i++)
        {
            MPI_Recv(&temp[0], cand, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            for(j=0;j<cand; j++)
                CountVoters[j]+=temp[j];
        }
        printf("All Candidates:\n");
        printArray(CountVoters, cand);      //All candidates after collecting every count from every process
        printf("\n");
        for (i=0; i<cand; i++)
            {
                percent = ((float)CountVoters[i]) / ((float)voters);
                printf("Candidate[%d] got %d/%d which is %.1f%\n", i+1, CountVoters[i], voters,(percent*100.0));
            }
            int *res = (int*)malloc(4*sizeof(int));
            res = checkSecRound(CountVoters, cand);     //check if there is a round two
            //printArray(res, 4);     //print dictionary with the first two candidates got votes with number of votes and index of candidate
            printf("\n");
            printf("Press any key to continue ..\n");
            getchar();
            if(res[0] == res[2] && res[1] != res[3])
            {
                SecRound = 1;
                int r2Cand1, r2cand2;
                r2Cand1 = res[1];
                r2cand2 = res[3];
                printf("There's a second round between the %dth and %dth candidate\n", res[1], res[3]);
                int** SecRoundVoters = GetVotersToSecondRound(localVoters, masterPortion, cand, res[1], res[3]);
                //print2DMat(SecRoundVoters, masterPortion, 2);   //portion of second round voters at process zero
                printf("\n");
                for(i=1; i<p; i++)
                {
                    MPI_Send(&SecRound, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                    MPI_Send(&r2Cand1, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                    MPI_Send(&r2cand2, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
                }
                int LocalSecondRound;
                int* LocalCountVotersRound2 = (int*)malloc(2 * sizeof(int));
                for(i=0; i<2; i++)
                    LocalCountVotersRound2[i] = 0;
                for (i=0; i<masterPortion; i++)
                {

                    LocalSecondRound = SecRoundVoters[i][0];
                    if(LocalSecondRound == r2Cand1)
                        LocalCountVotersRound2[0]++;
                    else if(LocalSecondRound == r2cand2)
                        LocalCountVotersRound2[1]++;
                }
               // printf("Votes from process %d in round 2\n", my_rank);
               // printArray(LocalCountVotersRound2, 2);    //counter of votes in second round at process zero
                printf("\n");
                int* tempRound2 = (int*)malloc(2 * sizeof(int));
                for(i=1; i<p; i++)
                {
                    MPI_Recv(&tempRound2[0], 2, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
                    for(j=0;j<2; j++)
                        LocalCountVotersRound2[j]+=tempRound2[j];
                }
                printf("All Candidates votes in round 2:\n");
                printArray(LocalCountVotersRound2, 2);         //All candidates after collecting every count from every process in round 2
                printf("\n");
                for (i=0; i<2; i++)
                {
                    percent = ((float)LocalCountVotersRound2[i]) / ((float)voters);
                    printf("Candidate[%d] got %d/%d which is %.1f%\n", i+1, LocalCountVotersRound2[i], voters,(percent*100.0));
                }
                int *res = (int*)malloc(4*sizeof(int));
                res = checkSecRound(LocalCountVotersRound2, 2);
                //printArray(res, 4);     //print dictionary with the first two candidates got votes with number of votes and index of candidate
                printf("\n");
                if(LocalCountVotersRound2[0] > LocalCountVotersRound2[1])
                {
                    printf("The %dth candidate wins in second round\n", r2Cand1);
                    printf("With number of votes equal %d out of %d\n", res[0], voters);

                }
                else
                {
                    printf("The %dth candidate wins in second round\n", r2cand2);
                    printf("With number of votes equal %d out of %d\n", res[0], voters);
                }

                MPI_Finalize();
                return 0;
            }
            else
            {
                printf("The %dth candidate wins in first round\n", res[1]);
                printf("With number of votes equal %d out of %d\n", res[0], voters);
                MPI_Finalize();
                return 0;
            }
    }
    else
    {
        FILE *filePointer;
        int position;
        filePointer = fopen("Voters.txt", "r");
        if ( filePointer == NULL )
        {
            printf( "Voters file failed to open!" );
        }
        else
        {
            MPI_Recv(&portion, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&cand, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&position, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            //printf("position %d in process %d \n", position, my_rank);
            int** localVoters = MatAllocRaw(portion, cand);
            LocalCountVoters = (int*)malloc(cand * sizeof(int));
            for (i=0; i<cand; i++)
                LocalCountVoters[i] = 0;
            //printf("Hello from process %d has %d %d\n", my_rank, cand, portion);
            fseek( filePointer, position, SEEK_SET );
            printf("position %d in process %d \n", position, my_rank);
            for(i=0; i<portion; i++)
            {
                for(j=0; j<cand; j++)
                {
                    fscanf(filePointer, "%1d", &localVoters[i][j]);
                }
            }
            //printf("Hello from process %d has\n", my_rank);
            print2DMat(localVoters, portion, cand);
            int LocalfirstRound;
            for (i=0; i<portion; i++)
            {

                LocalfirstRound = localVoters[i][0];
                LocalCountVoters[LocalfirstRound-1]++;
            }
            printf("Votes from process %d\n", my_rank);
            printArray(LocalCountVoters, cand);
            printf("\n");
            MPI_Send(&LocalCountVoters[0], cand, MPI_INT, 0, tag, MPI_COMM_WORLD);
            MPI_Recv(&SecRound, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            if(SecRound==1)
            {
                int r2Cand1, r2Cand2;
                MPI_Recv(&r2Cand1, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
                MPI_Recv(&r2Cand2, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
                int** SecndRoundVoters = GetVotersToSecondRound(localVoters, portion, cand, r2Cand1, r2Cand2);
                //printf("From Process Second Round validation: %d\n", SecRound);
                printf("From Process %d Second Round Voters are : \n", my_rank);
                print2DMat(SecndRoundVoters, portion, 2);
                printf("\n");
                int LocalSecondRound;
                int* LocalCountVotersRound2 = (int*)malloc(2 * sizeof(int));
                for(i=0; i<2; i++)LocalCountVotersRound2[i] = 0;
                for (i=0; i<portion; i++)
                {

                    LocalSecondRound = SecndRoundVoters[i][0];
                    if(LocalSecondRound == r2Cand1)
                        LocalCountVotersRound2[0]++;
                    else if(LocalSecondRound == r2Cand2)
                        LocalCountVotersRound2[1]++;
                }
                printf("Votes from process %d in round 2\n", my_rank);
                printArray(LocalCountVotersRound2, 2);
                printf("\n");
                MPI_Send(&LocalCountVotersRound2[0], 2, MPI_INT, 0, tag, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Finalize();
                return 0;
            }
        }

    }

    MPI_Finalize();
    return 0;
}
