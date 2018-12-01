# include <stdio.h>
# include <stdlib.h>

# include "bitmap.h"

// Create a new Bitmap image struct
Bitmap bitmap_create (int width, int height)
{
    Bitmap new = malloc(sizeof(struct bitmap));
    new->width = width;
    new->height = height;
    new->pixels = calloc(width * height * 3, sizeof(unsigned char));
    return new;
}

// Free the memory used by a Bitmap image struct
void bitmap_drop (Bitmap bmp)
{
    free(bmp->pixels);
    free(bmp);
}

// Set the contents of one pixel in a Bitmap image
void bitmap_set_pixel (Bitmap bmp, int x, int y, Pixel p)
{
    int base = ((bmp->height - y - 1) * bmp->width * 3) + (x * 3);
    bmp->pixels[base] = p.b;
    bmp->pixels[base + 1] = p.g;
    bmp->pixels[base + 2] = p.r;
}

// Get the contents of one pixel in a Bitmap image
Pixel bitmap_get_pixel (Bitmap bmp, int x, int y)
{
    Pixel ret;
    int base = ((bmp->height - y - 1) * bmp->width * 3) + (x * 3);
    ret.b = bmp->pixels[base];
    ret.g = bmp->pixels[base + 1];
    ret.r = bmp->pixels[base + 2];
    return ret;
}

// Save the Bitmap image as a primitive Microsoft BMP (no compression)
void bitmap_save (Bitmap bmp, FILE* f)
{
    // Write header
    char* sig = "BM";
    unsigned int size = 54u + (unsigned int) (bmp->width * bmp->height * 3);
    unsigned int none = 0u;
    unsigned int offset = 54u;

    fwrite(sig, sizeof(char), 2, f);
    fwrite(&size, sizeof(unsigned int), 1, f);
    fwrite(&none, sizeof(unsigned int), 1, f);
    fwrite(&offset, sizeof(unsigned int), 1, f);

    // Write information header
    unsigned int info = 40u;
    unsigned int width = (unsigned int) bmp->width;
    unsigned int height = (unsigned int) bmp->height;
    unsigned short planes = (unsigned short) 1u;
    unsigned short bpp = (unsigned short) 24u;
    unsigned int res = 100u;

    fwrite(&info, sizeof(unsigned int), 1, f);
    fwrite(&width, sizeof(unsigned int), 1, f);
    fwrite(&height, sizeof(unsigned int), 1, f);
    fwrite(&planes, sizeof(unsigned short), 1, f);
    fwrite(&bpp, sizeof(unsigned short), 1, f);
    fwrite(&none, sizeof(unsigned int), 1, f);
    fwrite(&none, sizeof(unsigned int), 1, f);
    fwrite(&res, sizeof(unsigned int), 1, f);
    fwrite(&res, sizeof(unsigned int), 1, f);
    fwrite(&none, sizeof(unsigned int), 1, f);
    fwrite(&none, sizeof(unsigned int), 1, f);

    // Write pixel data
    fwrite(bmp->pixels, sizeof(unsigned char), bmp->width * bmp->height * 3, f);
}
