#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>	

#define NB_VOISINS 8
#define SEUIL 30
#define Max(x, y) x > y ? x : y
#define Min(x, y) x < y ? x : y


typedef struct pixel_t pixel_t;
struct pixel_t{
	GLubyte r,g,b;
};

typedef struct hsv_t hsv_t;
struct hsv_t{
	double  h, s, v;
};

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
};
typedef struct Image Image;
typedef unsigned short utab [3][3][3];

int ImageLoad_PPM(char *filename, Image *image);
void imagesave_PPM(char *filename, Image *image);
/*
void floattoint3(float, float, float, int *, int *, int *);
void inttofloat3(int, int, int, float *, float *, float*);
void rgbtohls(float, float, float, float *, float *, float *);
void hlstorgb(float, float, float, float *, float *, float *);
void modifclr(GLubyte *, GLubyte *, int);
void dlr27(Image *), dlr54(Image *), dlr27fs(Image *);
*/

/*void untrait (Image *), untraitOK (Image *);
void moserr (Image *), mosprox(Image *);
Image * try2  (Image *);
Image * toclut  (Image *);
*/
void moyenneVoisinage(Image * image);
void moyennePondereeVoisinage(Image * image);
void contraste(Image * image);
void contraste_teinte(Image * image);