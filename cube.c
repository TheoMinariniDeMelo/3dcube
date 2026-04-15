#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define move(x, y) printf("\x1b[%d;%dH", y, x) 
#define clear() printf("\x1b[2J\x1b[H")
#define plot(x,y) move(x,y); printf("%c", CUBE_CHAR)
#define CUBE_SIZE 50
#define CUBE_CHAR '#'
#define FPS 30

#define CUBE_LEFT  (-CUBE_SIZE/ 2.0f)
#define CUBE_TOP   ( CUBE_SIZE / 2.0f)
#define CUBE_RIGHT ( CUBE_SIZE / 2.0f)
#define CUBE_DOWN  (-CUBE_SIZE / 2.0f)
#define CUBE_FRONT ( CUBE_SIZE / 2.0f)
#define CUBE_BACK  (-CUBE_SIZE / 2.0f)

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int numrow;
    int numcol;
} Screen;

Vector3 cube[8] = {
    {CUBE_LEFT,  CUBE_DOWN, CUBE_BACK}, 
    {CUBE_RIGHT, CUBE_DOWN, CUBE_BACK}, 
    {CUBE_RIGHT, CUBE_DOWN, CUBE_FRONT},
    {CUBE_LEFT,  CUBE_DOWN, CUBE_FRONT},
    {CUBE_LEFT,  CUBE_TOP,  CUBE_BACK}, 
    {CUBE_RIGHT, CUBE_TOP,  CUBE_BACK}, 
    {CUBE_LEFT,  CUBE_TOP,  CUBE_FRONT}, 
    {CUBE_RIGHT, CUBE_TOP,  CUBE_FRONT},
};

int arestas[12][2] =  {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 7}, {6, 7}, {6, 4},
    {0, 4}, {1, 5}, {2, 7}, {3, 6}
};

float d = 80.0f;

void print_cube(Screen);
void plot_line(Point, Point);
void rotate_x(Vector3*, float);
void rotate_y(Vector3*, float);
void rotate_z(Vector3*, float);
int get_cursor_position(int*, int*);
int get_window_size(Screen*);

Point project(Vector3);

int main(){
    float angle = 0.1;
    Screen sc;

    printf("\x1b[?25l"); // make cursor invisible
    if(get_window_size(&sc) <= 0){
        fprintf(stderr, "Error to get window size");
        exit(1);
    };
    if(sc.numrow < 25 || sc.numcol < 40){
        fprintf(stderr, "Not valid window size");
        exit(1);
    }

    for(;;){
        for(int i = 0; i < 8; ++i){
            rotate_y(&cube[i], angle);
            rotate_z(&cube[i], angle);
            rotate_x(&cube[i], angle);
        };
        clear();
        print_cube(sc);
        usleep(1000000/FPS);
    }
}

void print_cube(Screen sc){
    for(int i = 0; i < 12; ++i){
        int *aresta = arestas[i];
        Point p1 = project(cube[aresta[0]]);
        Point p2 = project(cube[aresta[1]]);
        p1.x = (int)(sc.numcol/2) + p1.x;
        p1.y = (int)(sc.numrow/2) - p1.y;
        p2.x = (int)(sc.numcol/2) + p2.x;
        p2.y = (int)(sc.numrow/2) - p2.y;
        plot_line(p1, p2);
    }
}

void 
rotate_x(Vector3* vector, float angle){
    float c = cos(angle);
    float s = sin(angle);
    float x = vector->x, y = vector->y, z = vector->z;
    vector->x = x;
    vector->y = y * c - z * s;
    vector->z = y * s + z * c;
}
void 
rotate_y(Vector3* vector, float angle){
    float c = cos(angle);
    float s = sin(angle);
    float x = vector->x, y = vector->y, z = vector->z;
    vector->x = x * c + z * s;
    vector->y = y;
    vector->z = -x * s + z * c;
}
void 
rotate_z(Vector3* vector, float angle){
    float c = cos(angle);
    float s = sin(angle);
    float x = vector->x, y = vector->y, z = vector->z;
    vector->x = x * c - y * s;
    vector->y = x*s + y * c;
    vector->z = z; 
}

//              y
//   y' =   ---------
//           z/d + 1
Point project(Vector3 v){
    v.z += 100;
    float px = (v.x * d) / (v.z + d);
    float py = (v.y * d) / (v.z + d);
    return (Point){
        .x = (int) px,
        .y = (int) py,
    };
}


void 
plot_line(Point p1, Point p2){
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int x = p1.x;
    int y = p1.y;
    int sx = dx > 0 ? 1 : -1;
    int sy = dy > 0 ? 1 : -1;
    if(dx == 0){
        for(int i = 0; i < dy; i++){
            move(x, y);
            y += sy;
        }
        return;
    }
    float m = (float)dy/dx;
    int y0 = y + sy;
    while(abs(x) <= abs(p2.x)){
        plot(x, y);
        x += sx;
        float p = m * (x - p1.x) + p1.y; 
        if(fabs(p - y0) < fabs(p - y)){
            y += sy; y0 += sy;
        }
    }

}
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
    return 1;
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
    return 1;
}
