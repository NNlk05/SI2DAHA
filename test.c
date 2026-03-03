#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROTL(d, lrot) ((d << (lrot)) | (d >> (64 - (lrot))))
#define SIZE 16

uint64_t romu_mix(uint64_t seed)
{
    uint64_t xState = seed;
    uint64_t yState = seed ^ 0x9e3779b97f4a7c15ULL;
    for (int i = 0; i < 4; i++)
    {
        uint64_t xp = xState;
        xState = 15241094284759029579u * yState;
        yState = yState - xp;
        yState = ROTL(yState, 27);
    }
    return xState;
}

uint64_t get_hash(uint8_t grid[SIZE][SIZE])
{
    uint64_t final_hash = 0;
    int W = SIZE, H = SIZE;
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            if (grid[y][x])
            {
                int cx = 2 * x - (W - 1);
                int cy = 2 * y - (H - 1);
                int ax = (cx < 0) ? -cx : cx;
                int ay = (cy < 0) ? -cy : cy;
                uint64_t u = (ax < ay) ? (uint64_t)ax : (uint64_t)ay;
                uint64_t v = (ax < ay) ? (uint64_t)ay : (uint64_t)ax;
                final_hash ^= romu_mix((u << 32) | (v & 0xFFFFFFFF));
            }
        }
    }
    return romu_mix(final_hash);
}

void rotate(uint8_t in[SIZE][SIZE], uint8_t out[SIZE][SIZE])
{
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            out[c][SIZE - 1 - r] = in[r][c];
}

void flip(uint8_t in[SIZE][SIZE], uint8_t out[SIZE][SIZE])
{
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            out[r][SIZE - 1 - c] = in[r][c];
}

int count_set_bits(uint64_t n)
{
    int count = 0;
    while (n)
    {
        n &= (n - 1);
        count++;
    }
    return count;
}

int main()
{
    srand(time(NULL));
    const int TRIALS = 1000000;
    uint8_t soup[SIZE][SIZE];
    uint8_t temp[SIZE][SIZE];
    uint8_t current[SIZE][SIZE];

    printf("Starting Test Suite (1,000,000 Soups)...\n\n");

    int symmetry_passed = 1;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            soup[r][c] = rand() % 2;

    uint64_t base_h = get_hash(soup);
    memcpy(current, soup, SIZE * SIZE);

    for (int f = 0; f < 2; f++)
    {
        for (int r = 0; r < 4; r++)
        {
            if (get_hash(current) != base_h)
                symmetry_passed = 0;
            rotate(current, temp);
            memcpy(current, temp, SIZE * SIZE);
        }
        flip(current, temp);
        memcpy(current, temp, SIZE * SIZE);
    }
    printf("Requirement [Symmetry]: %s\n", symmetry_passed ? "PASSED" : "FAILED");

    clock_t start = clock();
    double total_bit_diff = 0;
    uint64_t prev_h = base_h;

    for (int i = 0; i < TRIALS; i++)
    {

        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                soup[r][c] = rand() % 2;

        uint64_t h = get_hash(soup);

        total_bit_diff += count_set_bits(h ^ prev_h);
        prev_h = h;
    }
    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Requirement [Speed]: %.2f seconds for 1M hashes (%.2f ns/hash)\n",
           time_taken, (time_taken / TRIALS) * 1e9);
    printf("Requirement [Entropy]: Avg bit change: %.2f bits (Ideal: 32.0)\n",
           total_bit_diff / TRIALS);

    return 0;
}