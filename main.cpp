#include <iostream>
#include <string>
#include <stdint.h>

#define ROTL(d,lrot) ((d<<(lrot)) | (d>>(64-(lrot))))

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

    static uint64_t get_orbit_weight(int x, int y, int W, int H) {

        int cx = 2 * x - (W - 1);
        int cy = 2 * y - (H - 1);

        int ax = (cx < 0) ? -cx : cx;
        int ay = (cy < 0) ? -cy : cy;

        uint64_t u = (ax < ay) ? (uint64_t)ax : (uint64_t)ay;
        uint64_t v = (ax < ay) ? (uint64_t)ay : (uint64_t)ax;

        return romu_mix((u << 32) | (v & 0xFFFFFFFF));
    }
};

int main() {
    std::string line;
    int W = 0, H = 0;
    uint64_t final_hash = 0;

    while (std::getline(std::cin, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == 'x') {
            size_t x_pos = line.find("x = ") + 4;
            size_t y_pos = line.find("y = ", x_pos) + 4;
            W = std::stoi(line.substr(x_pos));
            H = std::stoi(line.substr(y_pos));
            break;
        }
    }

    int curX = 0, curY = 0, count = 0;
    char c;
    while (std::cin >> c && c != '!') {
        if (c >= '0' && c <= '9') {
            count = count * 10 + (c - '0');
        } else {
            int run = (count == 0) ? 1 : count;
            if (c == 'o') { 
                for (int i = 0; i < run; ++i) {
                    final_hash ^= D4IHA::get_orbit_weight(curX + i, curY, W, H);
                }
                curX += run;
            } else if (c == 'b') { 
                curX += run;
            } else if (c == '$') { 
                curY += run;
                curX = 0;
            }
            count = 0;
        }
    }

    std::cout << std::hex << D4IHA::romu_mix(final_hash) << std::endl;

    return 0;
}