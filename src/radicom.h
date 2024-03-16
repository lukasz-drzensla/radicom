#ifndef _RADICOM_H
#define _RADICOM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* sizes in bytes */
enum RC_FRAME {
    RC_FRAME_SIZE = 100,
    RC_HEADER_SIZE = 2,
    RC_DATETIME_SIZE = 5,
    RC_GPS_DATALEN = 80
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
    RC_ERR_BAD_FRAME = 3,
    RC_ERR_NULL_CB = 4
}  rcstatus_t;

/* callbacks */
/* The following callbacks have to be defined on the application level. RC_NUMO_CB represents the number of callbacks, it is not a callback itself.
* A prototype for each callback has been provided in callbacks.h.*/
enum RC_CALLBACKS {
    RC_CB_GPS_ERR,
    RC_CB_RTC_ERR,
    RC_CB_ALARM,
    RC_CB_READ_Q,
    RC_CB_READ_R,
    RC_CB_MEMREAD_Q,
    RC_CB_SETDT_Q,
    RC_CB_SETDT_R,
    RC_NUMO_CB
};

typedef struct {
    unsigned char qr;
    unsigned char more;
    unsigned char fc;
    unsigned char ec;
} rchdr_t;

typedef struct {
    unsigned char gpsdata[RC_GPS_DATALEN];
    unsigned char day;
    unsigned char month;
    unsigned short year;
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned int radiation;
} rcfdataupck_t; /* full data unpacked */

typedef unsigned char rcdt_t [RC_DATETIME_SIZE];

/* these functions allow fitting whole date and time in 5 bytes */
void rc_set_datetime (rcdt_t* datetime_dst, unsigned char day, unsigned char month, unsigned short year, unsigned char hours, unsigned char minutes, unsigned char seconds);
void rc_get_datetime (rcdt_t* datetime_src, unsigned char* day, unsigned char* month, unsigned short* year, unsigned char* hours, unsigned char* minutes, unsigned char* seconds);

/* header manipulation functions */
rcstatus_t rc_fill_header (unsigned char* frame, unsigned char qr, unsigned char more, unsigned char fc, unsigned char ec);
rcstatus_t rc_put_header (unsigned char* frame, const rchdr_t* hdr);
rcstatus_t rc_read_header (const unsigned char* frame, rchdr_t* hdr);

/* frame manipulation functions */
void rc_clear_frame(unsigned char* frame);

/* send functions */
/* query functions */
/* ------------------------------------------------------ */

/**
 * @brief Fill the frame with query to read a single measurement (eg current one)
 * @param frame buffer
*/
rcstatus_t rc_q_read (unsigned char* frame);
/**
 * @brief Fill the frame with query to read all measurements stored in memory
 * @param frame buffer
*/
rcstatus_t rc_q_memread (unsigned char* frame);
/**
 * @brief Fill the frame with query to set date and time in rtc
 * @param frame buffer
 * @param datetime date and time of the readout
*/
rcstatus_t rc_q_setdt (unsigned char* frame, rcdt_t* datetime);
/**
 * @brief Fill the frame with query to calibrate. Sending this query implies all necessary data has already been collected. The application needs to take care of this.
 * @param ext0 first measurement of an external device
 * @param ext1 second measurement of an external device
 * @param meas0 first measurement from calibrated device corresponding to first external measurement
 * @param meas1 second measurement from calibrated device corresponding to second external measurement
*/
rcstatus_t rc_q_calibrate (unsigned char* frame, unsigned int ext0, unsigned int ext1, unsigned int meas0, unsigned int meas1);
/* ------------------------------------------------------ */


/* response functions */
/* ------------------------------------------------------ */

/**
 * @brief Fill the frame with read data from gps, radiation sensor and rtc
 * @param frame buffer
 * @param gpsdata left-zero-padded buffer with length of RC_GPS_DATALEN
 * @param datetime date and time of the readout
 * @param radiation readout from the radiation sensor
 * @param ec error code
 * @return rcstatus_t RC_OK on success, error otherwise
*/
rcstatus_t rc_r_read (unsigned char* frame, const char* gpsdata, const rcdt_t* datetime, unsigned int radiation, unsigned char ec);
/**
 * @brief Fill the frame with gps, radiation and rtc data stored in memory. This function is meant to be called in a loop.
 * @param frame buffer
 * @param gpsdata left-zero-padded buffer with length of RC_GPS_DATALEN
 * @param datetime date and time of the readout
 * @param radiation readout from the radiation sensor
 * @param ec error code
 * @param is_last 0 if the provided data is not the last one in stored memory, 1 if it is
 * @return rcstatus_t RC_OK on success, error otherwise
*/
rcstatus_t rc_r_memread (unsigned char* frame, const char* gpsdata, const rcdt_t* datetime, unsigned int radiation, unsigned char ec, unsigned char is_last);
/**
 * @brief Fill the frame with response for setting date and time.
 * @param frame buffer
 * @param ec error code
*/
rcstatus_t rc_r_setdt (unsigned char* frame, unsigned char ec);
/* ------------------------------------------------------ */

/* receive functions */
rcstatus_t rc_decode (unsigned char* frame, rchdr_t* hdr, rcstatus_t(*callbacks[RC_NUMO_CB])(unsigned char* frame, void* param));
rcstatus_t rc_process_read (unsigned char* frame, rcfdataupck_t* fdata);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RADICOM_H */