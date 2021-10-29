#include "utils.h"

#include <cstdlib>
std::string randomString(uint8_t seed, uint8_t length) {
    srand(seed);
    const char* letters[40] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
                               "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
                               "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    std::string randString = "";
    for (int i = 0; i < length; i++) {
        randString.append(letters[rand() % 10]);
    }
    return randString;
}
