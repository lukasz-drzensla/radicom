#include <stdio.h>
#include "radicom.h"

int main()
{
    rcdt_t datetime = {0,0,0,0,0};
    rc_set_datetime(&datetime, 13, 10, 2024, 13, 33, 17);
    printf("%d %d %d %d %d\n", datetime[0], datetime[1], datetime[2], datetime[3], datetime[4]);

    unsigned char day, month, hours, minutes, seconds;
    day = month = hours = minutes = seconds = 0;
    unsigned short year = 0;
    rc_get_datetime(&datetime, &day, &month, &year, &hours, &minutes, &seconds);
    printf("%d %d %d %d %d %d\n", day, month, year, hours, minutes, seconds);

    unsigned char frame[RC_FRAME_SIZE];
    rcstatus_t res = rc_fill_header(frame, RC_R, RC_MORE, RC_FC_MEM_READ, RC_EC_OK);
    for (int i = 0; i < RC_HEADER_SIZE; i++)
    {
        printf("%d ", frame[i]);
    }
    printf("\n");
    printf("Status: %d\n", res);
    rchdr_t hdr;
    res = rc_read_header(frame, &hdr);
    printf("Recv: qr: %d, more: %d, fc: %d, ec: %d. Status: %d\n", hdr.qr, hdr.more, hdr.fc, hdr.ec, res);

    res = rc_q_setdt(frame, &datetime);
    for (int i = 0; i < RC_HEADER_SIZE+RC_DATETIME_SIZE; i++)
    {
        printf("%d ", frame[i]);
    }
    printf("\n");
    printf("Status: %d\n", res);
    res = rc_read_header(frame, &hdr);
    printf("Recv: qr: %d, more: %d, fc: %d, ec: %d. Status: %d\n", hdr.qr, hdr.more, hdr.fc, hdr.ec, res);

    res = rc_q_calibrate(frame, 260, 5,261,280);
    for (int i = 0; i < RC_HEADER_SIZE+16; i++)
    {
        printf("%d ", frame[i]);
    }
    printf("\n");
    printf("Status: %d\n", res);
    res = rc_read_header(frame, &hdr);
    printf("Recv: qr: %d, more: %d, fc: %d, ec: %d. Status: %d\n", hdr.qr, hdr.more, hdr.fc, hdr.ec, res);

    return 0;
}