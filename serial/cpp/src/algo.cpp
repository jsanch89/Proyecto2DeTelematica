#include <iostream>
#include <stdint.h>
#include <math.h>



typedef int8_t sint;
typedef int16_t lint;

const int MAX_SIZE_MB = (lint) 16;
const int MAX_WIDTH = (lint) 1000;
const int MAX_HEIGHT = (lint) 1000;

char frame1[MAX_WIDTH][MAX_HEIGHT], frame2[MAX_WIDTH][MAX_HEIGHT]; 
int __result__[MAX_WIDTH][MAX_HEIGHT];

int global_width = -1, global_height = -1;

using namespace std;



//Taken from https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file

inline unsigned char * readBMP(char* filename){
  int i;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) perror ("Error opening data"), exit(1);
  unsigned char info[54];
  fread(info, sizeof(unsigned char), 54, f);
  int width = *(int*) &info[18];
  int height = *(int*) &info[22];
  int bpp = *(int*) &info[28];
  //cerr << "BPP = " << bpp << endl;
  if (bpp != 8){
    puts("You have to enter a gray scale image");
    exit(1);
  }
  global_width = width, global_height = height;
  int size = 3 * width * height;
  unsigned char* data = new unsigned char[size];
  fread(data, sizeof(unsigned char), size, f);
  //cerr << data << endl;
  fclose(f);
  for(i = 0; i < size; i += 3){
    unsigned char tmp = data[i];
    data[i] = data[i+2];
    data[i+2] = tmp;
  }
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
  int min = (1 << 30 - 1);
  lint min_x, min_y, min_dx, min_dy;
  min_x = min_y = min_dx = min_dy = 0;
  lint i, j, dx, dy;
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
	      return;
	    }else{
	      if(cur_cost < min){
		min = cur_cost;
	      }
	      min_x = i, min_y = j, min_dx = dx, min_dy = dy;
	      __result__[i][j] = min;
	    }
	  }
	}
	//cerr << __result__[i][j] << endl;
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
  //cerr << "Here" << endl;
}

char * filename1 = "data/lena_gray.bmp";
char * filename2 = "data/lena_gray.bmp";
int main(int argc, char ** argv, char ** env){
  //printf("%s\n", filename1);
  unsigned char * _frame1 = readBMP(filename1);
  unsigned char * _frame2 = readBMP(filename2);
  for (int i = 0; i < sizeof(_frame1)/sizeof(_frame1[0]); ++i){
    cerr << _frame1[i] << " " << _frame2[i] << endl;
  }
  void * result = precompute_matrix(_frame1, _frame2);
  compute_matrix(global_width, global_height, global_width, global_height);
  // Now __result__(i, j) is the min(i+dx, j + dx), i=1,2,3,...; j = 1, 2, 3, ...; dx, dx in {16}
  return 0;
}
