#pragma once
#include "TapeInterface.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <format>
#include <filesystem>

using std::chrono::milliseconds;

/**
 * @brief Emulate tape behaviour using a file.
 * File structure:
 * 0-4 bytes: tape size
 * 4-8 bytes: first value
 * ...
 * 4*size-4*(size+1): last value
*/

class TapeEmulator :
    public TapeInterface
{
private:
    TapeEmulator& operator=(const TapeEmulator& other) = delete;
    TapeEmulator(const TapeEmulator&) = delete;
public: 
    enum Emulation {
        Disabled,
        EmulateTimeStatistics,
        Full
    };
protected:
    bool initialized;

    std::filesystem::path filepath;
    std::fstream tape;
    std::streampos filepos;
    

    /**
     * @brief Emulation options:
     *  Disabled              - no time emulation;
     *  EmulateTimeStatistics - use time counters, but don't actually slow the program;
     *  Full                  - full emulation, the program is slowed by tape operations.
    */
    Emulation mode;


    enum TimeType {
        Read,
        Write,
        Shift
    };

    milliseconds readTime;
    milliseconds writeTime;
    milliseconds shiftTime;
    
    inline void emulateTime(milliseconds time);
    void emulateOperation(TimeType type);

    /**
     * @brief emulated time counter
    */
    milliseconds elapsed;
    
    void failsafe() const;

public:

    TapeEmulator()
    : TapeInterface(0, 0), initialized(false), mode(TapeEmulator::Disabled),
        elapsed(0), readTime(0), writeTime(0), shiftTime(0) {};

    ~TapeEmulator();

    // Initialization
    bool initialize(Emulation Mode, std::filesystem::path FilePath,
        milliseconds ReadTime, milliseconds WriteTime, milliseconds ShiftTime);
    bool isInitialized() const;

    // Time statistics
    milliseconds getElapsed() const;

    // Size
    virtual size_t size() const;

    // Position 
    virtual size_t position() const;
    virtual void setPosition(size_t Pos);
    virtual void shiftForward();
    virtual void shiftBackward();

    // Read/Write
    virtual void write(int32_t value);
    virtual int32_t read();

    // Create/Read tape files statically
    static bool createFile(std::filesystem::path& filePath, std::vector<int32_t>& data);
    static std::vector<int32_t> readFile(std::filesystem::path& filePath);
};

