#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef struct {
    int x;
    int y;
    int z;
} Vector;

typedef struct {
    int x;
    int y;
} Point;

Vector vertices[8][3] = {
    {-1, -1, -1}, // 0
    { 1, -1, -1}, // 1
    { 1,  1, -1}, // 2
    {-1,  1, -1}, // 3
    {-1, -1,  1}, // 4
    { 1, -1,  1}, // 5
    { 1,  1,  1}, // 6
    {-1,  1,  1}  // 7
};

int arestas[12][2] =  {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // face traseira
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // face dianteira
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

int d = 10;

void draw_line(Vector, Vector);
void rotate_x(Vector*, float);
void rotate_y(Vector*, float);
void rotate_z(Vector*, float);
Point project(Vector);

int main(){

}

void rotate_x(Vector* vector, float angle){
    float _cos = cos(angle);
    float _sin = sin(angle);
    vector->x = vector->x;
    vector->y = vector->y * _cos - vector->z * _sin;
    vector->z = vector->y * _sin + vector->z * _cos;
}
