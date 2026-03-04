#include <iostream>
#include <vector>
#include <ctime>
#include <stdint.h>

#define ROTL(d, lrot) ((d << (lrot)) | (d >> (64 - (lrot))))
#define SIZE 16

struct D4IHA {

    static uint64_t romu_mix(uint64_t seed) {
        uint64_t xState = seed;
        uint64_t yState = seed ^ 0x9e3779b97f4a7c15ULL;

        for (int i = 0; i < 4; i++) {
            uint64_t xp = xState;
            xState = 15241094284759029579u * yState;
            yState = yState - xp;
            yState = ROTL(yState, 27);
        }
        return xState;
    }

    static uint64_t get_hash(const uint8_t grid[SIZE][SIZE]) {
        uint64_t final_hash = 0;

        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                if (grid[y][x]) {

                    int cx = 2 * x - (SIZE - 1);
                    int cy = 2 * y - (SIZE - 1);

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
};

int main() {
    srand(time(NULL));
    const int TRIALS = 1000000;
    const int AVALANCHE_SAMPLES = 1000000;
    uint8_t soup[SIZE][SIZE];

    double avalanche_total = 0;
    for (int i = 0; i < AVALANCHE_SAMPLES; i++) {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++) soup[r][c] = rand() % 2;

        uint64_t hash_orig = D4IHA::get_hash(soup);

        soup[rand() % SIZE][rand() % SIZE] ^= 1;

        uint64_t hash_flipped = D4IHA::get_hash(soup);
        avalanche_total += __builtin_popcountll(hash_orig ^ hash_flipped);
    }
    printf("Avalanche Test (%d samples): Avg bit change: %.2f bits\n", AVALANCHE_SAMPLES, avalanche_total / AVALANCHE_SAMPLES);

    clock_t start = clock();
    uint64_t prev_h = 0;
    double total_bit_diff = 0;

    for (int i = 0; i < TRIALS; i++) {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++) soup[r][c] = rand() % 2;

        uint64_t h = D4IHA::get_hash(soup);
        if (i > 0) total_bit_diff += __builtin_popcountll(h ^ prev_h);
        prev_h = h;
    }
    double time_taken = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("Speed: %.2f seconds for 1M hashes (%.2f ns/hash)\n", time_taken, (time_taken / TRIALS) * 1e9);
    printf("General Entropy: Avg bit change: %.2f bits\n", total_bit_diff / (TRIALS - 1));

    return 0;
}