#include <stdio.h> 
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <signal.h>

//#define _USE_MATH_DEFINES

#define PI 3.14159265

#define ROWS 32
#define COLS 32

#define NORMALIZE(f) ((f + 1.0f) / 2.0f)
#define TO_RADIANS(deg) ((double)deg * (PI / 180.0))

typedef struct WPoint {
    float x;
    float y;
    float z;
} WPoint;

typedef struct SPoint {
    uint8_t x;
    uint8_t y;
} SPoint;

const size_t fps = 30;
const double dT = 1 / (double)fps;
const char *lightingSymbols = " .,:;+*?%%S#@";

uint8_t screenBuffer[ROWS][COLS] = {};

void pointScr(WPoint fp, SPoint *screenPoints, size_t idx) {
    float z = fp.z;
    
    if (fabsf(z) < 0.1f) return;

    
    WPoint projFp = {fp.x / z, fp.y / z, 0};

    int x = (COLS - 1) * NORMALIZE(projFp.x);
    int y = (ROWS - 1) * NORMALIZE(projFp.y);

    if (x < 0 || y < 0 || x >= COLS || y >= ROWS) return;

    screenPoints[idx].x = x; // write to point buffer array
    screenPoints[idx].y = y;

    

    //screenBuffer[y][x] = 7;

}

void renderScr() {

    char flatScreenBuffer[ROWS * (COLS + 1) * 2 + 1];

    size_t idx = 0;

    for (size_t r = 0; r < ROWS; ++r) {
        for (size_t c = 0; c < COLS; ++c) {

            flatScreenBuffer[idx++] = lightingSymbols[screenBuffer[r][c]];
            flatScreenBuffer[idx++] = ' ';
        }

        flatScreenBuffer[idx++] = '\n';
        
    }

    flatScreenBuffer[idx++] = '\0';

    printf("%s\n", flatScreenBuffer);

    memset(screenBuffer, 0, sizeof(screenBuffer)); // clear screen buffer
}

WPoint vertices[] = {

    {-0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},
    

}; 

uint16_t tris[] = {
    0, 1, 2,
    0, 2, 3,
    
    6, 5, 4,
    7, 6, 4,
    
    6, 2, 1,
    5, 6, 1, 

    5, 1, 0,
    4, 5, 0,

    0, 3, 7,
    0, 7, 4,

    3, 2, 6,
    3, 6, 7,

};

void rotateX(WPoint* point, float angle) {
    float y = point->y;
    float z = point->z;

    float c = cos(TO_RADIANS(angle * dT));
    float s = sin(TO_RADIANS(angle * dT));

    point->y = y * c - z * s;
    point->z = y * s + z * c;
}

void rotateY(WPoint* point, float angle) {
    // x' = xcosB - zsinB
    // z' = xsinB + zcosB
    
    float x = point->x;
    float z = point->z;

    float c = cos(TO_RADIANS(angle * dT));
    float s = sin(TO_RADIANS(angle * dT));

    point->x = x * c - z * s;
    point->z = x * s + z * c;

}

void rotateZ(WPoint* point, float angle) {
    float x = point->x;
    float y = point->y;

    float c = cos(TO_RADIANS(angle * dT));
    float s = sin(TO_RADIANS(angle * dT));

    point->x = x * c - y * s;
    point->y = x * s + y * c;


}

/*
#define INBOUNDS(x,y) ((x) >= 0 && (x) < COLS && (y) >= 0 && (y) < ROWS)

// bresenham line algorithm
void drawLine(SPoint point1, SPoint point2) {
    int x0 = point1.x, y0 = point1.y, x1 = point2.x, y1 = point2.y;
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {

        //assert(!INBOUNDS(x0, y0));

        if (INBOUNDS(x0, y0))
            screenBuffer[y0][x0] = 4;

        if(x0 == x1 && y0 == y1) break;

        int e2 = err << 1;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;

        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }

    }
}
*/
double Magnitude(WPoint v) {
    double squaredSum = v.x * v.x + v.y * v.y + v.z * v.z;
    double sqrootSum = sqrt(squaredSum);

    return sqrootSum;
}

WPoint Cross(WPoint v0, WPoint v1) {
    float x = v0.y * v1.z - v0.z * v1.y;
    float y = v0.z * v1.x - v0.x * v1.z;
    float z = v0.x * v1.y - v0.y * v1.x;

    WPoint crossProduct = {x, y, z};

    return crossProduct;
}

WPoint DivideVectorScalar(WPoint v, float scalar) {
    assert(fabsf(scalar) > 0.1f); // prevent division by zero
    

    WPoint quotient = {v.x / scalar, v.y / scalar, v.z / scalar};

    return quotient;
}

WPoint VectorAdd(WPoint v1, WPoint v0) {
    WPoint difference = {v1.x + v0.x, v1.y + v0.y, v1.z + v0.z};

    return difference;

}

WPoint VectorSubtract(WPoint v1, WPoint v0) {
    WPoint difference = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};

    return difference;

}

double Dot(WPoint v0, WPoint v1) {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;

}

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    running = 0;

}

int RastEdge(SPoint vert0, SPoint vert1, SPoint vert2) {
    return (vert1.x - vert0.x) * (vert2.y - vert0.y) - (vert1.y - vert0.y) * (vert2.x - vert0.x);
}

#define MAX(a, b) ((a > b) ? (a) : (b))

int main() {
    signal(SIGINT, handle_sigint);

    WPoint camPos = {0.0f, 0.0f, 1.5f};
    WPoint lightSource = {0.0f, -1.0f, 1.5f};

    size_t ticks = 0;

    WPoint cubeVertices[8] = {};
    for (size_t i = 0; i < 8; ++i) {
        cubeVertices[i] = vertices[i];
    }

    for (;running; ++ticks) {
        fflush(stdout);
        system("clear");

        printf("\033[34m");
        
        SPoint screenPoints[sizeof(cubeVertices) / sizeof(cubeVertices[0])] = {};
        for (size_t i = 0; i < sizeof(cubeVertices) / sizeof(cubeVertices[0]); ++i) {
            
            rotateY(&cubeVertices[i], 20.f);
            rotateX(&cubeVertices[i], 40.f);
            


            pointScr(VectorAdd(cubeVertices[i], camPos), screenPoints, i);
        }

         
        size_t idxSize = sizeof(tris) / sizeof(tris[0]) / 3;
        for (size_t i = 0; i < idxSize; ++i) {
            uint16_t tri[] = {tris[i * 3], tris[i * 3 + 1], tris[i * 3 + 2]}; 
            
            SPoint vert0 = screenPoints[tri[0]];
            SPoint vert1 = screenPoints[tri[1]];
            SPoint vert2 = screenPoints[tri[2]];

            WPoint Wvert0 = cubeVertices[tri[0]];
            WPoint Wvert1 = cubeVertices[tri[1]];
            WPoint Wvert2 = cubeVertices[tri[2]];

            WPoint centroid = {(Wvert0.x + Wvert1.x + Wvert2.x) / 3, (Wvert0.y + Wvert1.y + Wvert2.y) / 3, (Wvert0.z + Wvert1.z + Wvert2.z) / 3};
            WPoint edgeB = {Wvert1.x - Wvert0.x, Wvert1.y - Wvert0.y, Wvert1.z - Wvert0.z};
            WPoint edgeC = {Wvert2.x - Wvert0.x, Wvert2.y - Wvert0.y, Wvert2.z - Wvert0.z};

            WPoint crossProduct = Cross(edgeC, edgeB);
            WPoint triNormal = DivideVectorScalar(crossProduct, Magnitude(crossProduct));
            
            WPoint cameraV = VectorSubtract(camPos, centroid);
            WPoint cameraNormal = DivideVectorScalar(cameraV, Magnitude(cameraV));
           
            double cameraDot = Dot(triNormal, cameraNormal);
            
            WPoint lightToCentroidV = VectorSubtract(lightSource, centroid);
            WPoint lightingNormal = DivideVectorScalar(lightToCentroidV, Magnitude(lightToCentroidV));

            double lightingDot = MAX(0, Dot(triNormal, lightingNormal));
            
            //double lightingDot = Dot(normal, 

            if (cameraDot > 0) {
                for (size_t r = 0; r < ROWS; ++r) {
                    for (size_t c = 0; c < COLS; ++c) {
                        SPoint spt = {c, r};

                        int E01 = RastEdge(vert0, vert1, spt);
                        int E12 = RastEdge(vert1, vert2, spt);
                        int E20 = RastEdge(vert2, vert0, spt);

                        if (E01 < 0 && E12 < 1 && E20 < 1) {
                            int charToUse = MAX(1, (int)(lightingDot * strlen(lightingSymbols)));

                            screenBuffer[c][r] = charToUse;


                        }
                    }
                }

                //edge(vert0, vert1, )

                /*
                drawLine(vert0, vert1);
                drawLine(vert1, vert2);
                drawLine(vert2, vert0);
                */
            }
        }
        
        renderScr();
        
        
        usleep((1000 * 1000) / fps); // 1ms = 1 micro second * 1000
    }
    
    printf("\033[0m\n");
   
    return 0;
}


