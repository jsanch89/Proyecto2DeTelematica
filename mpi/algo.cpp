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
int __result__[MAX_WIDTH][MAX_HEIGHT];

int global_width = -1, global_height = -1;

using namespace std;


//Taken from https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file

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
  for(lint di = 0; di < MAX_SIZE_MB; ++di)
    for(lint dj = 0; dj < MAX_SIZE_MB; ++dj)
      cost += abs(frame1[i + di][j + dj] - frame2[dx + di][dy + dj]);
  return cost;
}



inline void compute_matrix(lint width1, lint height1, lint width2, lint height2){
  assert(width1 != -1 and height1 != -1 and width2 != -1 and height2 != -1);
  int min = (1 << 30 - 1);
  int min_x, min_y, min_dx, min_dy;
  min_x = min_y = min_dx = min_dy = 0;
  int i, j, dx, dy;
  for(i = 0; i < 1 + width1 - MAX_SIZE_MB; ++i){
    for(j = 0; j < 1 + height2 - MAX_SIZE_MB; ++j){
      for(dx = 0; dx < 1 + width2 - MAX_SIZE_MB; ++dx){
	for(dy = 0; dy < 1 + height2 - MAX_SIZE_MB; ++dy){
	  if( i != dx && j != dy){
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


void * precompute_matrix(unsigned char * _frame1, unsigned char * _frame2){
  int i, j;
  for(i = 0; i < global_width; ++i){
    for(j = 0; j < global_height; ++j){
      frame1[i][j] = (_frame1[(int)(i * global_width + j)]);
      frame2[i][j] = (_frame2[(int)(i * global_width + j)]);
   }
  }
}

char * filename1 = (char *) "./../data/w3c_home_gray.bmp";
char * filename2 = (char * ) "./../data/w3c_home_gray.bmp";
int main(int argc, char ** argv, char ** env){
  /*Just a MPI template.*/
  
  //Initialize the MPI enviroment
  MPI_Init(&argc, &argv);
  
  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
  // Get the ID(Rank) of processes
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if(world_rank == 1){
    
  }
  // Start the execution
  unsigned char * _frame1 = readBMP(filename1);
  unsigned char * _frame2 = readBMP(filename2);
  void * result = precompute_matrix(_frame1, _frame2);
  compute_matrix(global_width, global_height, global_width, global_height);
  // Now __result__(i, j) is the min(i+dx, j + dx), i=1,2,3,...; j = 1, 2, 3, ...; dx, dx in {16}
  return 0;
}
