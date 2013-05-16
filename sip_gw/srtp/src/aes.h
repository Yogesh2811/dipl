/** @file */
/**
 *  \brief Serial implementation of AES cipher. 
 */
 
#ifndef AES_H
#define AES_H

/** \brief Size of round key*/
#define ROUND_KEY_SIZE 11
/** \brief Number of rounds per AES block*/
#define ROUNDS 10
/** \brief Size of block in bytes*/
#define BLOCK_SIZE 16
/** \brief Number of rows in AES block*/
#define ROWS 4
/** \brief Number of columns in AES block*/
#define COLUMNS 4

/** \brief Size of single byte in AES block*/
typedef unsigned char BYTE;

/** \brief Interface for AES implementation*/
namespace AES {
    /** \brief Key expansion algorithm
     *    
     * @param master_key defined by each SRTP session
     * @param round_key expanded key      
     */         
    void expand_key(BYTE* master_key, BYTE round_key[ROUND_KEY_SIZE][BLOCK_SIZE]);
    /** \brief Encoding of entire packet payload using 128-bit AES in CTR mode
     *    
     * @param src source data buffer with incomming packet payload
     * @param dst destination data buffer with outgoing packet payload 
     * @param key encryption master key defined by the session
     * @param iv initial vector for CTR mode
     * @param length number of bytes of incomming packet payload                   
     */
    void srtp_encode(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    /** \brief Decoding of entire packet payload using 128-bit AES in CTR mode
     *    
     * @param src source data buffer with incomming packet payload
     * @param dst destination data buffer with outgoing packet payload 
     * @param key encryption master key defined by the session
     * @param iv initial vector for CTR mode
     * @param length number of bytes of incomming packet payload                   
     */
    void srtp_decode(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    /** \brief testing method for measurement purposes*/
    void test();
    /** \brief print method for testing purposes*/
    void print_state(BYTE* state);
};

#endif
/** @} */