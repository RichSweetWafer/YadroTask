#pragma once
#include <cstdint>

class TapeInterface
{
protected:
	TapeInterface(size_t pos, size_t size)
		: _pos(pos), _size(size) {};
	size_t _pos;
	size_t _size;
public:

	/**
	 * @brief Return tape size
	 * @return position: size_t
	*/
	virtual size_t size() const = 0;

	/**
	 * @brief Return current head position
	 * @return position: size_t
	*/
	virtual size_t position() const = 0;
	/**
	 * @brief Set head position to pos
	 * @param pos The position to be set size_t
	*/
	virtual void setPosition(size_t pos) = 0;
	virtual void shiftForward() = 0;
	virtual void shiftBackward() = 0;

	/**
	 * @brief Write 4 bytes of data to current position
	 * @param value A 4 byte integer
	*/
	virtual void write(int32_t value) = 0;
	/**
	 * @brief Read 4 bytes from current position
	 * @return A 4 byte integer
	*/
	virtual int32_t read() = 0;
};
