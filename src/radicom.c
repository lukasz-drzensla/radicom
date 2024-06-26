#include "radicom.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RC_INIT_CRC 0xFFFF
#define RC_POLY 0xA001

/* helper functions */
unsigned char _rc_uc_pow (unsigned char base, unsigned char exp);
rcstatus_t _frame_r_read (unsigned char* frame, const char* gpsdata, const rcdt_t* datetime, unsigned int radiation);
rcstatus_t _rc_calc_crc (unsigned char* frame, size_t frame_size);
rcstatus_t _rc_validate_crc (const unsigned char* frame, size_t frame_size);
unsigned short _crc16 (const unsigned char* buffer, unsigned char len);

void rc_set_datetime (rcdt_t* datetime_dst, unsigned char day, unsigned char month, unsigned short year, unsigned char hours, unsigned char minutes, unsigned char seconds)
{
    (*datetime_dst)[0] = (unsigned char)((day & 0x1F) << 3);
    (*datetime_dst)[0] |= (unsigned char)((month & 0xF) >> 1);
    (*datetime_dst)[1] = (unsigned char)((month & 0x1) << 7);
    (*datetime_dst)[1] |= (unsigned char)((unsigned char)(year % 2000) >> 1);
    (*datetime_dst)[2] = (unsigned char)((((unsigned char)year) & 0x1) << 7);
    (*datetime_dst)[2] |= (unsigned char)((hours & 0x1F) << 2);
    (*datetime_dst)[2] |= (unsigned char)((minutes & 0x30)) >> 4;
    (*datetime_dst)[3] = (unsigned char)((minutes & 0x0F) << 4);
    (*datetime_dst)[3] |= (unsigned char)((seconds & 0x30) >> 4);
    (*datetime_dst)[4] = (unsigned char)((seconds & 0x0F) << 4);
}

void rc_get_datetime (rcdt_t* datetime_src, unsigned char* day, unsigned char* month, unsigned short* year, unsigned char* hours, unsigned char* minutes, unsigned char* seconds)
{
    *day = (*datetime_src)[0] >> 3;
    *month = ((*datetime_src)[0] & 0x7) << 1;
    *month |= (*datetime_src)[1] >> 7;
    *year = ((*datetime_src)[1] & 0x7F) << 1;
    *year |= (*datetime_src)[2] >> 7;
    *year += 2000;
    *hours = ((*datetime_src)[2] &0x7E) >> 2;
    *minutes = ((*datetime_src)[2] &0x3) << 4;
    *minutes |= ((*datetime_src)[3] &0xF0) >> 4;
    *seconds = ((*datetime_src)[3] &0x03) << 4;
    *seconds |= ((*datetime_src)[4] &0xF0) >> 4;
}

unsigned char _rc_uc_pow (unsigned char base, unsigned char exp)
{
    unsigned char result = 1;
    while (exp)
    {
        if (exp % 2)
        {
            result *= base;
        }
        exp /= 2;
        base *= base;
    }
    return result;
}

unsigned short _crc16 (const unsigned char* buffer, unsigned char len)
{
    unsigned short crc = RC_INIT_CRC;
    for (unsigned char j = 0; j < len; j++)
    {
        crc ^= buffer[j];
        for (unsigned char i = 0; i < 8; i++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1; 
                crc ^= RC_POLY;
            } 
            else 
            { 
                crc >>= 1;
            }
        }
    }
    return crc;
}

rcstatus_t _rc_calc_crc (unsigned char* frame, size_t frame_size)
{
    unsigned short crc = _crc16(frame, frame_size - 2) ;
    frame[frame_size - 1] = crc & 0xFF;
    frame[frame_size - 2] = (crc & 0xFF00) >> 8;

    return RC_OK;
}

rcstatus_t _rc_validate_crc (const unsigned char* frame, size_t frame_size)
{
    if (_crc16(frame, frame_size-2) == (unsigned short)(frame[frame_size-1] | frame[frame_size-2] << 8))
    {
        return RC_OK;
    }
    return RC_ERR_BAD_CRC;
}

rcstatus_t rc_fill_header (unsigned char* frame, unsigned char qr, unsigned char more, unsigned char fc, unsigned char ec)
{
    /* parameter checking */
    if ((qr > _rc_uc_pow(2, RC_HDR_QR_SIZE) - 1) || (more > _rc_uc_pow(2, RC_HDR_MORE_SIZE) - 1) || fc > (_rc_uc_pow(2, RC_HDR_FC_SIZE) - 1) || (ec > _rc_uc_pow(2, RC_HDR_EC_SIZE) - 1)) {
        return RC_ERROR_PARAM;
    }

    /* fill */
    frame[0] = (unsigned char)RC_START_SEQ;
    frame[1] = (unsigned char)((qr << RC_HDR_QR_POS) | (more << RC_HDR_MORE_POS) | (fc << RC_HDR_FC_POS) | (ec << RC_HDR_EC_POS));
    return RC_OK;
}

inline rcstatus_t rc_put_header (unsigned char* frame, const rchdr_t* hdr)
{
    return rc_fill_header(frame, hdr->qr, hdr->more, hdr->fc, hdr->ec);
}

rcstatus_t rc_read_header (const unsigned char* frame, rchdr_t* hdr)
{
    if (frame[0] != RC_START_SEQ)
    {
        return RC_ERR_BAD_FRAME;
    }
    hdr->qr = (frame[1] & (1 << RC_HDR_QR_POS)) == 0 ? RC_Q : RC_R;
    hdr->more = (frame[1] & (1 << RC_HDR_MORE_POS)) == 0 ? RC_NO_MORE : RC_MORE;
    hdr->fc = (frame[1] & (_rc_uc_pow(2, RC_HDR_FC_SIZE) - 1) << RC_HDR_FC_POS) >> RC_HDR_FC_POS;
    hdr->ec = frame[1] & (_rc_uc_pow(2, RC_HDR_EC_SIZE) - 1);
    return RC_OK;
}

void rc_clear_frame(unsigned char* frame)
{
    for (int i = 0; i < RC_FRAME_SIZE; i++)
    {
        frame[i] = 0;
    }
}

inline rcstatus_t rc_q_read (unsigned char* frame)
{
    if (RC_OK == rc_fill_header(frame, RC_Q, RC_NO_MORE, RC_FC_READ, RC_EC_OK))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

inline rcstatus_t rc_q_memread (unsigned char* frame)
{
    if (RC_OK == rc_fill_header(frame, RC_Q, RC_NO_MORE, RC_FC_MEM_READ, RC_EC_OK))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t rc_q_setdt (unsigned char* frame, rcdt_t* datetime)
{
    rcstatus_t res = rc_fill_header(frame, RC_Q, RC_NO_MORE, RC_FC_SET_DATE_TIME, RC_EC_OK);
    if (RC_OK != res)
    {
        return res;
    }
    
    for (int i = RC_HEADER_SIZE, j = 0; i < RC_HEADER_SIZE + RC_DATETIME_SIZE; i++, j++)
    {
        frame[i] = (*datetime)[j];
    }
    return _rc_calc_crc(frame, RC_FRAME_SIZE);
}

rcstatus_t rc_q_calibrate (unsigned char* frame, unsigned int ext0, unsigned int ext1, unsigned int meas0, unsigned int meas1)
{
    rcstatus_t res = rc_fill_header(frame, RC_Q, RC_NO_MORE, RC_FC_CALIBRATION, RC_EC_OK);
    if (RC_OK != res)
    {
        return res;
    }

    for (unsigned int i = RC_HEADER_SIZE; i < RC_HEADER_SIZE + sizeof(unsigned int); i++)
    {
        frame[i] = (ext0 & (0xFF << 8*(i-RC_HEADER_SIZE))) >> 8 * (i-RC_HEADER_SIZE);
    }

    for (unsigned int i = RC_HEADER_SIZE + sizeof(unsigned int); i < RC_HEADER_SIZE + 2*sizeof(unsigned int); i++)
    {
        frame[i] = (ext1 & (0xFF << 8*(i-RC_HEADER_SIZE-sizeof(unsigned int)))) >> 8 * (i-RC_HEADER_SIZE-sizeof(unsigned int));
    }

    for (unsigned int i = RC_HEADER_SIZE + 2*sizeof(unsigned int); i < RC_HEADER_SIZE + 3*sizeof(unsigned int); i++)
    {
        frame[i] = (meas0 & (0xFF << 8*(i-RC_HEADER_SIZE-2*sizeof(unsigned int)))) >> 8 * (i-RC_HEADER_SIZE-2*sizeof(unsigned int));
    }

    for (unsigned int i = RC_HEADER_SIZE + 3*sizeof(unsigned int); i < RC_HEADER_SIZE + 4*sizeof(unsigned int); i++)
    {
        frame[i] = (meas1 & (0xFF << 8*(i-RC_HEADER_SIZE-3*sizeof(unsigned int)))) >> 8 * (i-RC_HEADER_SIZE-3*sizeof(unsigned int));
    }

    return _rc_calc_crc(frame, RC_FRAME_SIZE);
}

rcstatus_t rc_q_save (unsigned char* __RESTRICT__ frame, const char* __RESTRICT__ gpsdata, const rcdt_t* datetime, unsigned int radiation, unsigned char ec)
{
    rcstatus_t res = rc_fill_header(frame, RC_Q, RC_NO_MORE, RC_FC_SAVE, ec);
    if (RC_OK != res)
    {
        return res;
    }
    if (RC_OK == _frame_r_read(frame, gpsdata, datetime, radiation))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t _frame_r_read (unsigned char* __RESTRICT__ frame, const char* __RESTRICT__ gpsdata, const rcdt_t* datetime, unsigned int radiation)
{
    for (unsigned int i = RC_HEADER_SIZE; i < RC_HEADER_SIZE + RC_GPS_DATALEN; i++)
    {
        frame[i] = (unsigned char)gpsdata[i];
    }

    for (unsigned int i = RC_HEADER_SIZE + RC_GPS_DATALEN; i < RC_HEADER_SIZE + RC_GPS_DATALEN + RC_DATETIME_SIZE; i++)
    {
        frame[i] = (*datetime)[i - (RC_HEADER_SIZE + RC_GPS_DATALEN)];
    }

    for (unsigned int i = RC_HEADER_SIZE + RC_GPS_DATALEN + RC_DATETIME_SIZE; i < RC_HEADER_SIZE + RC_GPS_DATALEN + RC_DATETIME_SIZE + sizeof(unsigned int); i++)
    {
        frame[i] = (radiation & 0xFF << (8*(i - (RC_HEADER_SIZE + RC_GPS_DATALEN + RC_DATETIME_SIZE)))) >> (8*(i - (RC_HEADER_SIZE + RC_GPS_DATALEN + RC_DATETIME_SIZE)));
    }
    
    return _rc_calc_crc(frame, RC_FRAME_SIZE);;
}

rcstatus_t rc_r_read (unsigned char* __RESTRICT__ frame, const char* __RESTRICT__ gpsdata, const rcdt_t* datetime, unsigned int radiation, unsigned char ec)
{
    rcstatus_t res = rc_fill_header(frame, RC_R, RC_NO_MORE, RC_FC_READ, ec);
    if (RC_OK != res)
    {
        return res;
    }
    if (RC_OK == _frame_r_read(frame, gpsdata, datetime, radiation))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t rc_r_memread (unsigned char* __RESTRICT__ frame, const char* __RESTRICT__ gpsdata, const rcdt_t* datetime, unsigned int radiation, unsigned char ec, unsigned char is_last)
{
    rcstatus_t res = rc_fill_header(frame, RC_R, !is_last, RC_FC_READ, ec);
    if (RC_OK != res)
    {
        return res;
    }
    if (RC_OK == _frame_r_read(frame, gpsdata, datetime, radiation))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t rc_r_setdt (unsigned char* frame, unsigned char ec)
{
    if (RC_OK == rc_fill_header(frame, RC_R, RC_NO_MORE, RC_FC_SET_DATE_TIME, ec))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t rc_r_save (unsigned char* frame, unsigned char ec)
{
    if (RC_OK == rc_fill_header(frame, RC_R, RC_NO_MORE, RC_FC_SAVE, ec))
    {
        return _rc_calc_crc(frame, RC_FRAME_SIZE);
    }
    return RC_GEN_ERROR;
}

rcstatus_t rc_process_read (unsigned char* frame, rcfdataupck_t* fdata)
{
    for (int i = RC_HEADER_SIZE; i < RC_GPS_DATALEN + RC_HEADER_SIZE; i++)
    {
        fdata->gpsdata[i-RC_HEADER_SIZE] = frame[i];
    }

    rcdt_t dt;
    /* Process date and time */
    for (int i = 0; i < RC_DATETIME_SIZE; i++)
    {
        dt[i] = ((unsigned char*)frame)[i + RC_GPS_DATALEN + RC_HEADER_SIZE];
    }
    rc_get_datetime(&dt, &fdata->day, &fdata->month, &fdata->year, &fdata->hours, &fdata->minutes, &fdata->seconds);
    

    fdata->radiation = 0;

    for (int i = RC_GPS_DATALEN + RC_HEADER_SIZE+ RC_DATETIME_SIZE; i < RC_GPS_DATALEN + RC_HEADER_SIZE + RC_DATETIME_SIZE + sizeof(unsigned int); i++)
    {
        fdata->radiation |= frame[i] << (8*(i - (RC_GPS_DATALEN + RC_HEADER_SIZE+ RC_DATETIME_SIZE)));
    }
    
    return _rc_calc_crc(frame, RC_FRAME_SIZE);;
}

rcstatus_t rc_decode (unsigned char* frame, rchdr_t* hdr, rcstatus_t(*callbacks[RC_NUMO_CB])(unsigned char* frame, void* param))
{
    rcstatus_t res = rc_read_header(frame, hdr);
    if (RC_OK != res)
    {
        return res;
    }

    res = _rc_validate_crc(frame, RC_FRAME_SIZE);
    if (RC_OK != res)
    {
        return res;
    }

    /* Received error code handling */
    switch (hdr->ec)
    {
        case RC_EC_OK:
        break;
        case RC_EC_GPS_ERR:
        if (NULL != callbacks[RC_CB_GPS_ERR])
            res = (*callbacks[RC_CB_GPS_ERR])(frame, NULL);
        else
            res = RC_ERR_NULL_CB;
        break;
        case RC_EC_RTC_ERR:
        if (NULL != callbacks[RC_CB_RTC_ERR])
            res = (*callbacks[RC_CB_RTC_ERR])(frame, NULL);
        else
            res = RC_ERR_NULL_CB;
        break;
        case RC_EC_ALARM:
        if (NULL != callbacks[RC_CB_ALARM])
            res = (*callbacks[RC_CB_ALARM])(frame, NULL);
        else
            res = RC_ERR_NULL_CB;
        break;
        default:
            res = RC_GEN_ERROR;
        break;
    }

    /* Received function code handling */
    switch (hdr->fc)
    {
        case RC_FC_READ:
        {
            if (RC_Q == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_READ_Q])
                    res = (*callbacks[RC_CB_READ_Q])(frame, NULL);
                else
                    res = RC_ERR_NULL_CB;
            } else if (RC_R == hdr->qr) {
                if (NULL != callbacks[RC_CB_READ_R])
                    res = (*callbacks[RC_CB_READ_R])(frame, NULL);
                else
                    res = RC_ERR_NULL_CB;
            }
            break;
        }
        case RC_FC_MEM_READ:
        {
            if (RC_Q == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_MEMREAD_Q])
                    res = (*callbacks[RC_CB_MEMREAD_Q])(frame, NULL);
                else
                    res = RC_ERR_NULL_CB;
            }
            break;
        }
        case RC_FC_SET_DATE_TIME:
        {
            if (RC_Q == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_SETDT_Q])
                    res = (*callbacks[RC_CB_SETDT_Q])(frame, (frame+RC_HEADER_SIZE));
                else
                    res = RC_ERR_NULL_CB;
            } else if (RC_R == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_SETDT_R])
                    res = (*callbacks[RC_CB_SETDT_R])(frame, (frame+RC_HEADER_SIZE));
                else
                    res = RC_ERR_NULL_CB;
            }
            break;
        }
        case RC_FC_SAVE:
        {
            if (RC_Q == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_SAVE_Q])
                    res = (*callbacks[RC_CB_SAVE_Q])(frame, NULL);
                else
                    res = RC_ERR_NULL_CB;
            } else if (RC_R == hdr->qr)
            {
                if (NULL != callbacks[RC_CB_SAVE_R])
                    res = (*callbacks[RC_CB_SAVE_R])(frame, NULL);
                else
                    res = RC_ERR_NULL_CB;
            }
            break;
        }
    }

    return res;
}

#ifdef __cplusplus
};
#endif /* __cplusplus */