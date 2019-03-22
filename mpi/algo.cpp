#include <iostream>
#include <stdint.h>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <cassert>

#include <mpi.h>

typedef int sint;
typedef int lint;

const int MAX_SIZE_MB = (int) 16;
const int MAX_WIDTH = (int) 1000;
const int MAX_HEIGHT = (int) 1000;

char frame1[MAX_WIDTH][MAX_HEIGHT], frame2[MAX_WIDTH][MAX_HEIGHT]; 
int __result__[MAX_WIDTH - 16][MAX_HEIGHT - 16];

int global_width = -1, global_height = -1;

using namespace std;


//Taken from https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file

int world_size;
int world_rank;
int numworkers;
int offset, extra, rows, avg;
long long __curtime__;

unsigned char* readBMP(char* filename)
{
  int i;
  FILE* f = fopen(filename, "rb");
  unsigned char info[54];
  fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
  // extract image height and width from header
  int width = *(int*)&info[18];
  int height = *(int*)&info[22];
  if(((*(int *)&info[28])) != 8){
    cerr << "!!!! Please enter a grayscale image !!!" << endl;
    exit(EXIT_FAILURE);
  }
  global_width = width, global_height = height;
  int size = width * height;
  unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
  fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
  fclose(f);
  return data;
}

inline int calculate_cost(lint i, lint j, lint dx, lint dy){
  int cost = 0;
  for(lint di = 0; di < MAX_SIZE_MB; ++di){
    for(lint dj = 0; dj < MAX_SIZE_MB; ++dj){
      cost += abs(frame1[i + di][j + dj] - frame2[dx + di][dy + dj]);
    }
  }
  return cost;
}

MPI_Status status;

inline void compute_matrix(lint height, lint width, lint rows){
  if(world_rank > 0){
    int source = 0;
    MPI_Recv(&offset, 1, MPI::INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI::INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&frame1, rows * height, MPI::CHAR, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&frame2, width * height, MPI::CHAR, source, 1, MPI_COMM_WORLD, &status);
    assert(width != -1 and height != -1);
    int min = (1 << 30 - 1);
    int min_x, min_y, min_dx, min_dy;
    min_x = min_y = min_dx = min_dy = 0;
    int i, j, dx, dy;
    for(i = 0; i < rows; i += 16){
      for(j = 0; j < 1 + width - MAX_SIZE_MB - 16; j += 16){
	for(dx = 0; dx < 1 + height - MAX_SIZE_MB; ++dx){
	  for(dy = 0; dy < 1 + width - MAX_SIZE_MB; ++dy){
	    if( i != dx && j != dy){
	      min = (1 << 30 -1);
	      int cur_cost = calculate_cost(i, j, dx, dy);
	      if(cur_cost == 0){
		min = cur_cost;
		min_x = i, min_y = j;
		min_dx = dx, min_dy = dy;
		__result__[i][j] = 0;
		break;
	      }else{
		if(cur_cost < min){
		  min = cur_cost;
		}
		min_x = i, min_y = j, min_dx = dx, min_dy = dy;
		__result__[i][j] = min;
	      }
	    }
	  }
	}
      }
    }
  }
  MPI_Send(&offset, 1, MPI::INT, 0, 2, MPI_COMM_WORLD);
  MPI_Send(&rows, 1, MPI::INT, 0, 2, MPI_COMM_WORLD);
  MPI_Send(&__result__, rows * height, MPI::INT, 0, 2, MPI_COMM_WORLD);
}


char * filename1 = (char *) "./../data/lena_gray.bmp";
char * filename2 = (char *) "./../data/lena_gray.bmp";


int pre;

int main(int argc, char ** argv, char ** env){
  memset(__result__, -1, sizeof __result__);
  /*Just a MPI template.*/  
  //Initialize the MPI enviroment
  MPI_Init(&argc, &argv);
  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if(world_size < 2){
    puts("Need at least 2 mpi workers");
    exit(EXIT_FAILURE);
  }
  // Calculate number of workers
  numworkers = world_size - 1;
  
  // Get the ID(Rank) of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  unsigned char * _frame1;
  unsigned char * _frame2;
  int cur_height;
  int cur_width;
  
  if(world_rank == 0){
    // Start the execution
    _frame1 = readBMP(filename1); 
    _frame2 = readBMP(filename2);
    int i, j;
    for(i = 0; i < global_height; ++i){
      for(j = 0; j < global_width; ++j){
	frame1[i][j] = (_frame1[(int)(i * global_width + j)]);
	frame2[i][j] = (_frame2[(int)(i * global_width + j)]);
      }
    }
    avg = (global_height - 16) / numworkers;
    extra = (global_height - 16) % numworkers;
    offset = 0;
    cur_height = global_height;
    cur_width = global_width;
    printf("Height =%d Width = %d\n", cur_height, cur_width);
    for(pre = 1; pre <= numworkers; ++pre){
      //MPI_Send(data, count, type, dest, tag, curworld)
      rows = (pre <= extra) ? (avg + 1) : avg;
      MPI_Send(&cur_height, 1, MPI::INT, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&cur_width, 1, MPI::INT, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI::INT, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&offset, 1, MPI::INT, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI::INT, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&frame1[offset][0], rows * global_height, MPI::CHAR, pre, 1, MPI_COMM_WORLD);
      MPI_Send(&frame2, global_height * global_width, MPI::CHAR, pre, 1, MPI_COMM_WORLD);
      offset = offset + rows;
    }
    for(int spre = 1; spre <= numworkers; ++spre){
      int source = spre;
      MPI_Recv(&offset, 1, MPI::INT, source, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI::INT, source, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&__result__[offset][0], rows * global_height, MPI::INT, source, 2, MPI_COMM_WORLD, &status);
    }
    FILE * out_mpi = fopen("output_mpi.txt", "w");
    for(int pk = 0; pk < cur_height - 16; ++pk){
      for(int pl = 0; pl < cur_height - 16; ++pl){
	fprintf(out_mpi, "%d ", __result__[pk][pl]);
      }
    }
    
  }
  if(world_rank > 0){
    int source = 0;
    MPI_Recv(&cur_height, 1, MPI::INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&cur_width, 1, MPI::INT, source, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI::INT, source, 1, MPI_COMM_WORLD, &status);
    compute_matrix(cur_height, cur_width, rows);
  }
  MPI_Finalize();
  return 0;
}
