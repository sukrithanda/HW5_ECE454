/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
void*
parallel_game_of_life (void* data_packet)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
	Data *data = (Data *) data_packet;
	char* outboard = data->outboard; 
	char* inboard = data->inboard;
	const int nrows = data->nrows;
	const int ncols = data->ncols;
	const int gens_max = data->gens_max;
	const int start = data->start;
	const int end = data->end;

    const int LDA = nrows;
    int i, j, curgen;

	for (curgen = 0; curgen < gens_max; curgen++)
    {
        for (i = start; i < end; i++)
        {
        	const int inorth = mod (i-1, nrows);
            const int isouth = mod (i+1, nrows);

        	for (j = 0; j < ncols; j++)
    		{
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
	        }
	    }
	    pthread_barrier_wait(data->sync);
        SWAP_BOARDS( outboard, inboard );
    }
    pthread_exit(NULL);
}

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max)
{
	pthread_t thread_id[NTHREADS];
	pthread_barrier_t sync;
	Data *data = malloc(NTHREADS*sizeof(Data));

	int i;

	for (i = 0; i < NTHREADS; i++) {
		data[i].outboard = outboard; 
		data[i].inboard = inboard;
		data[i].nrows = nrows;
		data[i].ncols = ncols;
		data[i].gens_max = gens_max;
		data[i].start = i*nrows/NTHREADS;
		data[i].end = (i+1)*nrows/NTHREADS;
		data[i].sync = &sync;
	}

	pthread_barrier_init(&sync, NULL, NTHREADS);

	for (i = 0; i < NTHREADS; i++) {
		pthread_create(&thread_id[i], NULL, parallel_game_of_life, (void*) &data[i]);
	}

	for (i = 0; i < NTHREADS; i++) {
		pthread_join(thread_id[i], NULL);
	}

	return data[0].inboard;
}