# include <stdio.h>
# include <stdlib.h>

# include <immintrin.h>

# include "bitmap.h"

int main (int argc, char** argv) {
    int size, iters;
    
    float xmin, xmax, ymin, ymax;

    // Raise error if no command line args given
    if (argc != 7 && argc != 8) {
        fprintf(stderr, "Usage: %s (size) (iterations) (x min) (x max) (y min) (y max) [palette file]\n", argv[0]);
        return 1;
    }

    // Load values of command line args
    size = atoi(argv[1]);
    iters = atoi(argv[2]);

    sscanf(argv[3], "%f", &xmin);
    sscanf(argv[4], "%f", &xmax);
    sscanf(argv[5], "%f", &ymin);
    sscanf(argv[6], "%f", &ymax);

    // Load the palette
    Pixel* palette = malloc(sizeof(Pixel) * 1024);
    int palette_size = 0;
    int parray_size = 1024;

    if (argc == 8) {
        // If a palette file was given, read it
        FILE* pf = fopen(argv[7], "r");

        if (!pf) {
            fprintf(stderr, "%s: Palette file %s could not be loaded", argv[0], argv[7]);
            return 1;
        }

        char line[32];
        while (fgets(line, 16, pf)) {
            // Parse each line and load into a Pixel struct
            Pixel* p = &palette[palette_size];
            int r, g, b;
            sscanf(line, "%d, %d, %d", &r, &g, &b);
            p->r = (unsigned char) r;
            p->g = (unsigned char) g;
            p->b = (unsigned char) b;
            palette_size++;
            if (palette_size == parray_size) {
                parray_size *= 2;
                palette = realloc(palette, sizeof(Pixel) * parray_size);
            }
        }
        fclose(pf);
    } else {
        // Otherwise, use a simple linear green gradient
        palette_size = 255;
        for (int i = 0; i < palette_size; i++) {
            palette[i].r = 0;
            palette[i].g = i;
            palette[i].b = 0;
        }
    }

    // To take advantage of AVX, size must be a multiple of 8
    if (size % 8 != 0) {
        fprintf(stderr, "%s: Size must be a multiple of 8\n", argv[0]);
        return 1;
    }

    // Create an empty image
    Bitmap img = bitmap_create(size, size);

    float* xvalues;
    float* yvalues;

    float* xscaling;
    float* yscaling;

    int* iterations;

    // Initialise x and y values, aligned to 32 bytes
    posix_memalign((void**) &xvalues, 32, sizeof(float) * size * size);
    posix_memalign((void**) &yvalues, 32, sizeof(float) * size * size);

    posix_memalign((void**) &xscaling, 32, sizeof(float) * size * size);
    posix_memalign((void**) &yscaling, 32, sizeof(float) * size * size);

    // Initialise iterations array
    iterations = malloc(sizeof(int)*size*size);
    
    // Initialise arrays
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int addr = (i * size) + j;

            // Calculate scaling, to position the "camera" over the Mandelbrot set
            xscaling[addr] = ((i / (float) size) * (xmax - xmin)) + xmin;
            yscaling[addr] = ((j / (float) size) * (ymax - ymin)) + ymin;

            xvalues[addr] = 0.0f;
            yvalues[addr] = 0.0f;

            iterations[addr] = 0;
        }
    }
   
    // Create a 32-byte aligned float array of 2.0f, used to double with AVX
    float dbl[8] __attribute__((aligned(32)));
    for (int i = 0; i < 8; i++) dbl[i] = 2.0f;

    for (int iter = 0; iter < iters; iter++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j += 8) {
                int addr = (i * size) + j;

                // Firstly, check each value
                int skip = 1;
                for (int k = 0; k < 8; k++) {
                    // If this particular pixel needs to be incremented, increment it, by first checking x^2 + y^2 < 4
                    if (xvalues[addr + k] * xvalues[addr + k] + yvalues[addr + k] * yvalues[addr + k] < 4.0f) {
                        iterations[addr + k]++;
                        skip = 0;
                    }
                }

                // If none of the pixels in this "block" needed incrementing, skip calculations
                if (skip) continue;

                // Load x and y
                __m256 mm_x = _mm256_load_ps(&xvalues[addr]);
                __m256 mm_y = _mm256_load_ps(&yvalues[addr]);

                // Let xs = x^2, ys = y^2
                __m256 mm_xs = _mm256_mul_ps(mm_x, mm_x);
                __m256 mm_ys = _mm256_mul_ps(mm_y, mm_y);

                // Let xt = x^2 - y^2
                __m256 mm_xt = _mm256_sub_ps(mm_xs, mm_ys);

                // Let d = 2, and ynt = x * y
                // Then, let yn = 2 * x * y
                __m256 mm_d = _mm256_load_ps(dbl);
                __m256 mm_ynt = _mm256_mul_ps(mm_x, mm_y);
                __m256 mm_yn = _mm256_mul_ps(mm_ynt, mm_d);

                // Load x scaling (xsc) and y scaling (ysc)
                __m256 mm_xsc = _mm256_load_ps(&xscaling[addr]);
                __m256 mm_ysc = _mm256_load_ps(&yscaling[addr]);

                // Let x = xt + xsc
                // Let y = yn + ysc
                mm_x = _mm256_add_ps(mm_xt, mm_xsc);
                mm_y = _mm256_add_ps(mm_yn, mm_ysc);

                // Store x and y back into the array
                _mm256_store_ps(&xvalues[addr], mm_x);
                _mm256_store_ps(&yvalues[addr], mm_y);
            }
        }
    }

    // Free the x and y values, and x and y scaling (no longer needed)
    free(xvalues);
    free(yvalues);

    free(xscaling);
    free(yscaling);

    // Fill out the pixels in the image, using the palette and the iterations array
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float colour = iterations[(i * size) + j] / ((float) iters);
            colour = palette_size * colour;
            bitmap_set_pixel(img, i, j, palette[(int) colour]);
        }
    }

    // Write the image to stdout
    bitmap_save(img, stdout);

    // Free any remaining memory
    free(iterations);
    free(palette);

    bitmap_drop(img);

    return 0;
}
