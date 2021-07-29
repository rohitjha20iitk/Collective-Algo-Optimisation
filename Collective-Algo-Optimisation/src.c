#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"


double buf[2048 * 128];
double local_buf[2048 * 128];
double recvMessage[2048 * 128 * 128];
double localMessage[2048 * 128 * 128];
double sTime, eTime, time, maxTime;
double defTime1 = 0, optTime1 = 0, defTime2 = 0, optTime2 = 0, defTime3 = 0, optTime3 = 0, defTime4 = 0;
double local_sum = 0, sum = 0, totalsum = 0;


MPI_Group new_group[7];
MPI_Comm new_comm[7];



void method1(int count, int nodes_per_group, int cores, int no_of_groups, int rank, MPI_Group g_group){
    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
            buf[j] = rand();


        //default bcast
        sTime = MPI_Wtime();
        MPI_Bcast(buf, count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        eTime = MPI_Wtime();
        time = eTime - sTime;
        defTime1 += time;
    }

    MPI_Reduce (&defTime1, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(rank == 0)
        printf("DB = %lf\n", maxTime);


    //optimised version starts here
    int start = 0;
    int process_per_group = nodes_per_group * cores;
    int end = process_per_group - 1;
    int begin = 0;
    int ranks[process_per_group];
    int k = 0;
    int root = 0;
    
    
    
     sTime = MPI_Wtime();
        if(no_of_groups > 1){

            for( ; begin < no_of_groups; begin++){

                k = 0;

                for(int j = start; j <= end; j++)
                    ranks[k++] = j;

                MPI_Group_incl (g_group, process_per_group, ranks, &new_group[begin]);
                MPI_Comm_create_group (MPI_COMM_WORLD, new_group[begin], begin, &new_comm[begin]);
                start += process_per_group;
                end += process_per_group;

            }

            // Inter

            k = 0;
            start = 0;
            end = (no_of_groups - 1) * process_per_group;

            for(int j = start; j <= end; j += process_per_group)
                ranks[k++] = j;

            MPI_Group_incl (g_group, no_of_groups, ranks, &new_group[begin]);
            MPI_Comm_create_group (MPI_COMM_WORLD, new_group[begin], begin, &new_comm[begin]);
        }
        
    eTime = MPI_Wtime();
    time = eTime - sTime;
    optTime1 += time;
    

    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
                buf[j] = rand();


        //optimised bcast
        sTime = MPI_Wtime();
        if(no_of_groups == 4){

            // Inter
            if(rank == root || rank == (process_per_group) || rank == (2 * process_per_group) || rank == (3 * process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[4]);


            // Intra
            if(rank >= (3 * process_per_group) && rank < (4 * process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[3]);


            else if(rank >= (2 * process_per_group) && rank < (3 * process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[2]);


            else if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[0]);

        }



        else if(no_of_groups == 2){

            // Inter
            if(rank == root || rank == (process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[2]);


            // Intra
            if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Bcast(buf, count, MPI_DOUBLE, 0, new_comm[0]);

        }

        else if(no_of_groups == 1)
            MPI_Bcast(buf, count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        eTime = MPI_Wtime();
        time = eTime - sTime;
        optTime1 += time;


     }


        MPI_Reduce (&optTime1, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if(rank == 0)
            printf("OB = %lf\n", maxTime);

}



void method2(int count, int nodes_per_group, int cores, int no_of_groups, int rank, MPI_Group g_group, double time){
    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
            buf[j] = rank + j;

        local_sum = 0;
        for (int j = 0; j < count; j++)
            local_sum += buf[j];


        //default reduce
        sTime = MPI_Wtime();
        MPI_Reduce(&local_sum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        eTime = MPI_Wtime();
        time = eTime - sTime;
        defTime2 += time;

    }



    MPI_Reduce (&defTime2, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(rank == 0)
        printf("DR = %lf\n", maxTime);



    //optimised version starts here
    optTime2 += time;
    
    int root = 0;
    int process_per_group = nodes_per_group * cores;

    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
             buf[j] = rank + j;


        local_sum = 0;
        for (int j = 0; j < count; j++)
            local_sum += buf[j];


        // optimised reduce
        sTime = MPI_Wtime();
        if(no_of_groups == 4){

            // Inter
            if(rank == root || rank == (process_per_group) || rank == (2 * process_per_group) || rank == (3 * process_per_group))
                MPI_Reduce(&sum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[4]);


            // Intra
            if(rank >= (3 * process_per_group) && rank < (4 * process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[3]);


            else if(rank >= (2 * process_per_group) && rank < (3 * process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[2]);


            else if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[0]);

        }



        else if(no_of_groups == 2){

            // Inter
            if(rank == root || rank == (process_per_group))
                MPI_Reduce(&sum, &totalsum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[2]);


            // Intra
            if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Reduce (&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm[0]);

        }

        else if(no_of_groups == 1)
            MPI_Reduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        eTime = MPI_Wtime();
        time = eTime - sTime;
        optTime2 += time;

      }

        MPI_Reduce (&optTime2, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        if(rank == 0)
            printf("OR = %lf\n", maxTime);

}



void method3(int count, int nodes_per_group, int cores, int no_of_groups, int rank, MPI_Group g_group, double time){
    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
            buf[j] = rank + j;


        //default gather
        sTime = MPI_Wtime();
	    MPI_Gather(buf, count, MPI_DOUBLE, recvMessage, count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	    eTime = MPI_Wtime();
      	time = eTime - sTime;
      	defTime3 += time;

    }

    MPI_Reduce (&defTime3, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(rank == 0)
        printf("DG = %lf\n", maxTime);

    optTime3 += time;
    
    int root = 0;
    int process_per_group = nodes_per_group * cores;
    
    for(int iteration = 0; iteration < 5; iteration++){

        // initialize data
        for (int j = 0; j < count; j++)
                buf[j] = rank + j;


        // optimised gather
        sTime = MPI_Wtime();
        if(no_of_groups == 4){

            // Inter
            if(rank == root || rank == (process_per_group) || rank == (2 * process_per_group) || rank == (3 * process_per_group))
                MPI_Gather(localMessage, count * process_per_group, MPI_DOUBLE, recvMessage, count * process_per_group, MPI_DOUBLE, 0, new_comm[4]);


            // Intra
            if(rank >= (3 * process_per_group) && rank < (4 * process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, localMessage, count, MPI_DOUBLE, 0, new_comm[3]);


            else if(rank >= (2 * process_per_group) && rank < (3 * process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, localMessage, count, MPI_DOUBLE, 0, new_comm[2]);


            else if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, localMessage, count, MPI_DOUBLE, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, localMessage, count, MPI_DOUBLE, 0, new_comm[0]);

        }



        else if(no_of_groups == 2){

            // Inter
            if(rank == root || rank == (process_per_group))
                MPI_Gather(localMessage, count * process_per_group, MPI_DOUBLE, recvMessage, count * process_per_group, MPI_DOUBLE, 0, new_comm[2]);


            // Intra
            if(rank >= (process_per_group) && rank < (2 * process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, recvMessage, count, MPI_DOUBLE, 0, new_comm[1]);


            else if(rank >= root && rank < (process_per_group))
                MPI_Gather(buf, count, MPI_DOUBLE, recvMessage, count, MPI_DOUBLE, 0, new_comm[0]);

        }

        else if(no_of_groups == 1)
            MPI_Gather(buf, count, MPI_DOUBLE, recvMessage, count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        eTime = MPI_Wtime();
        time = eTime - sTime;
        optTime3 += time;

    }

    MPI_Reduce (&optTime3, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(rank == 0)
        printf("OG = %lf\n", maxTime);

}

void method4(int count, int nodes_per_group, int cores, int no_of_groups, int rank, MPI_Group g_group, int size){

    for(int iteration = 0; iteration < 5; iteration++){

        // initialize the data
        for(int i = 0; i < count * size; i++) {
            localMessage[i] = i ;
            recvMessage[i] = -i;
        }

        int *sendcounts = (int *)malloc( size * sizeof(int) );
        int *recvcounts = (int *)malloc( size * sizeof(int) );
        int *rdispls = (int *)malloc( size * sizeof(int) );
        int *sdispls = (int *)malloc( size * sizeof(int) );

        for(int i = 0; i < size; i++){
            sendcounts[i] = count;
            recvcounts[i] = count;
            rdispls[i] = i * count;
            sdispls[i] = i * count;
        }

        //default alltoallv
        sTime = MPI_Wtime();
        MPI_Alltoallv(localMessage, sendcounts, sdispls,  MPI_DOUBLE, recvMessage, recvcounts, rdispls, MPI_DOUBLE, MPI_COMM_WORLD);
        eTime = MPI_Wtime();
        time = eTime - sTime;
        defTime4 += time;

    }

    MPI_Reduce (&defTime4, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(rank == 0)
        printf("DA = %lf\n", maxTime);
}


int main(int argc, char *argv[]){

    int size, rank;
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);


    // get the world group
    MPI_Group g_group;
    MPI_Comm_group (MPI_COMM_WORLD, &g_group);


    int no_of_groups = atoi(argv[1]);
    int nodes_per_group = atoi(argv[2]);
    int cores = atoi(argv[3]);
    int count = (atoi(argv[4]) * 1024) / 8;
    
    
    
    int start = 0;
    int process_per_group = nodes_per_group * cores;
    int end = process_per_group - 1;
    int begin = 0;
    int ranks[process_per_group];
    int k = 0;
    
    
    
     sTime = MPI_Wtime();
        if(no_of_groups > 1){

            for( ; begin < no_of_groups; begin++){

                k = 0;

                for(int j = start; j <= end; j++)
                    ranks[k++] = j;

                MPI_Group_incl (g_group, process_per_group, ranks, &new_group[begin]);
                MPI_Comm_create_group (MPI_COMM_WORLD, new_group[begin], begin, &new_comm[begin]);
                start += process_per_group;
                end += process_per_group;

            }

            // Inter

            k = 0;
            start = 0;
            end = (no_of_groups - 1) * process_per_group;

            for(int j = start; j <= end; j += process_per_group)
                ranks[k++] = j;

            MPI_Group_incl (g_group, no_of_groups, ranks, &new_group[begin]);
            MPI_Comm_create_group (MPI_COMM_WORLD, new_group[begin], begin, &new_comm[begin]);
        }
        
    eTime = MPI_Wtime();
    time = eTime - sTime;


    method1(count, nodes_per_group, cores, no_of_groups, rank, g_group);

    method2(count, nodes_per_group, cores, no_of_groups, rank, g_group, time);

    method3(count, nodes_per_group, cores, no_of_groups, rank, g_group, time);

    //method4(count, nodes_per_group, cores, no_of_groups, rank, g_group, size);
    MPI_Finalize();
    return 0;
}
