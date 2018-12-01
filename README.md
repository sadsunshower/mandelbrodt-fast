# Mandelbrot

Mandelbrot set image generator, accelerated with AVX instructions.

## Buidling

Using the provided Makefile:

```sh
make build
```

Or, for an executable with GDB information:

```sh
make build-debug
```

## Usage

The program can be run with

```sh
./mandel (size) (max iterations) (x min) (x max) (y min) (y max) [palette file]
```

`palette file` is a text file with lines in the format `RRR, GGG, BBB` with each of those values between 0 and 255. The number of iterations / max iterations is used to calculate the line in the palette file to use, scaled to fit the size of the palette. A sample `palette.txt` is provided in the repo.

If omitted, the palette defaults to just a linear green gradient.

The Python script used to generate the sample palette is included for completeness.

The program outputs a Microsoft BMP directly to stdout, which you can display directly (with ImageMagick) like so:

```sh
./mandel 1536 768 -0.595 -0.565 0.45 0.48 palette.txt | display -
```

Or, to save as a PNG:

```sh
./mandel 1536 768 -0.595 -0.565 0.45 0.48 palette.txt | convert - PNG24:sample.png
```

As was used to generate the `sample.png` present in the repo.
