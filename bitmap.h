# ifndef BITMAP_H
# define BITMAP_H

# include <stdio.h>

struct pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

typedef struct pixel Pixel;

struct bitmap {
    int width;
    int height;
    unsigned char* pixels;
};

typedef struct bitmap* Bitmap;

Bitmap bitmap_create (int width, int height);
void bitmap_drop (Bitmap bmp);

void bitmap_set_pixel (Bitmap bmp, int x, int y, Pixel p);
Pixel bitmap_get_pixel (Bitmap bmp, int x, int y);

void bitmap_save (Bitmap bmp, FILE* f);

# endif
