#include "TapeEmulator.h"

/**
 * @brief If partial or full emulation is enabled, update time statistics. 
 * Full emulation uses thread::sleep_for, which can be inaccurate due to OS scheduling.
 * @param time milliseconds to emulate.
*/
inline void TapeEmulator::emulateTime(milliseconds time)
{
    switch (mode)
    {
    case TapeEmulator::EmulateTimeStatistics:
        elapsed += time;
        break;
    case TapeEmulator::Full:
        std::this_thread::sleep_for(time);
        elapsed += time;
        break;
    default:
        break;
    }

}

/**
 * @brief Emulate given operation using emulateTime and the corresponding duration.
 * @param type operation type
*/
void TapeEmulator::emulateOperation(TimeType type)
{
    switch (type)
    {
    case TapeEmulator::Read:
        emulateTime(readTime);
        break;
    case TapeEmulator::Write:
        emulateTime(writeTime);
        break;
    case TapeEmulator::Shift:
        emulateTime(shiftTime);
        break;
    default:
        break;
    }

}


void TapeEmulator::failsafe() const
{
    if (!initialized)
        throw std::exception("TapeEmulator is not initialized");
}

TapeEmulator::~TapeEmulator()
{
    if (tape.is_open())
        tape.close();
}

/**
 * @brief initialize emulator for further usage.
 * 
 * @param Mode Emulation mode.
 * @param FilePath Path to file.
 * 
 * @param ReadTime emulated read operation time.
 * @param WriteTime emulated write operation time.
 * @param ShiftTime emulated shift operation time.
 * 
 * @return success/fail.
*/
bool TapeEmulator::initialize(Emulation Mode, std::filesystem::path FilePath, milliseconds ReadTime, milliseconds WriteTime, milliseconds ShiftTime)
{
    initialized = false;
    if (tape.is_open())
        tape.close();
    
    _pos = 0;
    mode = Mode;
    
    filepath = FilePath;
    tape.open(filepath, tape.in | tape.out | tape.binary);
    
    readTime = ReadTime;
    writeTime = WriteTime;
    shiftTime = ShiftTime;
    
    if (!tape)
        throw std::runtime_error("Tape file could not be opened.");
    
    int32_t s = 0;
    tape.read(reinterpret_cast<char*>(&s), sizeof(int32_t));
    _size = s;
    if (!_size)
        throw std::invalid_argument("Tape length is zero.");
    tape.seekp(sizeof(int32_t), std::fstream::beg);
    filepos = tape.tellp();
    initialized = true;
        
    return initialized;
}

bool TapeEmulator::isInitialized() const
{
    return initialized;
}

milliseconds TapeEmulator::getElapsed() const
{
    failsafe();
    return elapsed;
}

size_t TapeEmulator::size() const
{
    failsafe();
    return _size;
}

size_t TapeEmulator::position() const
{
    failsafe();
    return _pos;
}

void TapeEmulator::setPosition(size_t Pos)
{
    failsafe();

    if (Pos >= _size) {
        std::string message = std::format("{} out of tape range {}", Pos, _size);
        throw std::out_of_range(message);
    }

    size_t diff = _pos > Pos ? _pos - Pos : Pos - _pos;
    for (size_t i = 0; i < diff; i++)
        emulateOperation(Shift);
    
    _pos = Pos;
    tape.seekp(4 * (_pos + 1), std::fstream::beg);
    filepos = tape.tellp();
}

void TapeEmulator::shiftForward()
{
    failsafe();
    if (_pos + 1 >= _size) {
        std::string message = std::format("{} out of tape range {}", _pos + 1, _size);
        throw std::out_of_range(message);
    }
    emulateOperation(Shift);
    _pos++;
    tape.seekp(4 * (_pos + 1), std::fstream::beg);
    filepos = tape.tellp();
}

void TapeEmulator::shiftBackward()
{
    failsafe();
    if (_pos == 0) {
        std::string message = std::format("{} out of tape range {}", -1, _size);
        throw std::out_of_range(message);
    }
    emulateOperation(Shift);
    _pos--;
    tape.seekp(4 * (_pos + 1), std::fstream::beg);
    filepos = tape.tellp();
}

void TapeEmulator::write(int32_t value)
{
    failsafe();
    emulateOperation(Write);
    tape.write(reinterpret_cast<char*>(&value), sizeof(int32_t));
    tape.seekp(filepos);
}

int32_t TapeEmulator::read()
{
    failsafe();
    emulateOperation(Read);
    int32_t value;
    tape.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
    tape.seekp(filepos);
    return value;
}

/**
 * @brief create file for emulation.
 * @param filePath location and name of the file.
 * @param data vector of int32_t values to be used;
 * @return success/fail.
*/
bool TapeEmulator::createFile(std::filesystem::path& filePath, std::vector<int32_t>& data)
{
    std::ofstream file(filePath, std::fstream::binary | std::fstream::trunc);
    if (!file)
        return false;
    int32_t length = data.size();
    file.write(reinterpret_cast<char*>(&length), sizeof(int32_t));
    for (int32_t val : data)
        file.write(reinterpret_cast<char*>(&val), sizeof(int32_t));
    file.close();

    return true;
}

/**
 * @brief read data from emulation file.
 * @param filePath location and name of the file.
 * @return Read values from file;
*/
std::vector<int32_t> TapeEmulator::readFile(std::filesystem::path& filePath)
{
    std::ifstream file(filePath, std::fstream::binary);
    std::vector<int32_t> vec;
    if (!file)
        return vec;

    int32_t length = 0;
    file.read(reinterpret_cast<char*>(&length), sizeof(int32_t));
    for (size_t i = 0; i < length; i++) {
        int32_t val = 0;
        file.read(reinterpret_cast<char*>(&val), sizeof(int32_t));
        vec.push_back(val);
    };
    file.close();

    return vec;
}
