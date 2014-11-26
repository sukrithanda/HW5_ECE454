#ifndef _life_h
#define _life_h

#include <stdlib.h>
#include <pthread.h>

#include "util.h"

#define NTHREADS 4

/**
 * Data packet passed to individual thread instance 
 */
typedef struct Data {
	char* outboard; 
	char* inboard;
	int nrows;
	int ncols;
	int gens_max;
	int start;
	int end;
	pthread_barrier_t *sync;
} Data;

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

/**
 * Given the initial board state in inboard and the board dimensions
 * nrows by ncols, evolve the board state gens_max times by alternating
 * ("ping-ponging") between outboard and inboard.  Return a pointer to 
 * the final board; that pointer will be equal either to outboard or to
 * inboard (but you don't know which).
 */
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max);

/**
 * Same output as game_of_life() above, except this is not
 * parallelized.  Useful for checking output.
 */
char*
sequential_game_of_life (char* outboard, 
			 char* inboard,
			 const int nrows,
			 const int ncols,
			 const int gens_max);

/**
 * Same output as game_of_life() above but it has
 * parallel and compiler optimizations.
 */
void*
parallel_game_of_life (void* data_packet);

#endif /* _life_h */
