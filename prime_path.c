#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "print.h"

int numDigits(int x);
char* intToStr(int x);
// read primes from file "name" into array "p"
// returns number of prime numbers read, output array size to "size"
int readPrimes(int ** const p, int *size, const char *name);
void writePrimes(const int *p, int n, const char *name);
int generatePrimes(int *p, int *size, int bound);
int isPrime(int x);
void* initArray(size_t num, size_t size);
// Expand array by a factor of 2
void* expandArray(void *a, size_t num, size_t size, int right);
void deleteArray(int **a, int n);
int writeArrayToFile(const int **a, const char *name, int pad, int x, int y);

static int primeCount = 0, primeSize = 0;
static int *primes = NULL;

struct Coord {
    int x, y;
};

int main(int argc, char *argv[])
{
    int start = 1;
    int end = 100;
    switch (argc)
    {
    case 5:
    case 4:
        {
            primeCount = readPrimes(&primes, &primeSize, argv[3]);
        }
    case 3:
        {
            end = atoi(argv[2]);
        }
    case 2:
        {
            start = atoi(argv[1]);
            break;
        }
    case 1:
        {
            break;
        }
    default:
        {
            fprintf(stderr, "Too many arguments!\n");
            return 0;
        }
    }
    if (start < 1)
    {
        start = 1;
    }
    if (end < start)
    {
        end = start;
    }

    int pCount = 0;
    int max_x = 1;
    int max_y = 1;
    int n = start, x = (max_x - 1) / 2, y = (max_y - 1)/ 2;
    // R, UR, U, UL, L, DL, D, DR
    const int rotateTotal = 8;
    const struct Coord directions[] = {
                    { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
                    { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
    int current = 0;
    struct Coord translate = directions[current];

    /* Initialize array */
    int **p = (int **)initArray(max_x, sizeof(int *));
    int ow_size = 2, ow_index = 0;
    int *overwritten = (int *)initArray(ow_size, sizeof(int));

    int i;
    for (i = 0; i < max_x; ++i)
    {
        p[i] = (int *)initArray(max_y, sizeof(int));
    }

    SDL_Surface *s = NULL, *sP = NULL;
    const char *title = "Prime Path";
    int s_w = 160, s_h = 120, scale = 4;
    s_w *= scale; s_h *= scale;
    int s_x = s_w / 2, s_y = s_h / 2;
    if (InitSDL())
    {
        s = InitScreen(s_w, s_h, title);
    }
    while (n <= end)
    {
        if (0 != p[x][y])
        {
            overwritten[ow_index++] = p[x][y];
            overwritten[ow_index++] = n;
            if (ow_index >= ow_size)
            {
                overwritten = (int *)expandArray(overwritten, ow_size, sizeof(int), 1);
                ow_size *= 2;
            }
        }
        p[x][y] = n;
        if (s && s_x >= 0 && s_x < s_w && s_y >= 0 && s_y < s_h)
        {
            printPoint(s, s_x, s_y);
        }
        if (isPrime(n))
        {
            ++pCount;
            ++current;
            if (rotateTotal == current)
            {
                current = 0;
            }
            translate = directions[current];
        }

        switch (translate.x)
        {
        case 1:
            {
                ++x; ++s_x;
                if (x >= max_x)
                {
                    p = (int **)expandArray(p, max_x, sizeof(int *), 1);
                    max_x *= 2;
                    for (i = max_x / 2; i < max_x; ++i)
                    {
                        p[i] = (int *)initArray(max_y, sizeof(int));
                    }
                }
                break;
            }
        case -1:
            {
                --x; --s_x;
                if (x < 0)
                {
                    p = (int **)expandArray(p, max_x, sizeof(int *), 0);
                    for (i = 0; i < max_x; ++i)
                    {
                        p[i] = (int *)initArray(max_y, sizeof(int));
                    }
                    x = max_x - 1;
                    max_x *= 2;
                }
                break;
            }
        }
        switch (translate.y)
        {
        case 1:
            {
                --y; --s_y;
                if (y < 0)
                {
                    for (i = 0; i < max_x; ++i)
                    {
                        p[i] = (int *)expandArray(p[i], max_y, sizeof(int), 0);
                    }
                    y = max_y - 1;
                    max_y *= 2;
                }
                break;
            }
        case -1:
            {
                ++y; ++s_y;
                if (y >= max_y)
                {
                    for (i = 0; i < max_x; ++i)
                    {
                        p[i] = (int *)expandArray(p[i], max_y, sizeof(int), 1);
                    }
                    max_y *= 2;
                }
                break;
            }
        }
        ++n;
    }
    printf("Finished creating path!\n");

/*
    int temp = n, pad = 0;
    // get number of digits of n
    while (temp)
    {
        temp /= 10;
        ++pad;
    }
    if (writeArrayToFile((const int **)p, "diagram.txt", pad, max_x, max_y))
    {
        printf("Done writing to file!\n");
    }
    for (i = 0; i < ow_index; i += 2)
    {
        //if (isPrime(overwritten[i]))
        printf("%d %d; ", overwritten[i], overwritten[i + 1]);
    }
    printf("\n");
*/
    deleteArray(p, max_x);

    const char *fileExt = ".bmp";
    char *widthS = intToStr(s_w), *heightS = intToStr(s_h);
    char *startS = intToStr(start), *endS = intToStr(end);
    int bmpNameLen = strlen(widthS) + 1 + strlen(heightS) + 1 +
                strlen(startS) + 1 + strlen(endS) + strlen(fileExt);
    char bmpName[bmpNameLen+1];
    strcpy(strcpy(strcpy(strcpy(strcpy(strcpy(strcpy(strcpy(bmpName, widthS) + strlen(widthS),"x") + 1, heightS) + strlen(heightS), "_") + 1, startS) + strlen(startS), "-") + 1, endS) + strlen(endS), fileExt);
    bmpName[bmpNameLen] = '\0';
    if (0 == SDL_SaveBMP(s, bmpName))
    {
        printf("Saved screen to %s\n", bmpName);
    }
    delay(1000);
    FreeScreen(s);
    QuitSDL();
    printf("Quit SDL\n");

    if (5 == argc)
    {
        writePrimes((const int *)primes, pCount, argv[4]);
    }

    return 0;
}

int writeArrayToFile(const int **a, const char *name, int pad, int x, int y)
{
    assert(x >= 1 && y >= 1);
    FILE *file = fopen(name, "w");
    if (!file)
    {
        return 0;
    }
    int i, j, first_x = x, first_y = y, last_y = 0;
    int *last_x = (int *)initArray(y, sizeof(int));
    i = j = 0;
    while (j < y)
    {
        while (i < x)
        {
            if (0 != a[i][j])
            {
                if (j < first_y)
                {
                    first_y = j;
                }
                if (i < first_x)
                {
                    first_x = i;
                }
                if (j > last_y)
                {
                    last_y = j;
                }
                if (i > last_x[j])
                {
                    last_x[j] = i;
                }
            }
            ++i;
        }
        i = 0;
        ++j;
    }
    j = first_y;
    i = first_x;
    while (j <= last_y)
    {
        while (i <= last_x[j])
        {
            if (0 == a[i][j])// || !isPrime(a[i][j]))
            {
                fprintf(file, "%*s", pad+1, "");
            }
            else
            {
                fprintf(file, "%*d ", pad, a[i][j]);
            }
            ++i;
        }
        fprintf(file, "\n");
        i = first_x;
        ++j;
    }
    fclose(file);
    free(last_x);
    return 1;
}

void* initArray(size_t num, size_t size)
{
    void *a = calloc(num, size);
    assert(a);
    return a;
}

void* expandArray(void *a, size_t num, size_t size, int right)
{
    assert(a && num > 0);
    a = realloc(a, size*2*num);
    assert(a);
    size_t i, half = size * num;
    void *b = a + half;
    for (i = 0; i < half; ++i)
    {
        if (right)
        {
            ((char *)b)[i] = 0;
        }
        else
        {
            ((char *)b)[i] = ((char *)a)[i];
            ((char *)a)[i] = 0;
        }
    }
    return a;
}

void deleteArray(int **a, int n)
{
    assert(a && n >= 1);
    while (--n >= 0)
    {
        free(a[n]);
    }
    free(a);
}

int readPrimes(int ** const p, int *size, const char *name)
{
    assert(NULL == *p);
    assert(name);
    FILE *f = fopen(name, "r");
    if (!f)
    {
        fprintf(stderr, "Error reading '%s'\n", name);
        return 0;
    }

    int c, x = 0, pCount = 0, pSize = 1;
    *p = (int *)initArray(pSize, sizeof(int));
    while ((c = fgetc(f)) != EOF)
    {
        if ('0' <= (char)c && (char)c <= '9')
        {
            x = (x * 10) + (c - '0');
        }
        else
        {
            if (x > 0)
            {
                if (pCount == pSize)
                {
                    pSize *= 2;
                    *p = (int *)realloc(*p, sizeof(int)*pSize);
                }
                (*p)[pCount++] = x;
            }
            x = 0;
        }
    }
    fclose(f);
    *size = pSize;
    return pCount;
}

void writePrimes(const int *p, int n, const char *name)
{
    assert(p && n > 1 && name);
    FILE *f = fopen(name, "w");
    if (!f)
    {
        fprintf(stderr, "Failed to read '%s'\n", name);
        return;
    }

    int i;
    for (i = 0; i < n; ++i)
    {
        fprintf(f, "%d\n", p[i]);
    }
    fclose(f);
}

int generatePrimes(int *p, int *size, int bound)
{
    assert(NULL == p && bound > 1);
    if (!primes)
    {
        primeSize = 1;
        primes = (int *)initArray(primeSize, sizeof(int));
        primes[primeCount++] = 2;
    }

    p = (int *)initArray(primeSize, sizeof(int));
    memcpy(p, primes, sizeof(int)*primeCount);
    *size = primeSize;
    return primeCount;
}

int isPrime(int x)
{
    if (!primes)
    {
        primeSize = 1;
        primes = (int *)initArray(primeSize, sizeof(int));
        primes[primeCount++] = 2;
    }
    if (x <= 1) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0;
    int i;
    for (i = 0; i < primeCount && primes[i] * primes[i] <= x; ++i)
    {
        if (x % primes[i] == 0)
        {
            if (x == primes[i])
            {
                return 1;
            }
            return 0;
        }
    }
    if (x > primes[primeCount - 1])
    {
        if (primeCount == primeSize)
        {
            primes = (int *)expandArray(primes, primeSize, sizeof(int), 1);
            primeSize *= 2;
        }
        primes[primeCount++] = x;
    }
    return 1;
}

char* intToStr(int x)
{
    char *str = (char *)malloc(sizeof(char)*(numDigits(x)+1));
    sprintf(str, "%d", x);
    return str;
}

int numDigits(int x)
{
    int digits = 0;
    while (x)
    {
        ++digits;
        x /= 10;
    }
    return digits;
}

