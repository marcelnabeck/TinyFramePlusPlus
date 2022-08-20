//
// Created by MightyPork on 2017/10/15.
//

#ifndef TF_UTILS_H
#define TF_UTILS_H

#include <cstdio>
#include "../TinyFrame.hpp"

namespace TinyFrame_n{

/** pointer to unsigned char */
typedef unsigned char* pu8;

/**
 * Dump a binary frame as hex, dec and ASCII
 */
void dumpFrame(const uint8_t *buff, size_t len);

/**
 * Dump message metadata (not the content)
 *
 * @param msg
 */
void dumpFrameInfo(TF_Msg *msg);

} // TinyFrame_n

#endif //TF_UTILS_H
