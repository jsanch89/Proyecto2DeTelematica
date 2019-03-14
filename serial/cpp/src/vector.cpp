#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

const int MAX_SIZE_MB = 16;

int width,height,size,width1,height1,size1,width2,height2,size2;
unsigned char  *frame1, *frame2;

unsigned char* readBMP(const char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    width  = *(int*)&info[18];
    height = *(int*)&info[22];
    int sizePoint = *(int*)&info[28];
    if(sizePoint != 8){
        cout<<"No se puede procesar esta imagen, debe ser una imagen de 256 colores \n";
        return 0;
    }

    size = width * height;
    unsigned char* data = new unsigned char[size];
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);
    return data;
}

int calculate_cost(int i, int j, int dx, int dy){
    int cost = 0;
    for(int di = 0; di < MAX_SIZE_MB; ++di){
        for(int dj = 0; dj < MAX_SIZE_MB; ++dj){
            cost += fabs(frame1[i+di + j+dj*width1] - frame2[dx+di + dy+dj*width2]);
            cout<<i+di+j+dj*height1<<"\n";
         }
    }
     return cost;
}

int** compute_matrix(int width1, int height1, int width2, int height2)
{   int** result = new int*[width1];
    int min = (1 << 30 - 1);
    int min_x, min_y, min_dx, min_dy;
    min_x = min_y = min_dx = min_dy = 0;
    int i, j, dx, dy;
    for (i = 0; i < 1 + width1 - MAX_SIZE_MB; ++i)
    {
        result[i] = new int[height1];
        for (j = 0; j < 1 + height2 - MAX_SIZE_MB; ++j)
        {
            for (dx = 0; dx < 1 + width2 - MAX_SIZE_MB; ++dx)
            {
                for (dy = 0; dy < 1 + height2 - MAX_SIZE_MB; ++dy)
                {
                    if (i != dx && j != dy)
                    {
                        int cur_cost = calculate_cost(i, j*height1, dx, dy*height2);
                        if (cur_cost == 0)
                        {
                            min = cur_cost;
                            min_x = i, min_y = j;
                            min_dx = dx, min_dy = dy;
                            result[i][j] = 0;
                            break;
                        }
                        else
                        {
                            if (cur_cost < min)
                            {
                                min = cur_cost;
                            }
                            min_x = i, min_y = j, min_dx = dx, min_dy = dy;
                            result[i][j] = min;
                        }
                    }
                }
            }
        }
    }
    return result;
}

int main(int argc, char* argv[]){
    const char* frame1_name = "./data/lena_gray.bmp";
    frame1 = readBMP(frame1_name);
    width1=width,height1=height,size1=size;
    cout<<"width1="<<width<<"\n"<<"height1="<<height<<"\n";
    const char* frame2_name = "./data/lena_gray_copy.bmp";
    frame2 = readBMP(frame2_name);
    width2=width,height2=height,size2=size;
    cout<<"width2="<<width<<"\n"<<"height2="<<height<<"\n";
    if(!frame1 || !frame2){
        return 1;
    } 
    int **result = compute_matrix(width1,height1,width2,height2);
} 
