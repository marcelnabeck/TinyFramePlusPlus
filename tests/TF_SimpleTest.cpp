#include "../TinyFrame.hpp"
#include "../TinyFrame_CheckSum.hpp"

#include <iostream>
#include <cstring>
#include <cassert>

using TinyFrame_CRC16 = TinyFrame_n::TinyFrame<TinyFrame_n::CKSUM_t::CRC16>;
using TinyFrame_CRC16T = TinyFrame_n::TinyFrame<TinyFrame_n::CKSUM_t::CRC16_TABLELESS>;

extern TinyFrame_CRC16 tf_1;
extern TinyFrame_CRC16T tf_2;
namespace TinyFrame_n{

void WriteImpl_1(const uint8_t *buff, size_t len)
{
    printf("WriteImpl1:");
    // send to UART
    for (size_t i = 0; i < len; i++){
        printf("%02x", buff[i]);
    }
    printf("\n");
    tf_2.Accept(buff, len);

    uint8_t buff2[128];
    memcpy(buff2, buff, len);
    buff2[10] = 0; // invalidate payload

    printf("WriteImpl1:");
    // send to UART
    for (size_t i = 0; i < len; i++){
        printf("%02x", buff2[i]);
    }
    printf("\n");
    tf_2.Accept(buff2, len);
}

void WriteImpl_2(const uint8_t *buff, size_t len)
{
    tf_1.Accept(buff, len);
    printf("WriteImpl2:");
    // send to UART
    for (size_t i = 0; i < len; i++){
        printf("%02x", buff[i]);
    }
    printf("\n");
}

void Error_1(TinyFrame_n::ErrorMsg_t message){
    printf("Error_1: %u\n", static_cast<uint8_t>(message));
}

void Error_2(TinyFrame_n::ErrorMsg_t message){
    printf("Error_2: %u\n", static_cast<uint8_t>(message));
}

// --------- Mutex callbacks ----------
// Needed only if USE_MUTEX is 1 in the config file.
// DELETE if mutex is not used

/** Claim the TX interface before composing and sending a frame */
bool ClaimTx()
{
    // take mutex
    return true; // we succeeded
}

/** Free the TX interface after composing and sending a frame */
void ReleaseTx()
{
    // release mutex
}

// --------- Custom checksums ---------
// This should be defined here only if a custom checksum type is used.
// DELETE those if you use one of the built-in checksum types

/** Initialize a checksum */
template<>
CKSUM<CKSUM_t::CUSTOM8> CksumStart<CKSUM_t::CUSTOM8>(void)
{
    return 0;
}

/** Update a checksum with a byte */
template<>
CKSUM<CKSUM_t::CUSTOM8> CksumAdd<CKSUM_t::CUSTOM8>(CKSUM<CKSUM_t::CUSTOM8> cksum, uint8_t byte)
{
    return cksum ^ byte;
}

/** Finalize the checksum calculation */
template<>
CKSUM<CKSUM_t::CUSTOM8> CksumEnd<CKSUM_t::CUSTOM8>(CKSUM<CKSUM_t::CUSTOM8> cksum)
{
    return cksum;
}


// --------- Example Type Listener ---------
Result genericListener(Msg *msg){
    printf("Received Message - Generic Listener : %s\n", msg->data);    
    return Result::STAY;
}

Result typeListener123(Msg *msg){
    printf("Received Message - Type Listener 123: %s\n", msg->data);    
    return Result::STAY;
}

Result typeTimeoutListener123(){
    printf("Timeout - Type Listener 123\n");    
    return Result::STAY;
}

Result idListener234(Msg *msg){
    printf("Received Message - ID Listener 234: %s\n", msg->data);    
    return Result::CLOSE;
}

Result idTimeoutListener234(){
    printf("Timeout - ID Listener 234\n");
    return Result::STAY;
}

Result queryListener(Msg *msg){
    printf("Received Message - Query Listener: %s\n", msg->data);    
    return Result::CLOSE;
}

Result queryTimeoutListener(){
    printf("Timeout - Query Listener\n");
    return Result::STAY;
}

} // TinyFrame_n



const TinyFrame_CRC16::RequiredCallbacks callbacks_1 = {
    TinyFrame_n::WriteImpl_1, // WriteImpl
    TinyFrame_n::Error_1, // WriteImpl
};

const TinyFrame_CRC16T::RequiredCallbacks callbacks_2 = {
    TinyFrame_n::WriteImpl_2,
    TinyFrame_n::Error_2,
};

const TinyFrame_CRC16::OptionalCallbacks callbacks_Mutex_1 = {
    TinyFrame_n::ClaimTx, // ClaimTx
    TinyFrame_n::ReleaseTx, // ReleaseTx
};



TinyFrame_n::TinyFrameConfig_t config = {
    1U, // ID_BYTES
    2U, // LEN_BYTES
    1U, // TYPE_BYTES
    1U, // USE_SOF_BYTE
    0xAAU, // SOF_BYTE
    10U // PARSER_TIMEOUT_TICKS
};

TinyFrame_CRC16 tf_1(callbacks_1, TinyFrame_n::Peer::MASTER);
TinyFrame_CRC16T tf_2(callbacks_2, TinyFrame_n::Peer::SLAVE);

// TinyFrame_CRC16 tf16(callbacks16);
// TinyFrame_CRC32 tf32_2(callbacks32, config);


int main(){

    printf("size tf_1: 0x%X\n\n", (int)sizeof(tf_1));

    {
        uint8_t crc_data_8[] = {0x40, 0x41, 0x42};
        uint8_t crc1 = 0x00;
        uint8_t crc2 = 0x00;
        crc1 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC8>();
        crc2 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC8_TABLELESS>();
        for(size_t i = 0; i < sizeof(crc_data_8); i++){
            crc1 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC8>(crc1, crc_data_8[i]);
            crc2 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC8_TABLELESS>(crc2, crc_data_8[i]);
        }
        crc1 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC8>(crc1);
        crc2 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC8_TABLELESS>(crc2);
        printf("CRC8: 0x%X == 0x%X\n", crc1, crc2);
        assert(crc1 == crc2);
    }
 
    {
        uint16_t crc_data_16[] = {0x4040, 0x4141, 0x4242};
        uint16_t crc1 = 0x0000;
        uint16_t crc2 = 0x0000;
        crc1 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC16>();
        crc2 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC16_TABLELESS>();
        for(size_t i = 0; i < sizeof(crc_data_16); i++){
            uint8_t byte = reinterpret_cast<uint8_t*>(&crc_data_16)[i];
            crc1 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC16>(crc1, byte);
            crc2 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC16_TABLELESS>(crc2, byte);
        }
        crc1 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC16>(crc1);
        crc2 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC16_TABLELESS>(crc2);
        printf("CRC16: 0x%X == 0x%X\n", crc1, crc2);
        assert(crc1 == crc2);
    }

    {
        uint32_t crc_data_32[] = {0x40404040, 0x41414141, 0x42424242};
        uint32_t crc1 = 0x00000000;
        uint32_t crc2 = 0x00000000;
        crc1 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC32>();
        crc2 = TinyFrame_n::CksumStart<TinyFrame_n::CKSUM_t::CRC32_TABLELESS>();
        for(size_t i = 0; i < sizeof(crc_data_32); i++){
            uint8_t byte = reinterpret_cast<uint8_t*>(&crc_data_32)[i];
            crc1 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC32>(crc1, byte);
            crc2 = TinyFrame_n::CksumAdd<TinyFrame_n::CKSUM_t::CRC32_TABLELESS>(crc2, byte);
        }
        crc1 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC32>(crc1);
        crc2 = TinyFrame_n::CksumEnd<TinyFrame_n::CKSUM_t::CRC32_TABLELESS>(crc2);
        printf("CRC32: 0x%X == 0x%X\n", crc1, crc2);
        assert(crc1 == crc2);
    }

    uint8_t messageData[] = "Hello TinyFrame!";

    TinyFrame_n::Msg msg = {
        234, // frame_id   
        false, // is_response
        123, // type       
        messageData, // data       
        sizeof(messageData), // len        
        nullptr, // userdata
        nullptr // userdata2
    };

    bool successGenericListener = tf_2.AddGenericListener(&TinyFrame_n::genericListener);
    assert(successGenericListener);
    bool successTypeListener = tf_2.AddTypeListener(123, &TinyFrame_n::typeListener123);
    assert(successTypeListener);
    bool successIDListener = tf_2.AddIdListener(&msg, &TinyFrame_n::idListener234, &TinyFrame_n::idTimeoutListener234, 10);
    assert(successIDListener);

    for (size_t i = 0; i < 5; i++){
        tf_2.Tick();
    }

    bool successSend = tf_1.Send(&msg);
    assert(successSend);
    bool successSendSimple = tf_1.SendSimple(123, messageData, sizeof(messageData));
    assert(successSendSimple);

    bool successSendQuery = tf_1.Query(&msg, &TinyFrame_n::idListener234, nullptr, 0);
    assert(successSendQuery);
    bool successSendRespond = tf_1.Respond(&msg);
    assert(successSendRespond);
    bool successSendQueryMulti = tf_1.Query_Multipart(&msg, &TinyFrame_n::queryListener, &TinyFrame_n::queryTimeoutListener, 10);
    assert(successSendQueryMulti);
    tf_1.Respond_Multipart(&msg);
    
    bool successSendQuerySimple = tf_1.QuerySimple(123, messageData, sizeof(messageData), &TinyFrame_n::typeListener123, &TinyFrame_n::typeTimeoutListener123, 10);
    assert(successSendQuerySimple);

    tf_1.Multipart_Payload(messageData, sizeof(messageData));
    tf_1.Multipart_Close();

    assert(successSend);
}