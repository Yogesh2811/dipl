typedef unsigned char BYTE;

__kernel void srtp_decode(__local BYTE* src, __local BYTE* dst,
                          __local BYTE* key, __local BYTE* counter, int length){


}

__kernel void srtp_encode(__local BYTE* src, __local BYTE* dst,
                          __local BYTE* key, __local BYTE* counter, int length){
}
