/*
* Autor: Roland Schulz (xschul06)
*/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SERVER_PORT 69

#define BITS_IN_BYTE 8

#define BLOCK_LENGTH_BYTES 512

//ports are between 0 and 65535
//unsigned short int is enough

#define OPCODE_LENGTH_BYTES 2

enum tftp_opcode : unsigned short {
    RRQ = 1,    //read request 
    WRQ = 2,    //write request
    DATA = 3,   //data
    ACK = 4,    //acknowledgement
    ERR = 5,    //error
    OACK = 6    //option acknowledgement
};

// muze byt kombinace velkych a malych pismen -> udelej vsechno na lowercase
#define NETASCII_MODE "netascii"
#define OCTET_MODE "octet"
#define MAIL_MODE "mail"
#define MODE_LENGTH_BYTES_MAX 8
#define MODE_LENGTH_BYTES_MIN 5

enum tftp_mode {
    netascii,
    octet,
};

#define BLOCK_NUMBER_LENGTH_BYTES 2

// RRQ/WRQ packet
// 2 bytes     string    1 byte     string   1 byte
// ------------------------------------------------
//| Opcode |  Filename  |   0  |    Mode    |   0  |
// ------------------------------------------------

// DATA packet
// 2 bytes     2 bytes      n bytes
// ----------------------------------
//| Opcode |   Block #  |   Data     |
// ----------------------------------

#define DATA_BLOCK_NUMBER_START 1
#define DATA_BLOCK_NUMBER_INCREMENT 1

// if the data field is less than 512 bytes long, it signals termination of the transfer
// if the data field is exactly 512 bytes long, it signals that there is probably more data to come
#define DATA_FIELD_LENGTH_MAX = BLOCK_LENGTH_BYTES

#define DEFAULT_BLOCK_SIZE_BYTES 512


// ACK packet
// 2 bytes     2 bytes
// ---------------------
//| Opcode |   Block #  |
// ---------------------


// ERR packet
// 2 bytes     2 bytes      string    1 byte
// -----------------------------------------
//| Opcode |  ErrorCode |   ErrMsg   |   0  |
// -----------------------------------------

#define ERROR_CODE_LENGTH_BYTES 2



#endif