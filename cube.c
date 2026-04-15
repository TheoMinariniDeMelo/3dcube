#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define move(x, y) printf("\x1b[%d;%dH", x, y) 
#define clear() printf("\x1b[2J")
#define CUBE_SIZE 50;

typedef struct {
    int x;
    int y;
    int z;
} Vector;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int numrow;
    int numcol;
} Screen;

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

void print_cube();
void draw_line(Vector, Vector);
void rotate_x(Vector*, float);
void rotate_y(Vector*, float);
void rotate_z(Vector*, float);

Point project(Vector);

int get_cursor_position(int *row, int *col){
    char buf[32];
    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    unsigned int i = 0;
    int n;
    while(i < sizeof(buf) - 1){
        if((n = read(STDIN_FILENO, &buf[i], 1)) != 1) break;
        if(buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if(buf[0] != '\x1b' || buf[1] != '[') return -1;
    if(sscanf(&buf[2], "%d;%d", row, col) != 2) return -1;
    return 0;
}


int get_window_size(Screen *screen){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12 ) != 12) return -1;
        return get_cursor_position(&screen->numrow, &screen->numcol);
    }else {
        screen->numrow = ws.ws_row;
        screen->numcol = ws.ws_col;
    }
    return 0;
}

int main(){
    float angle = 1;
    Screen sc;
    
    if(get_window_size(&sc) <= 0){
        perror("Error to get window size");
        exit(1);
    };
    if(sc.numrow < 40 || sc.numcol < 40){
        exit(1);
    }

    for(;;){
        for(int i = 0; i < 8; ++i){
            rotate_x(vertices[i], angle);
            rotate_y(vertices[i], angle);
            rotate_z(vertices[i], angle);
        }
        print_cube();
    }
}

void print_cube(){
    for(int i = 0; i < 8; ++i){
        
    }
}

void 
rotate_x(Vector* vector, float angle){
    float _cos = cos(angle);
    float _sin = sin(angle);
    vector->x = vector->x;
    vector->y = vector->y * _cos - vector->z * _sin;
    vector->z = vector->y * _sin + vector->z * _cos;
}
void 
rotate_y(Vector* vector, float angle){
    float _cos = cos(angle);
    float _sin = sin(angle);
    vector->x = vector->x * _cos + vector->z * _sin;
    vector->y = vector->y;
    vector->z = -vector->x * _sin + vector->z * _cos;
}
void 
rotate_z(Vector* vector, float angle){
    float _cos = cos(angle);
    float _sin = sin(angle);
    vector->x = vector->x * _cos - vector->y * _sin;
    vector->y = vector->y * _cos - vector->x * _sin;
    vector->z = vector->z; 
}

void 
draw_line(Vector vec1, Vector vec2){
    
}
