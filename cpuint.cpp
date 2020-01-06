#include <sstream>

#include "cpuint.h"

using namespace std;

void clearStruct(CPUID_DATA * data) {
    data->EAX = 0;
    data->EBX = 0;
    data->ECX = 0;
    data->EDX = 0;
}

string stringFromRaw(char * rawData, uint64_t maxLength) {
    string result;
    uint64_t index = 0;
    while (index < maxLength && rawData[index] != 0) {
        result.append(1, rawData[index]);
        index++;
    }
    return result;
}

uint32_t intFromBitRange(uint32_t num, uint8_t lowBit, uint8_t highBit) {
    uint8_t maxBit = (8 * sizeof(uint32_t)) - 1;
    if (lowBit > maxBit || highBit > maxBit || lowBit > highBit) return num;

    uint8_t shift = (maxBit - highBit);
    uint32_t result = num << shift;
    return result >> (shift + lowBit);
}

bool flagAtIndex(uint32_t num, uint8_t bitIndex) {
    uint32_t result = num >> bitIndex;
    result &= 1U;
    return result;
}
