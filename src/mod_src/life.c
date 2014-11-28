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
    /* HINT: in the parallel decomposition, nrows may not be equal to
       nrows! */
	Data *data = (Data *) data_packet;
	char* outboard = data->outboard; 
	char* inboard = data->inboard;
	const int nrows = data->nrows;
	const int ncols = data->ncols;
	const int gens_max = data->gens_max;
	int start = data->start;
    int end = data->end;

    //const int nrows = nrows;
    int i, j, curgen;

    int north_neighbor;
    int south_neighbor;
    int west_neighbor;
    int east_neighbor;

	for (curgen = 0; curgen < gens_max; curgen++)
    {

             //go through first column
            j=start;
           
            int col_mask = nrows * j;
            //if (start != 0){ 
            for(i = 0; i < nrows ; i++)
            {
				
				if (i == 0)
					north_neighbor = nrows - 1;
				else	
					north_neighbor = i - 1;

				if (i == nrows - 1 )
					south_neighbor = 0;
				else 
					south_neighbor = i + 1;

				if (j == 0) 
					west_neighbor = (ncols - 1) * nrows;
				else
					west_neighbor = col_mask - nrows;
				
				//if (j == ncols -1 ) 
				//	east_neighbor = 0;
				//else
					east_neighbor = col_mask + nrows; 

                const char neighbor_count =
                    //NORTH WEST
                    data->inboard[north_neighbor + west_neighbor] +
                    //NORTH
                    data->inboard[north_neighbor + col_mask] +
                    //NOTH EAST
                    data->inboard[north_neighbor + east_neighbor] +
                    //WEST
                    data->inboard[i + west_neighbor] +
                    //EAST
                    data->inboard[i + east_neighbor] +
                    //SOUTH WEST
                    data->inboard[south_neighbor + west_neighbor] +
                    //SOUTH
                    data->inboard[south_neighbor + col_mask] +
                    //SOUTH EAST
                    data->inboard[south_neighbor + east_neighbor];

                data->outboard[i + col_mask] = alivep (neighbor_count,  data->inboard[i + col_mask]);
            }
        //}
        //go through the middle columns
        for (j=j+1; j < (end-1); j++)
        {
            //unique case first pixel in column
                i=0;
                int col_mask = nrows * j;
                const char neighbor_count =
                	data->inboard[(nrows - 1) + (col_mask - nrows)] +
                	data->inboard[(nrows - 1) + col_mask] +
                	data->inboard[(nrows - 1) + (col_mask + nrows)] +
                	data->inboard[i + (col_mask - nrows)] +
                	data->inboard[i + (col_mask + nrows)] +
                	data->inboard[(i + 1) + (col_mask - nrows)] +
                	data->inboard[(i + 1) + col_mask] +
               		data->inboard[(i + 1) + (col_mask + nrows)];

                data->outboard[i+col_mask] = alivep (neighbor_count,  data->inboard[ i+ col_mask]);
            
            for (i = 1; i < (data->nrows-1) ; i++)
            {               
                  //  int jrow = nrows * j;
                    const char neighbor_count =
                    data->inboard[(i-1)+ (col_mask - nrows)] +
                    data->inboard[(i-1) + col_mask] +
                    data->inboard[(i-1) + (col_mask + nrows)] +
                    data->inboard[i + (col_mask - nrows)] +
                    data->inboard[i + (col_mask + nrows)] +
                    data->inboard[(i+1)+ (col_mask - nrows)] +
                    data->inboard[ (i+1)+ col_mask] +
                    data->inboard[ (i+1)+ (col_mask + nrows)];

               data->outboard[i+col_mask] = alivep (neighbor_count, data->inboard[i+ col_mask]);
            }
            //unique case last pixel in column
            {
                    i=nrows-1;            
                    const char neighbor_count =
                    data->inboard[(i-1)+ (col_mask - nrows)] +
                    data->inboard[(i-1)+col_mask] +
                    data->inboard[(i-1)+ (col_mask + nrows)] +
                    data->inboard[i+ (col_mask - nrows)] +
                    data->inboard[i+(col_mask + nrows)] +
                    data->inboard[(col_mask - nrows)] +
                    data->inboard[col_mask] +
                    data->inboard[(col_mask + nrows)];

                data->outboard[i+col_mask] = alivep (neighbor_count,  data->inboard[ i+ col_mask]);
            }
        }
        
        
            j=end-1;
            col_mask = nrows * j;

            for(i = 0; i < data->nrows ; i++)
            {
             
                if (i == 0)
					north_neighbor = nrows - 1;
				else	
					north_neighbor = i - 1;

				if (i == nrows - 1 )
					south_neighbor = 0;
				else 
					south_neighbor = i + 1;

				//if (j == 0) 
				//	west_neighbor = (ncols - 1) * nrows;
				//else
					west_neighbor = col_mask - nrows;
				
				if (j == ncols -1 ) 
					east_neighbor = 0;
				else
					east_neighbor = col_mask + nrows; 

                const char neighbor_count =
                    data->inboard[north_neighbor+ west_neighbor] +
                    data->inboard[north_neighbor+col_mask] +
                    data->inboard[north_neighbor+ east_neighbor] +
                    data->inboard[i+ west_neighbor] +
                    data->inboard[i+east_neighbor] +
                    data->inboard[south_neighbor+ west_neighbor] +
                    data->inboard[south_neighbor+col_mask] +
                    data->inboard[south_neighbor+ east_neighbor];

                data->outboard[i+col_mask] = alivep (neighbor_count,  data->inboard[ i+ col_mask]);
            }
              
	 
   
 			 char* temp = data->outboard; 
  			 data->outboard = data->inboard; 
 			 data->inboard = temp; 
				
        pthread_barrier_wait(data->sync);

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
		data[i].start = (i*nrows) >> 2; //NTHREADS;
	
		data[i].end = ((i+1)*nrows) >> 2 ;// /NTHREADS;
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
