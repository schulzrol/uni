#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SERVER_PORT 69

#define BITS_IN_BYTE 8

#define BLOCK_LENGTH_BYTES 512

//ports are between 0 and 65535
//unsigned short int is enough

#define OPCODE_LENGTH_BYTES 2

#define OPCODE_RRQ 1  //read request
#define OPCODE_WRQ 2  //write request
#define OPCODE_DATA 3 //data
#define OPCODE_ACK 4  //acknowledgement
#define OPCODE_ERR 5  //error
#define OPCODE_OACK 6 //option acknowledgement

enum tftp_opcode {
    RRQ = 1,
    WRQ = 2,
    DATA = 3,
    ACK = 4,
    ERR = 5,
    OACK = 6,
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
    mail
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
// overit, ze je tohle vsechno a ze je to spravne
enum ERROR_CODES {
    NOT_DEFINED = 0,
    FILE_NOT_FOUND = 1,
    ACCESS_VIOLATION = 2,
    DISK_FULL = 3,
    ILLEGAL_OPERATION = 4,
    UNKNOWN_TID = 5,
    FILE_ALREADY_EXISTS = 6,
    NO_SUCH_USER = 7
};



#endif