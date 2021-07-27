#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct bit_vector_impl bit_vector;

/**
 * @return A new, empty bit vector.
 */
bit_vector* bit_vector_new();

/**
 * Releases the memory used by a bit vector.
 *
 * @param vec Vector to free.
 */
void bit_vector_free(bit_vector* vec);

/**
 * Appends a bit to the vector, resizing if needed.
 *
 * @param vec Vector to append data to.
 * @param bit The bit to append.
 */
void bit_vector_push(bit_vector* vec, bool bit);

/**
 * Returns how many bits the given vector contains.
 *
 * @param vec Vector to get the size of.
 *
 * @return How many bits have been written to the vector.
 */
size_t bit_vector_size(const bit_vector* vec);

/**
 * Returns the bit at a given index of the vector.
 *
 * If the position is out of bounds, this function panics.
 *
 * @param vec Vector to read data from.
 * @param pos Position of the wanted bit.
 *
 * @return The value of the bit at the given position.
 */
bool bit_vector_get(const bit_vector* vec, size_t pos);

/**
 * Sets the bit at a given position.
 *
 * If the position is out of bounds, this function panics.
 *
 * @param vec   Vector to write data to.
 * @param pos   Position of the wanted bit.
 * @param value Value to write.
 */
void bit_vector_set(bit_vector* vec, size_t pos, bool value);

/**
 * Removes a range of bits from the vector, shifting further elements
 * into the position where bits were removed, such as:
 *
 * ```
 * vec = [111110000011111]
 * bit_vector_remove_range(vec, 5, 5)
 * vec == [1111111111]
 * ```
 *
 * If the range to be removed falls outside the vector, this function panics.
 *
 * @param vec    Vector to remove bits from.
 * @param start  Index to start removing bits at.
 * @param amount How many bits to remove.
 */
void bit_vector_remove_range(bit_vector* vec, size_t start, size_t amount);

/**
 * Clears the vector, removing all bits from it.
 *
 * @param vec Vector to clear.
 */
void bit_vector_clear(bit_vector* vec);
