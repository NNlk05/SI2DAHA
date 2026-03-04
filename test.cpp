#include <iostream>
#include <vector>
#include <ctime>
#include <stdint.h>
#include <iomanip>

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
    const int TEST_SAMPLES = 500000; 

    uint8_t soup[SIZE][SIZE];
    uint64_t bit_ones_count[64] = {0};
    double avalanche_total = 0;
    double entropy_total = 0;
    uint64_t prev_h = 0;

    clock_t start = clock();

    for (int i = 0; i < TEST_SAMPLES; i++) {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++) soup[r][c] = rand() % 2;

        uint64_t h1 = D4IHA::get_hash(soup);

        if (i > 0) entropy_total += __builtin_popcountll(h1 ^ prev_h);
        for (int b = 0; b < 64; b++) if ((h1 >> b) & 1) bit_ones_count[b]++;
        prev_h = h1;

        soup[rand() % SIZE][rand() % SIZE] ^= 1;
        uint64_t h2 = D4IHA::get_hash(soup);
        avalanche_total += __builtin_popcountll(h1 ^ h2);
    }

    double duration = (double)(clock() - start) / CLOCKS_PER_SEC;

    std::cout << "Avalanche: " << (avalanche_total / TEST_SAMPLES) << " bits\n";
    std::cout << "Speed: " << duration << "s (" << (duration / TEST_SAMPLES) * 1e9 << " ns/hash)\n";
    std::cout << "Entropy: " << (entropy_total / (TEST_SAMPLES - 1)) << " bits\n";

    std::cout << "\n--- Bit Chance Heatmap ---\n";
    for (int b = 63; b >= 0; b--) {
        double p = (bit_ones_count[b] * 100.0) / TEST_SAMPLES;
        std::cout << "Bit " << std::setw(2) << b << ": " << std::fixed << std::setprecision(2) << p << "% ";
        for (int j = 0; j < (int)(p / 2); j++) std::cout << "#";
        std::cout << "\n";
        if (b % 8 == 0 && b != 0) std::cout << "-------------------------------------------\n";
    }

    return 0;
}