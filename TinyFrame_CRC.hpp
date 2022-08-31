#ifndef TinyFrame_CRC_HPP
#define TinyFrame_CRC_HPP

#include "TinyFrame_Types.hpp"

namespace TinyFrame_n{
//region Checksums

// Custom checksum functions

/**
 * Initialize a checksum
 *
 * @return initial checksum value
 */
template<CKSUM_t CKSUM_TYPE>
CKSUM<CKSUM_TYPE> CksumStart(void);

/**
 * Update a checksum with a byte
 *
 * @param cksum - previous checksum value
 * @param byte - byte to add
 * @return updated checksum value
 */
template<CKSUM_t CKSUM_TYPE>
CKSUM<CKSUM_TYPE> CksumAdd(CKSUM<CKSUM_TYPE> cksum, uint8_t byte);

/**
 * Finalize the checksum calculation
 *
 * @param cksum - previous checksum value
 * @return final checksum value
 */
template<CKSUM_t CKSUM_TYPE>
CKSUM<CKSUM_TYPE> CksumEnd(CKSUM<CKSUM_TYPE> cksum);

#define CKSUM_RESET(cksum)     do { (cksum) = CksumStart<CKSUM_TYPE>(); } while (0)
#define CKSUM_ADD(cksum, byte) do { (cksum) = CksumAdd<CKSUM_TYPE>((cksum), (byte)); } while (0)
#define CKSUM_FINALIZE(cksum)  do { (cksum) = CksumEnd<CKSUM_TYPE>((cksum)); } while (0)

/* Tableless CRC */

template <typename DATA_TYPE>
DATA_TYPE TablelessCrc_Function(DATA_TYPE poly, DATA_TYPE inital_crc, uint8_t byte)
{

    /* extract relevant byte references */
    DATA_TYPE msbMask = static_cast<DATA_TYPE>(1U << ((sizeof(DATA_TYPE) * 8U) - 1));

    /* init crc byte */
    inital_crc ^= (byte << ((sizeof(DATA_TYPE) - 1) * 8U));

    /* calculate byte CRC using XOR */
    for (uint8_t bitIndex = 0U; bitIndex < 8U; bitIndex++)
    {
        if (inital_crc & msbMask)
        {
            inital_crc = (inital_crc << 1U) ^ poly;
        }
        else
        {
            inital_crc <<= 1U;
        }
    }

    /* return modified crc */
    return inital_crc; 
}

//endregion
}

#endif // TinyFrame_CRC_HPP