#include <cstdio>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "cpuint.h"
#include "constants.h"

using namespace std;

template <typename T>
void printFeatureValue(string featureString, T value) {
    cout << left;

    cout << setw(FEATURE_NAME_WIDTH) << featureString << ": " << value << endl;

    cout << right;
}

template <typename T>
void printFeatureValueHex(string featureString, T value) {
    cout << hex << uppercase << left;

    cout << setw(FEATURE_NAME_WIDTH) << featureString << ": 0x";
    cout << setfill('0') << right;
    cout << setw(sizeof(value) * 2) << value << endl;

    cout << dec << nouppercase << setfill(' ');
}

void printSupported(uint32_t flagData, uint16_t flagIndex, string featureString) {
    // string value = flagAtIndex(flagData, flagIndex) ? "SUPPORTED" : "NOT SUPPORTED";
    string value = flagAtIndex(flagData, flagIndex) ? "true" : "false";
    printFeatureValue(featureString, value);
}

void printSupported(string featureString, bool supported) {
    // string value = supported ? "SUPPORTED" : "NOT SUPPORTED";
    string value = supported ? "true" : "false";
    printFeatureValue(featureString, value);
}

int main() {
    auto data = new CPUID_DATA();

    bool supported = CPUIDInstructionSupported();
    printSupported("CPUID instruction", supported);

    if (!supported) {
        delete data;
        return 1;
    }


    //////////////////////////////////////////////////////////
    //                       EAX = 0                        //
    //////////////////////////////////////////////////////////

    getCPUIDData(0, data);

    uint32_t maxBasicInput = data->EAX;
    printFeatureValueHex("Basic CPUID Max Input", maxBasicInput);

    string manufacturerString;
    manufacturerString.reserve(12);
    manufacturerString.append(stringFromRaw(reinterpret_cast<char *>(&(data->EBX)), 4));
    manufacturerString.append(stringFromRaw(reinterpret_cast<char *>(&(data->EDX)), 4));
    manufacturerString.append(stringFromRaw(reinterpret_cast<char *>(&(data->ECX)), 4));

    printFeatureValue("Manufacturer ID String", manufacturerString);

    //////////////////////////////////////////////////////////
    //                   EAX = 80000000H                    //
    //////////////////////////////////////////////////////////

    clearStruct(data);
    getCPUIDData(0x80000000, data);

    uint32_t maxExtendedInput = data->EAX;

    bool brandStringSupported = maxExtendedInput >= 0x80000004;
    printSupported("Brand String Method", brandStringSupported);

    string brandString;
    brandString.reserve(48);
    if (brandStringSupported) {
        for (uint32_t i = 0x80000002; i <= 0x80000004; i++) {
            clearStruct(data);
            getCPUIDData(i, data);

            auto one = stringFromRaw(reinterpret_cast<char *>(&(data->EAX)), 4);
            auto two = stringFromRaw(reinterpret_cast<char *>(&(data->EBX)), 4);
            auto three = stringFromRaw(reinterpret_cast<char *>(&(data->ECX)), 4);
            auto four = stringFromRaw(reinterpret_cast<char *>(&(data->EDX)), 4);

            brandString.append(one).append(two).append(three).append(four);
        }
        printFeatureValue("CPU Brand String", brandString);
    }

    //////////////////////////////////////////////////////////
    //                       EAX = 1                        //
    //////////////////////////////////////////////////////////

    cout << endl << "-------- Section One --------" << endl;

    clearStruct(data);
    getCPUIDData(1, data);

    uint32_t signature = data->EAX;

    uint16_t steppingID = intFromBitRange(data->EAX, 0, 3),
                   model = intFromBitRange(data->EAX, 4, 7),
                   procType = intFromBitRange(data->EAX, 12, 13),
                   familyID = intFromBitRange(data->EAX, 8, 11);

    if (familyID == 0x0FU) {
        familyID |= intFromBitRange(data->EAX, 20, 27) << 4;
    }

    if (familyID == 0x0FU || familyID == 0x06) {
        model |= intFromBitRange(data->EAX, 16, 19) << 4;
    }

    printFeatureValueHex("Signature", signature);

    printFeatureValueHex("Family ID", familyID);
    printFeatureValueHex("Model #", model);
    printFeatureValueHex("Stepping ID", steppingID);

    printFeatureValue("Processor Type", PROC_TYPES[procType]);

    uint32_t brandIndex = intFromBitRange(data->EBX, 0, 7),
                 CLFLUSHCacheSize = 8 * intFromBitRange(data->EBX, 8, 15),
                 APICLocalID = intFromBitRange(data->EBX, 23, 31);

    bool indexSupported = brandIndex > 0;
    printSupported("Brand Index Method", indexSupported);

    if (indexSupported) {
        if (signature == 0x00000F13U) {
            if (brandIndex == 11) {
                brandIndex = 12;
            }
            else if (brandIndex == 14) {
                brandIndex = 11;
            }
        }
        else if (signature == 0x000006B1U && brandIndex == 3) {
            brandIndex = 1;
        }

        brandString = BRAND_TABLE.at(brandIndex);
        printFeatureValue("CPU Brand String", brandString);
    }

    printFeatureValue("CLFLUSH Cache Line Size", CLFLUSHCacheSize);
    printFeatureValueHex("Local APIC ID", APICLocalID);

    printSupported(data->ECX, 0, "SSE3");
    printSupported(data->ECX, 1, "PCLMULQDQ");
    printSupported(data->ECX, 2, "DTES64");
    printSupported(data->ECX, 3, "MONITOR");
    printSupported(data->ECX, 4, "DS-CPL");
    printSupported(data->ECX, 5, "VMX");
    printSupported(data->ECX, 6, "SMX");
    printSupported(data->ECX, 7, "EIST");
    printSupported(data->ECX, 8, "TM2");
    printSupported(data->ECX, 9, "SSSE3");
    printSupported(data->ECX, 10, "CNXT-ID");
    printSupported(data->ECX, 11, "SDBG");
    printSupported(data->ECX, 12, "FMA");
    printSupported(data->ECX, 13, "CMPXCHG16B");
    printSupported(data->ECX, 14, "xTPR Update Control");
    printSupported(data->ECX, 15, "PDCM");
    printSupported(data->ECX, 17, "PCID");
    printSupported(data->ECX, 18, "DCA");
    printSupported(data->ECX, 19, "SSE4.1");
    printSupported(data->ECX, 20, "SSE4.2");
    printSupported(data->ECX, 21, "x2APIC");
    printSupported(data->ECX, 22, "MOVBE");
    printSupported(data->ECX, 23, "POPCNT");
    printSupported(data->ECX, 24, "TSC-Deadline");
    printSupported(data->ECX, 25, "AESNI");
    printSupported(data->ECX, 26, "XSAVE");
    printSupported(data->ECX, 27, "OSXSAVE");
    printSupported(data->ECX, 28, "AVX");
    printSupported(data->ECX, 29, "F16C");
    printSupported(data->ECX, 30, "RDRAND");

    printSupported(data->EDX, 0, "FPU");
    printSupported(data->EDX, 1, "VME");
    printSupported(data->EDX, 2, "DE");
    printSupported(data->EDX, 3, "PSE");
    printSupported(data->EDX, 4, "TSC");
    printSupported(data->EDX, 5, "MSR");
    printSupported(data->EDX, 6, "PAE");
    printSupported(data->EDX, 7, "MCE");
    printSupported(data->EDX, 8, "CX8");
    printSupported(data->EDX, 9, "APIC");
    printSupported(data->EDX, 11, "SEP");
    printSupported(data->EDX, 12, "MTRR");
    printSupported(data->EDX, 13, "PGR");
    printSupported(data->EDX, 14, "MCA");
    printSupported(data->EDX, 15, "CMOV");
    printSupported(data->EDX, 16, "PAT");
    printSupported(data->EDX, 17, "PSE-36");

    bool serialNumberSupported = intFromBitRange(data->EDX, 18, 18) == 1;
    printSupported("PSN", serialNumberSupported);

    printSupported(data->EDX, 19, "CLFSH");
    printSupported(data->EDX, 21, "DS");
    printSupported(data->EDX, 22, "ACPI");
    printSupported(data->EDX, 23, "MMX");
    printSupported(data->EDX, 24, "FXSR");
    printSupported(data->EDX, 25, "SSE");
    printSupported(data->EDX, 26, "SSE2");
    printSupported(data->EDX, 27, "SS");
    printSupported(data->EDX, 28, "HTT");
    printSupported(data->EDX, 29, "TM");
    printSupported(data->EDX, 31, "PBE");

    //////////////////////////////////////////////////////////
    //                       EAX = 2                        //
    //////////////////////////////////////////////////////////

    cout << endl << "-------- Section Two --------" << endl;

    clearStruct(data);
    getCPUIDData(2, data);

    cout << "TLB/Cache/Prefetch Descriptors:" << endl;

    uint8_t count = data->EAX;

    while (true) {
        uint32_t registers[] = {data->EAX, data->EBX, data->ECX, data->EDX};
        for (int r = 0; r < 4; r++) {
            uint32_t reg = registers[r];

            // If register is eax...
            if (r == 0) {
                // Clear the least significant byte to keep
                // from interpreting it as a descriptor
                reg &= 0xFFFFFF00;
            }

            // If most significant bit is 1; register is invalid
            if (reg & 0x80000000 != 0) continue;

            for (int i = 0; i <= 24; i += 8) {
                uint8_t byte = intFromBitRange(reg, i, i + 7);

                if (byte == 0) continue;

                try {
                    auto descriptor = CACHE_DESCRIPTORS.at(byte);

                    cout << descriptor << endl;
                }
                catch (out_of_range) {
                    continue;
                }
            }
        }

        count--;

        if (count <= 1) break;

        clearStruct(data);
        getCPUIDData(2, data);
    }

    //////////////////////////////////////////////////////////
    //                       EAX = 3                        //
    //////////////////////////////////////////////////////////

    cout << endl << "------- Section Three -------" << endl;

    if (serialNumberSupported) {
        clearStruct(data);
        getCPUIDData(3, data);

        uint32_t mid = data->EDX;
        uint32_t low = data->ECX;

        char highString[9];
        char midString[9];
        char lowString[9];
        snprintf(highString, 9, "%08X", signature);
        snprintf(midString, 9, "%08X", mid);
        snprintf(lowString, 9, "%08X", low);

        string serial;

        serial.append(stringFromRaw(highString, 4));
        serial.append(1, '-');
        serial.append(stringFromRaw(highString + 4, 4));
        serial.append(1, '-');
        serial.append(stringFromRaw(midString, 4));
        serial.append(1, '-');
        serial.append(stringFromRaw(midString + 4, 4));
        serial.append(1, '-');
        serial.append(stringFromRaw(lowString, 4));
        serial.append(1, '-');
        serial.append(stringFromRaw(lowString + 4, 4));

        printFeatureValue("Processor Serial Number", serial);
    }

    // for (int i = 1; i <= maxBasicInput; i++) {
    //     clearStruct(data);
    //     getCPUIDData(i, data);

    //     cout << dec << i << hex
    //          << "\tEAX:\t" << data->EAX
    //          << "\tEBX:\t" << data->EBX
    //          << "\tECX:\t" << data->ECX
    //          << "\tEDX:\t" << data->EDX
    //          << endl;
    // }

    delete data;
    return 0;
}
