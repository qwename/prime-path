#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int isPrime(int n);
void deleteArray(int **a, int n);
int writeArrayToFile(const int **a, const char *name, int pad, int x, int y);

int main(int argc, char *argv[])
{
    const int start = 1;
    const int end = 100;
    int max_x = 1;
    int max_y = 1;
    int n = start, x = (max_x - 1) / 2, y = (max_y - 1)/ 2;
    // 0 = R; 1 = U; 2 = L; 3 = D
    int direction = 0;

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

    while (n <= end)
    {
        // printf("n = %d\n", n);
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
        if (isPrime(n))
        {
       		p[x][y] = n;
            ++direction;
        }
		else
		{
		}
        if (direction > 3)
        {
            direction = 0;
        }
        //assert(x-1 >= 0 && x < max_x);
        //assert(y-1 >= 0 && y < max_y);
        switch (direction)
        {
        case 0:
            {
                ++x;
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
        case 1:
            {
                --y;
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
        case 2:
            {
                --x;
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
        case 3:
            {
                ++y;
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

    int temp = n, pad = 0;
    while (temp)
    {
        temp /= 10;
        ++pad;
    }
    if (writeArrayToFile(p, "diagram.txt", pad, max_x, max_y))
    {
        printf("Done writing to file!\n");
    }
/*
    for (i = 0; i < ow_index; i += 2)
    {
        //if (isPrime(overwritten[i]))
        printf("%d %d; ", overwritten[i], overwritten[i + 1]);
    }
    printf("\n");
*/
    deleteArray(p, max_x);

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

int isPrime(int n)
{
    assert(n >= 0);
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    int x;
    for (x = 3; x * x <= n; x += 2)
    {
        if (n % x == 0)
        {
            return 0;
        }
    }
    return 1;
}
