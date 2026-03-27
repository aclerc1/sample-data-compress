/**
 * Sample Data Compress
 *
 * This sample provides the solution for a sample data compression challenge.
 *
 * See docs/challenge.pdf for the detailed problem description.
 * See docs/solution.pdf for a detailed explanation of the solution.
 *
 * Author: Andre Clerc
 * Created: 2022-03-23
 * Last modified: 2026-03-27
 */
#include <stdint.h>
#include <stdio.h>

/* Maximum count */
#define MAX_COUNT 127

/**
 * @brief Compress a given data buffer of bytes
 *
 * @param data_ptr pointer to the data buffer
 * @param data_size number of bytes to compress
 *
 * @return the size of the modified buffer (i.e. compressed)
 */
static int byte_compress(uint8_t *data_ptr, int data_size)
{

    /* Initialize local variables */
    int count = 0;                     // counter for repeated values
    int j = 0;                         // 'write' index for in-place array modifications
    uint8_t current_val = data_ptr[0]; // initialize to first value of buffer

    /**
     * Loop through buffer and modify in-place.
     * Handle repeated values by first writing a count then the value
     */
    for (int i = 0; i < data_size; i++)
    {
        if (current_val != data_ptr[i])
        {
            /* Save the compressed data */
            if (count > 1)
            {
                /**
                 * Assign the count to the buffer. The MSB lets us know that
                 * the value represents a count.
                 */
                data_ptr[j] = count | 0x80;
                data_ptr[j + 1] = current_val;
                j += 2; // increment 'write' index
            }
            else
            {
                /**
                 * Assign the value to the buffer.
                 */
                data_ptr[j] = current_val;
                j += 1; // increment 'write' index
            }

            /* Update the current value and reset the count */
            current_val = data_ptr[i];
            count = 1;
        }
        else
        {
            /**
             * If we have reached our specified maximum count, save the current
             * count and reset the count.
             */
            if (count > MAX_COUNT - 1)
            {
                data_ptr[j] = count | 0x80;
                data_ptr[j + 1] = current_val;
                j += 2; // increment 'write' index
                count = 1;
            }
            else
            {
                count++;
            }
        }
    }

    /* Save the last compressed data */
    if (count > 1)
    {
        /**
         * Assign the count to the buffer. The MSB lets us know that
         * the value represents a count.
         */
        data_ptr[j] = count | 0x80;
        data_ptr[j + 1] = current_val;
        j += 2; // increment 'write' index
    }
    else
    {
        /**
         * Assign the value to the buffer.
         */
        data_ptr[j] = current_val;
        j += 1;
    }

    /* Return the new size of the buffer */
    return j;
}

/**
 * @brief Decompress a given data buffer of bytest
 *
 * @note In-place operations require lots of element shifting.
 * Depending on the problem constraints, it may be more desirable
 * to create a new buffer and fill that instead.
 *
 * @param data_ptr pointer to the data buffer
 * @param data_size number of compressed bytes
 *
 * @return the size of the modified buffer (i.e. uncompressed)
 */
static int byte_decompress(uint8_t *data_ptr, int data_size)
{

    /* Initialize local variables */
    int i = 0;                // 'read' index for in-place array modifications
    int j = 0;                // 'write' index for in-place array modifications
    int last = data_size - 1; // index of last element

    /* Loop through the buffer */
    while (i < last)
    {
        /* If the MSB is set, the value in data_ptr is a COUNT */
        if ((data_ptr[i] & 0x80) == 0x80)
        {
            int count = data_ptr[i] & 0x7F; // extract the count (max is 127)
            int value = data_ptr[i + 1];    // extract the (repeated) data

            for (int k = last; k >= (i + 2); k--)
            {
                /* shift (count-2) places starting from index (i+2) */
                data_ptr[k + (count - 2)] = data_ptr[k];
            }
            last += (count - 2); // update last index

            int start = j;
            while (j < (start + count))
            {
                /* We have made space, so we can now insert the (repeated) data */
                data_ptr[j] = value;
                j++;
            }

            i += count; // increment the 'read' index
        }
        else
        {
            data_ptr[j] = data_ptr[i]; // extract and assign the data
            i++;
            j++;
        }
    }
    return last + 1;
}

int main(void)
{
    printf("Starting sample-data-compress...");
    printf("\n\n");

    /* Initialize the data buffer */
    uint8_t data_ptr[24] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
                            0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};
    int data_size = 24;
    uint8_t buffer[100] = {0};

    /**
     * The original buffer will be modified in place, so copy the data to a
     * secondary buffer for future checking
     */
    for (int i = 0; i < data_size; i++)
    {
        buffer[i] = data_ptr[i];
    }

    /* Do compression, and print the results */
    int new_size = byte_compress(data_ptr, data_size);
    printf("Compressed Buffer Size: %d\n", new_size);
    for (int i = 0; i < new_size; i++)
    {
        printf("0x%02X ", data_ptr[i]);
    }
    printf("\n\n");

    /* Do decompression, and print the results */
    int size = byte_decompress(data_ptr, new_size);
    printf("Decompressed Buffer Size: %d\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("0x%02X ", data_ptr[i]);
    }
    printf("\n\n");

    /**
     * Check the data buffer against the original buffer to ensure correctness.
     * We expect our buffer to contain the initial values of data_ptr and data_size.
     */
    for (int i = 0; i < data_size; i++)
    {
        if (buffer[i] != data_ptr[i])
        {
            printf("Error!");
            return 1;
        }
    }

    printf("Done.");
    return 0;
}
