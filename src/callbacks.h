#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "radicom.h"

/* null param = null will be passed*/
/* RC_CB_GPS_ERR */
rcstatus_t rc_gps_error_cb (unsigned char* frame, void* null_param);
/* RC_CB_RTC_ERR */
rcstatus_t rc_rtc_error_cb (unsigned char* frame, void* null_param);
/* RC_CB_ALARM */
rcstatus_t rc_alarm_cb (unsigned char* frame, void* null_param);
/* RC_CB_READ_Q */
rcstatus_t rc_read_q_cb (unsigned char* frame, void* null_param);
/* RC_CB_READ_R */
rcstatus_t rc_read_r_cb (unsigned char* frame, void* null_param);
/* RC_CB_MEMREAD_Q */
rcstatus_t rc_memread_q_cb (unsigned char* frame, void* null_param);
/* RC_CB_SETDT_Q */
/**
 * @param dtbuffer Pointer to the buffer of type unsigned char* in which RC_DATETIME_SIZE bytes storing date and time are located.
*/
rcstatus_t rc_setdt_q_cb (unsigned char* frame, void* dtbuffer);
/* RC_CB_SETDT_R */
/**
 * @param ec Pointer to an unsigned char in which error code is stored.
*/
rcstatus_t rc_setdt_r_cb (unsigned char* frame, void* ec);
/* RC_CB_SAVE_Q */
/**
 * @param ec Pointer to an unsigned char in which error code is stored.
*/
rcstatus_t rc_save_q_cb (unsigned char* frame, void* ec);
/* RC_CB_SAVE_R */
/**
 * @param ec Pointer to an unsigned char in which error code is stored.
*/
rcstatus_t rc_save_r_cb (unsigned char* frame, void* ec);


#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _CALLBACKS_H */