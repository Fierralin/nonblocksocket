#ifndef SK_H
#define SK_H

typedef struct TCPheader {
    uint8_t type; // 1
    uint32_t src_ip;  // 4 +1
    uint16_t port; // 2 +5
    uint16_t num_msg; // 2 +7
    uint16_t len_msg;
} TCPheader; // 11

int process() {

}

int sev() {

}



#endif // SK_H
