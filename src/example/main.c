#include <stdio.h>
#include "radicom.h"
#include "callbacks.h"
#include <unistd.h>

const char* current_gpsreadout = "000000000000000$GPRMC,030742.00,A,2232.73830,N,11404.58520,E,0.356,,070314,,,A*77";
rcdt_t current_datetime = {109, 12, 54, 17, 16};
unsigned int current_radiation = 15000;

int main()
{
    rcdt_t datetime = {0,0,0,0,0};
    rc_set_datetime(&datetime, 13, 10, 2024, 13, 33, 17);
    // printf("%d %d %d %d %d\n", datetime[0], datetime[1], datetime[2], datetime[3], datetime[4]);

    // unsigned char day, month, hours, minutes, seconds;
    // day = month = hours = minutes = seconds = 0;
    // unsigned short year = 0;
    // rc_get_datetime(&datetime, &day, &month, &year, &hours, &minutes, &seconds);
    // printf("%d %d %d %d %d %d\n", day, month, year, hours, minutes, seconds);

    unsigned char frame[RC_FRAME_SIZE] = {0};
    rcstatus_t res;
    //res = rc_fill_header(frame, RC_R, RC_MORE, RC_FC_MEM_READ, RC_EC_OK);
    // for (int i = 0; i < RC_HEADER_SIZE; i++)
    // {
    //     printf("%d ", frame[i]);
    // }
    // printf("\n");
    // printf("Status: %d\n", res);
    // rchdr_t hdr;
    // res = rc_read_header(frame, &hdr);
    // printf("Recv: qr: %d, more: %d, fc: %d, ec: %d. Status: %d\n", hdr.qr, hdr.more, hdr.fc, hdr.ec, res);

    res = rc_q_setdt(frame, &datetime);
    for (int i = 0; i < RC_HEADER_SIZE+RC_DATETIME_SIZE; i++)
    {
        printf("%d ", frame[i]);
    }
    printf("\n");
    printf("Status: %d\n", res);

    // res = rc_q_calibrate(frame, 260, 5,261,280);
    // for (int i = 0; i < RC_HEADER_SIZE+16; i++)
    // {
    //     printf("%d ", frame[i]);
    // }
    // printf("\n");
    // printf("Status: %d\n", res);
    // res = rc_read_header(frame, &hdr);
    // printf("Recv: qr: %d, more: %d, fc: %d, ec: %d. Status: %d\n", hdr.qr, hdr.more, hdr.fc, hdr.ec, res);

    // rc_r_read(frame, readout, (const rcdt_t*)&datetime, 0xFF, 0);
    // for (int i = 0; i < RC_HEADER_SIZE; i++)
    // {
    //     printf("%d ", frame[i]);
    // }
    // for (int i = RC_HEADER_SIZE; i < RC_HEADER_SIZE + RC_GPS_DATALEN; i++)
    // {
    //     printf("%c ", (char)frame[i]);
    // }
    // for (int i = RC_HEADER_SIZE + RC_GPS_DATALEN; i < RC_HEADER_SIZE + RC_GPS_DATALEN + 5*sizeof(unsigned char); i++)
    // {
    //     printf("%d ", frame[i]);
    // }
    // for (int i = RC_HEADER_SIZE + RC_GPS_DATALEN + 5*sizeof(unsigned char); i < RC_HEADER_SIZE + RC_GPS_DATALEN + 5*sizeof(unsigned char) + sizeof(unsigned int); i++)
    // {
    //     printf("%d ", frame[i]);
    // }
    // printf("\n");

    rcstatus_t result;
    result = rc_q_read(frame);

    rcstatus_t(*callbacks[RC_NUMO_CB])(unsigned char* frame, void* param) = {NULL};
    callbacks[RC_CB_GPS_ERR] = rc_gps_error_cb;
    callbacks[RC_CB_RTC_ERR] = rc_alarm_cb;
    callbacks[RC_CB_ALARM] = rc_alarm_cb;
    callbacks[RC_CB_READ_Q] = rc_read_q_cb;
    callbacks[RC_CB_MEMREAD_Q] = rc_memread_q_cb;
    callbacks[RC_CB_SETDT_Q] = rc_setdt_q_cb;
    callbacks[RC_CB_SETDT_R] = rc_setdt_r_cb;
    callbacks[RC_CB_READ_R] = rc_read_r_cb;
    callbacks[RC_CB_SAVE_Q] = rc_save_q_cb;
    rchdr_t hdr;
    while (1)
    {
        result = rc_decode(frame, &hdr, callbacks);
        sleep(1);
    }
    return result;
}

rcstatus_t rc_gps_error_cb (unsigned char* frame, void* null_param)
{
    return RC_OK;
}

rcstatus_t rc_rtc_error_cb (unsigned char* frame, void* null_param)
{
    return RC_OK;
}

rcstatus_t rc_alarm_cb (unsigned char* frame, void* null_param)
{
    printf("ALARM!!!\n");
    return RC_OK;
}

rcstatus_t rc_read_q_cb (unsigned char* frame, void* null_param)
{
    /* form a response */
    rc_clear_frame(frame);
    rcstatus_t res = rc_fill_header(frame, RC_R, RC_NO_MORE, RC_FC_READ, RC_EC_OK);
    if (RC_OK != res)
    {
        return res;
    }
    unsigned char ec = RC_OK;
    res = rc_r_read(frame, current_gpsreadout, (const rcdt_t*)(&current_datetime), current_radiation, ec);
    return res;
}

rcstatus_t rc_memread_q_cb (unsigned char* frame, void* null_param)
{
    /* some loop going through the whole memory */
    /* call rc_r_memread in loop, set is_last = 1 for last iteration*/
    return RC_OK;
}

rcstatus_t rc_setdt_q_cb (unsigned char* frame, void* dtbuffer)
{
    rcdt_t dt;
    unsigned char day, month, hours, minutes, seconds;
    unsigned short year;
    year = day = month = hours = minutes = seconds = 0;
    /* Process date and time */
    for (int i = 0; i < RC_DATETIME_SIZE; i++)
    {
        dt[i] = ((unsigned char*)dtbuffer)[i];
    }
    rc_get_datetime(&dt, &day, &month, &year, &hours, &minutes, &seconds);

    /* Actually set date and time */
    printf("%d %d %d %d %d %d\n", day, month, year, hours, minutes, seconds);
    unsigned char ec = RC_EC_ALARM; /* should be retrieved from rtc set function */

    /* Form a response */
    rc_clear_frame(frame);
    return rc_r_setdt(frame, ec);
}

rcstatus_t rc_setdt_r_cb (unsigned char* frame, void* ec)
{
    printf("Error code: %d\n", *((unsigned char*)ec));
    rc_clear_frame(frame);
    return RC_OK;
}

rcstatus_t rc_read_r_cb (unsigned char* frame, void* null_param)
{
    printf("read r cb\n");
    rcfdataupck_t tfulldata;

    rcstatus_t res = rc_process_read(frame, &tfulldata);

    printf("GPSdata: %s\n", (char*)tfulldata.gpsdata);
    printf("%d %d %d %d %d %d\n", tfulldata.day, tfulldata.month, tfulldata.year, tfulldata.hours, tfulldata.minutes, tfulldata.seconds);
    printf("Current radiation: %d\n", tfulldata.radiation);
    
    rc_clear_frame(frame);
    
    return res;
}

rcstatus_t rc_save_q_cb (unsigned char* frame, void* ec)
{
    printf("save q cb\n");
    rcfdataupck_t tfulldata;

    rcstatus_t res = rc_process_read(frame, &tfulldata);

    if (RC_OK != res)
    {
        /* log error */
        return res;
    }

    /* save the received data instead of printing it */
    printf("GPSdata: %s\n", (char*)tfulldata.gpsdata);
    printf("%d %d %d %d %d %d\n", tfulldata.day, tfulldata.month, tfulldata.year, tfulldata.hours, tfulldata.minutes, tfulldata.seconds);
    printf("Current radiation: %d\n", tfulldata.radiation);
    
    rc_clear_frame(frame);

    return rc_r_save(frame, RC_EC_OK);
}

rcstatus_t rc_save_r_cb (unsigned char* frame, void* ec)
{
    printf("save r cb\n");

    return RC_OK;
}