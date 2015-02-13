#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "print.h"

int numDigits(int x);
int readPrimes(int ** const primes, int *size, const char *name);
void writePrimes(const int *primes, int n, const char *name);
int generatePrimes(int *primes, int *size, int bound);
int isPrime(int x);
void deleteArray(int **a, int n);
int writeArrayToFile(const int **a, const char *name, int pad, int x, int y);

static int primeCount = 0, primeSize = 0;
static int *primes = NULL;

struct Coord {
    int x, y;
};

int main(int argc, char *argv[])
{
    int start = 2;
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
    if (start < 2)
    {
        start = 2;
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
    int **p = (int **)calloc(max_x, sizeof(int *));
    assert(p);
    int ow_size = 2, ow_index = 0;
    int *overwritten = (int *)calloc(ow_size, sizeof(int));

    int i, j;
    for (i = 0; i < max_x; ++i)
    {
        p[i] = (int *)calloc(max_y, sizeof(int));
        assert(p[i]);
    }

    SDL_Surface *s = NULL;
    int s_w = 2560, s_h = 1920;
    int s_x = s_w / 2, s_y = s_h / 2;
    if (InitSDL())
    {
        s = InitScreen(s_w, s_h);
    }
    while (n <= end)
    {
        if (0 != p[x][y])
        {
            overwritten[ow_index++] = p[x][y];
            overwritten[ow_index++] = n;
            if (ow_index >= ow_size)
            {
                ow_size *= 2;
                overwritten = (int *)realloc(overwritten, sizeof(int)*ow_size);
                for (i = ow_size / 2; i < ow_size; ++i)
                {
                    overwritten[i] = 0;
                }
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
                    max_x *= 2;
                    p = (int **)realloc(p, sizeof(int *)*max_x);
                    assert(p);
                    for (i = x; i < max_x; ++i)
                    {
                        p[i] = (int *)calloc(max_y, sizeof(int));
                        assert(p[i]);
                    }
                }
                break;
            }
        case -1:
            {
                --x; --s_x;
                if (x < 0)
                {
                    int **temp = (int **)calloc(max_x * 2, sizeof(int *));
                    assert(temp);
                    for (i = 0; i < max_x * 2; ++i)
                    {
                        temp[i] = (int *)calloc(max_y, sizeof(int));
                        assert(temp[i]);
                    }
                    for (i = 0; i < max_x; ++i)
                    {
                        memcpy(temp[i + max_x], p[i], sizeof(int)*max_y);
                    }
                    deleteArray(p, max_x);
                    p = temp;
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
                    int **temp = (int **)calloc(max_x, sizeof(int *));
                    assert(temp);
                    for (i = 0; i < max_x; ++i)
                    {
                        temp[i] = (int *)calloc(max_y * 2, sizeof(int));
                        assert(temp[i]);
                        for (j = 0; j < max_y; ++j)
                        {
                            temp[i][j] = 0;
                        }
                        memcpy((int *)&temp[i][max_y], p[i], sizeof(int)*max_y);
                    }
                    deleteArray(p, max_x);
                    p = temp;
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
                    max_y *= 2;
                    for (i = 0; i < max_x; ++i)
                    {
                        p[i] = (int *)realloc(p[i], sizeof(int)*max_y);
                        for (j = y; j < max_y; ++j)
                        {
                            p[i][j] = 0;
                        }
                    }
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
    char startS[numDigits(start)+1];
    sprintf(startS, "%d", start);
    char endS[numDigits(end)+1];
    sprintf(endS, "%d", end);
    int bmpNameLen = strlen(startS) + strlen(endS) + 
                    strlen(fileExt) + 2;
    char bmpName[bmpNameLen];
    strcpy(bmpName, startS);
    strcpy(bmpName + strlen(startS), "-");
    strcpy(bmpName + strlen(startS) + 1, endS);
    strcpy(bmpName + strlen(startS) + 1 + strlen(endS), fileExt);
    bmpName[bmpNameLen - 1] = '\0';
    if (0 == SDL_SaveBMP(s, bmpName))
    {
        printf("Saved screen to %s\n", bmpName);
    }
    delay(1000);
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
    int *last_x = (int *)calloc(y, sizeof(int));
    assert(last_x);
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

void deleteArray(int **a, int n)
{
    assert(a && n >= 1);
    while (--n >= 0)
    {
        free(a[n]);
    }
    free(a);
}

// read primes from file "name" into array "p"
// returns number of prime numbers read, output array size to "size"
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
    *p = (int *)calloc(pSize, sizeof(int));
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
        primes = (int *)calloc(primeSize, sizeof(int));
        primes[primeCount++] = 2;
    }

    p = (int *)calloc(primeSize, sizeof(int));
    memcpy(p, primes, sizeof(int)*primeCount);
    *size = primeSize;
    return primeCount;
}


// "p" is a sequence of "n" primes, all less than "x"
int isPrime(int x)
{
    if (!primes)
    {
        primeSize = 1;
        primes = (int *)calloc(primeSize, sizeof(int));
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
            primeSize *= 2;
            primes = (int *)realloc(primes, sizeof(int)*primeSize);
        }
        primes[primeCount++] = x;
    }
    return 1;
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

