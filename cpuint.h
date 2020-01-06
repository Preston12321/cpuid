#ifndef CPUINT_H
#define CPUINT_H

#include <cstdint>
#include <string>

#pragma pack(push, 1) // Don't pad struct
struct CPUID_DATA
{
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
};
#pragma pack(pop)

extern "C" bool CPUIDInstructionSupported();
extern "C" void getCPUIDData(uint32_t eax, CPUID_DATA * data);

void clearStruct(CPUID_DATA * data);
std::string stringFromRaw(char * rawData, uint64_t maxLength);
uint32_t intFromBitRange(uint32_t num, uint8_t lowBit, uint8_t highBit);
bool flagAtIndex(uint32_t num, uint8_t bitIndex);

#endif /* CPUINT_H */
