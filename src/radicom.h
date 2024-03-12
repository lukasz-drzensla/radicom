#ifndef _RADICOM_H
#define _RADICOM_H

/* sizes in bytes */
enum RC_FRAME {
    RC_FRAME_SIZE = 100,
    RC_HEADER_SIZE = 2,
    RC_DATETIME_SIZE = 5
};

/* sizes in bits */
/* pos in bits */
enum RC_HDR {
    RC_HDR_QR_SIZE = 1,
    RC_HDR_MORE_SIZE = 1,
    RC_HDR_FC_SIZE = 4,
    RC_HDR_EC_SIZE = 2,
    RC_HDR_QR_POS = 7,
    RC_HDR_MORE_POS = 6,
    RC_HDR_FC_POS = 2,
    RC_HDR_EC_POS = 0
};

/* constants */
enum {
    RC_START_SEQ = 219,
    RC_Q = 0,
    RC_R = 1,
    RC_MORE = 1,
    RC_NO_MORE = 0
};
enum RC_FC {
    RC_FC_READ = 0,
    RC_FC_MEM_READ = 1,
    RC_FC_SET_DATE_TIME = 2,
    RC_FC_CALIBRATION = 3
};
enum RC_EC {
    RC_EC_OK = 0,
    RC_EC_GPS_ERR = 1,
    RC_EC_RTC_ERR = 2,
    RC_EC_ALARM = 3
};

/* type definitions */
typedef enum {
    RC_OK = 0,
    RC_GEN_ERROR = 1,
    RC_ERROR_PARAM = 2,
    RC_ERR_BAD_FRAME = 3
}  rcstatus_t;

typedef struct {
    unsigned char qr;
    unsigned char more;
    unsigned char fc;
    unsigned char ec;
} rchdr_t;

typedef unsigned char rcdt_t [RC_DATETIME_SIZE];

/* these functions allow fitting whole date and time in 5 bytes */
void rc_set_datetime (rcdt_t* datetime_dst, unsigned char day, unsigned char month, unsigned short year, unsigned char hours, unsigned char minutes, unsigned char seconds);
void rc_get_datetime (rcdt_t* datetime_src, unsigned char* day, unsigned char* month, unsigned short* year, unsigned char* hours, unsigned char* minutes, unsigned char* seconds);

/* helper power function */
unsigned char rc_uc_pow (unsigned char base, unsigned char exp);

/*header manipulation functions */
rcstatus_t rc_fill_header (unsigned char* frame, unsigned char qr, unsigned char more, unsigned char fc, unsigned char ec);
rcstatus_t rc_put_header (unsigned char* frame, const rchdr_t* hdr);
rcstatus_t rc_read_header (const unsigned char* frame, rchdr_t* hdr);

/* query functions */
rcstatus_t rc_q_read (unsigned char* frame);
rcstatus_t rc_q_memread (unsigned char* frame);
rcstatus_t rc_q_setdt (unsigned char* frame, rcdt_t* datetime);
rcstatus_t rc_q_calibrate (unsigned char* frame, unsigned int ext0, unsigned int ext1, unsigned int meas0, unsigned int meas1);

/* response functions */
/* TODO */

#endif /* _RADICOM_H */