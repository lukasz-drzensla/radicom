#include "radicom.h"
#include "CRadicom.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned char frame[RC_FRAME_SIZE] = {0};

#define JHDR_LEN 4

/* for replying to ourselves */
const char* current_gpsreadout = "000000000000000$GPRMC,030742.00,A,2232.73830,N,11404.58520,E,0.356,,070314,,,A*77";
rcdt_t current_datetime = {109, 12, 54, 17, 16};
unsigned int current_radiation = 15000;

jintArray read_hdr (JNIEnv* env, jobject obj, jintArray array)
{
    jintArray jhdr = (*env)->NewIntArray(env, JHDR_LEN);
    jsize len = (*env)->GetArrayLength(env, array);

    if (len < RC_HEADER_SIZE)
    {
        /* log error - bad header length */
        goto Java_JRadicom_read_1header_finish;
    }

    jint *body = (*env)->GetIntArrayElements(env, array, 0);
    unsigned char header[2];
    header[0] = (unsigned char)body[0];
    header[1] = (unsigned char)body[1];

    //rc_read_header(header, hdr);
    rchdr_t hdr;
    rc_read_header(header, &hdr);

    int chdr[JHDR_LEN] = {0};
    chdr[0] = hdr.qr;
    chdr[1] = hdr.more;
    chdr[2] = hdr.fc;
    chdr[3] = hdr.ec;

    (*env)->SetIntArrayRegion(env, jhdr, 0, JHDR_LEN, chdr);
    Java_JRadicom_read_1header_finish:
    return jhdr;
}

jintArray q_read (JNIEnv* env, jobject obj)
{
    jintArray jframe = (*env)->NewIntArray(env, RC_FRAME_SIZE);
    int cframe[RC_FRAME_SIZE] = {0};
    rc_q_read (frame);
    for (int i = 0; i < RC_FRAME_SIZE; i++)
    {
        cframe[i] = (int)frame[i];
    }
    (*env)->SetIntArrayRegion(env, jframe, 0, RC_FRAME_SIZE, cframe);
    return jframe;
}

jintArray q_memread (JNIEnv* env, jobject obj)
{
    jintArray jframe = (*env)->NewIntArray(env, RC_FRAME_SIZE);
    int cframe[RC_FRAME_SIZE] = {0};
    rc_q_memread (frame);
    for (int i = 0; i < RC_FRAME_SIZE; i++)
    {
        cframe[i] = (int)frame[i];
    }
    (*env)->SetIntArrayRegion(env, jframe, 0, RC_FRAME_SIZE, cframe);
    return jframe;
}

jintArray q_save (JNIEnv* env, jobject obj)
{
    jintArray jframe = (*env)->NewIntArray(env, RC_FRAME_SIZE);
    int cframe[RC_FRAME_SIZE] = {0};
    rc_q_save(frame, current_gpsreadout, current_datetime, current_radiation, RC_EC_OK);
    for (int i = 0; i < RC_FRAME_SIZE; i++)
    {
        cframe[i] = (int)frame[i];
    }
    (*env)->SetIntArrayRegion(env, jframe, 0, RC_FRAME_SIZE, cframe);
    return jframe;
}

jintArray r_read(JNIEnv *env, jobject obj)
{
    jintArray jframe = (*env)->NewIntArray(env, RC_FRAME_SIZE);
    int cframe[RC_FRAME_SIZE] = {0};
    rc_r_read(frame, current_gpsreadout, (const rcdt_t*)(&current_datetime), current_radiation, RC_EC_OK);
    for (int i = 0; i < RC_FRAME_SIZE; i++)
    {
        cframe[i] = (int)frame[i];
    }
    (*env)->SetIntArrayRegion(env, jframe, 0, RC_FRAME_SIZE, cframe);
    return jframe;
}

jintArray process_read(JNIEnv *env, jobject obj, jintArray array)
{
    jintArray jframe = (*env)->NewIntArray(env, RC_FRAME_SIZE);
    jsize len = (*env)->GetArrayLength(env, array);
    jint *body = (*env)->GetIntArrayElements(env, array, 0);
    rcfdataupck_t fdata;

    if (len > RC_FRAME_SIZE)
    {
        /* log error - frame too big */
        goto Java_JRadicom_process_1read_finish;
    }

    unsigned char tframe[RC_FRAME_SIZE];
    for (int i = 0; i < len; i++)
    {
        tframe[i] = (unsigned char)body[i];
    }
    if (RC_OK != rc_process_read(tframe, &fdata))
    {
        /* log error */
        goto Java_JRadicom_process_1read_finish;
    }

    int cframe[RC_FRAME_SIZE] = {0};
    for (int i = 0; i < RC_GPS_DATALEN; i++)
    {
        cframe[i] = (int)fdata.gpsdata[i];
    }
    cframe[RC_GPS_DATALEN] = (int)fdata.day;
    cframe[RC_GPS_DATALEN+1] = (int)fdata.month;
    cframe[RC_GPS_DATALEN+2] = (int)fdata.year;
    cframe[RC_GPS_DATALEN+3] = (int)fdata.hours;
    cframe[RC_GPS_DATALEN+4] = (int)fdata.minutes;
    cframe[RC_GPS_DATALEN+5] = (int)fdata.seconds;
    cframe[RC_GPS_DATALEN+6] = (int)fdata.radiation;
    (*env)->SetIntArrayRegion(env, jframe, 0, RC_FRAME_SIZE, cframe);

    Java_JRadicom_process_1read_finish:
    return jframe;
}

#ifdef __cplusplus
};
#endif /* __cplusplus */