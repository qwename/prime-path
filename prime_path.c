#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "print.h"

struct Coord {
    int x, y;
};

int* getTurns(int start, int end, int *pCount, int *s_w, int *s_h, int *s_x, int *s_y);
void getBoundaries(const int **a, int x, int y, struct Coord *topLeft, struct Coord *bottomRight);
int numDigits(int x);
char* intToStr(int x);
char* catStrs(const char *a, const char *b, const char *c);
// read primes from file "name" into array "p"
// returns number of prime numbers read, output array size to "size"
int readPrimes(int ** const p, int *size, const char *name);
void writePrimes(const int *p, int n, const char *name);
int* generatePrimes(int *num, int *size, int bound);
int isPrime(int x);
void* initArray(size_t num, size_t size);
// Expand array by a factor of 2
void* expandArray(void *a, size_t num, size_t size, int right);
void deleteArray(int **a, int n);
int writeArrayToFile(const int **a, const char *name, int pad, int x, int y);

static int primeCount = 0, primeSize = 0;
static int *primes = NULL;

int main(int argc, char *argv[])
{
    int start = 1, end = 100;
    const char *title = "Prime Path";
    // 4:3, 640x360 for 16:9
    int s_w = 640, s_h = 480;
    int s_x = s_w / 2, s_y = s_h / 2;
    switch (argc)
    {
    case 7:
    case 6:
        {
            primeCount = readPrimes(&primes, &primeSize, argv[5]);
        }
    case 5:
        {
            end = atoi(argv[4]);
        }
    case 4:
        {
            start = atoi(argv[3]);
        }
    case 3:
        {
            s_w = atoi(argv[1]);
            s_h = atoi(argv[2]);
        }
    case 1:
        {
            break;
        }
    case 2:
        {
            fprintf(stderr, "Not enough arguments!\n");
            return 0;
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
    if (s_h < 1)
    {
        s_h = 60;
    }
    if (s_w < 1)
    {
        s_w = s_h / 3 * 4;
    }

    int w, h;
    int pCount = 0, turn = 0;
    printf("Getting turns...");
    int *turns = getTurns(start, end, &pCount, &w, &h, &s_x, &s_y);
    printf("done!\n");
    if (w > s_w)
    {
        s_w = w;
    }
    if (h > s_h)
    {
        s_h = h;
    }

    Uint32 yellow, white;
    SDL_Surface *s = NULL, *sP = NULL;
    if (InitSDL())
    {
        s = InitScreen(s_w, s_h, title);
        yellow = SDL_MapRGB(s->format, 0xFF, 0xFF, 0x00);
        white = SDL_MapRGB(s->format, 0xFF, 0xFF, 0xFF);
    }

    int max_x = 1;
    int max_y = 1;
    int x = (max_x - 1) / 2, y = (max_y - 1)/ 2;
    struct Coord origin = { x, y };
    // R, UR, U, UL, L, DL, D, DR
    const int rotateTotal = 8;
    const struct Coord directions[] = {
                    { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
                    { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
    int current = 0;
    struct Coord translate = directions[current];

    /* Initialize array */
    int **p = (int **)initArray(max_x, sizeof(int *));
    int i;
    for (i = 0; i < max_x; ++i)
    {
        p[i] = (int *)initArray(max_y, sizeof(int));
    }
    printf("Initialized 2D array\n");

    int ow_size = 2, ow_index = 0;
    int *overwritten = (int *)initArray(ow_size, sizeof(int));

    int n = start;
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
            if (overwritten[ow_index - 1] == n)
            {
                printPoint(s, s_x, s_y, yellow);
            }
            else
            {
                printPoint(s, s_x, s_y, white);
            }
        }
        if (n == turns[turn])
        {
            ++turn;
            ++current;
            if (rotateTotal == current)
            {
                current = 0;
            }
            translate = directions[current];
        }

        x += translate.x;
        s_x += translate.x;
        if (x < 0 || x >= max_x)
        {
            int right = (1 == translate.x);
            p = (int **)expandArray(p, max_x, sizeof(int *), right);
            for (i = 0; i < max_x; ++i)
            {
                p[i + max_x*right] = (int *)initArray(max_y, sizeof(int));
            }
            x = max_x - !right;
            origin.x = origin.x + (!right)*max_x;
            max_x *= 2;
        }

        // Minus since y coordinates grow downwards
        y -= translate.y;
        s_y -= translate.y;
        if (y < 0 || y >= max_y)
        {
            int down = (-1 == translate.y);
            for (i = 0; i < max_x; ++i)
            {
                p[i] = (int *)expandArray(p[i], max_y, sizeof(int), down);
            }
            y = max_y - !down;
            origin.y = origin.y + (!down)*max_y;
            max_y *= 2;
        }
        ++n;
    }

    struct Coord tl, br;
    getBoundaries((const int **)p, max_x, max_y, &tl, &br);
    printf("Filled array, columns: %d rows: %d\n", br.x - tl.x + 1, br.y - tl.y + 1);
    printf("Origin: (%d, %d)\n", origin.x - tl.x, origin.y - tl.y);

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

    char *widthS = intToStr(s_w), *heightS = intToStr(s_h);
    char *dimS = catStrs(widthS, "x", heightS);
    free(widthS); free(heightS);
    char *startS = intToStr(start), *endS = intToStr(end);
    char *rangeS = catStrs(startS, "-", endS);
    free(startS); free(endS);
    char *fileName = catStrs(dimS, "_", rangeS);
    free(dimS); free(rangeS);
    const char *fileExt = "bmp";
    char *bmpName = catStrs(fileName, ".", fileExt);
    free(fileName);
    if (0 == SDL_SaveBMP(s, bmpName))
    {
        printf("Saved screen to %s\n", bmpName);
    }
    free(bmpName);

    delay(1000);
    FreeScreen(s);
    QuitSDL();
    printf("Quitted SDL\n");

    if (7 == argc)
    {
        writePrimes((const int *)primes, pCount, argv[6]);
        printf("Wrote %d primes to '%s'\n", pCount, argv[6]);
    }

    return 0;
}

int* getTurns(int start, int end, int *pCount, int *s_w, int *s_h, int *s_x, int *s_y)
{
    int max_x = 1;
    int max_y = 1;
    int x = (max_x - 1) / 2, y = (max_y - 1)/ 2;
    struct Coord origin = { x, y };
    // R, UR, U, UL, L, DL, D, DR
    const int rotateTotal = 8;
    const struct Coord directions[] = {
                    { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
                    { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
    int current = 0;
    struct Coord translate = directions[current];

    /* Initialize array */
    int **p = (int **)initArray(max_x, sizeof(int *));
    int i;
    for (i = 0; i < max_x; ++i)
    {
        p[i] = (int *)initArray(max_y, sizeof(int));
    }

    int n = start;
    while (n <= end)
    {
        p[x][y] = n;
        if (isPrime(n))
        {
            ++*pCount;
            ++current;
            if (rotateTotal == current)
            {
                current = 0;
            }
            translate = directions[current];
        }

        x += translate.x;
        if (x < 0 || x >= max_x)
        {
            int right = (1 == translate.x);
            p = (int **)expandArray(p, max_x, sizeof(int *), right);
            for (i = 0; i < max_x; ++i)
            {
                p[i + max_x*right] = (int *)initArray(max_y, sizeof(int));
            }
            x = max_x - !right;
            origin.x = origin.x + (!right)*max_x;
            max_x *= 2;
        }

        // Minus since y coordinates grow downwards
        y -= translate.y;
        if (y < 0 || y >= max_y)
        {
            int down = (-1 == translate.y);
            for (i = 0; i < max_x; ++i)
            {
                p[i] = (int *)expandArray(p[i], max_y, sizeof(int), down);
            }
            y = max_y - !down;
            origin.y = origin.y + (!down)*max_y;
            max_y *= 2;
        }
        ++n;
    }

    struct Coord tl, br;
    getBoundaries((const int **)p, max_x, max_y, &tl, &br);
    *s_w = br.x - tl.x + 1;
    *s_h = br.y - tl.y + 1;
    *s_x = origin.x - tl.x;
    *s_y = origin.y - tl.y;

    deleteArray(p, max_x);
    return generatePrimes(pCount, NULL, end);
}

void getBoundaries(const int **a, int x, int y, struct Coord *topLeft, struct Coord *bottomRight)
{
    assert(a && x >= 1 && y >= 1);
    int i, j, first_x = x, first_y = y, last_y = 0, last_x = 0;
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
                if (i > last_x)
                {
                    last_x = i;
                }
            }
            ++i;
        }
        i = 0;
        ++j;
    }
    topLeft->x = first_x; topLeft->y = first_y;
    bottomRight->x = last_x; bottomRight->y = last_y;
}

int writeArrayToFile(const int **a, const char *name, int pad, int x, int y)
{
    assert(a && x >= 1 && y >= 1);
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

int* generatePrimes(int *num, int *size, int bound)
{
    assert(num && bound > 1);
    if (!primes)
    {
        primeSize = 1;
        primes = (int *)initArray(primeSize, sizeof(int));
        primes[primeCount++] = 2;
    }

    int *p = (int *)initArray(primeSize, sizeof(int));
    memcpy(p, primes, sizeof(int)*primeCount);
    if (size)
    {
        *size = primeSize;
    }
    *num = primeCount;
    return p;
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
    assert(str);
    sprintf(str, "%d", x);
    return str;
}

char* catStrs(const char *a, const char *b, const char *c)
{
    assert(a && b && c);
    int aLen = strlen(a), bLen = strlen(b), cLen = strlen(c);
    int destLen = aLen + bLen + cLen + 1;
    char *dest = (char *)malloc(sizeof(char) * destLen);
    strcpy(strcpy(strcpy(dest, a) + aLen, b) + bLen, c);
    return dest;
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

