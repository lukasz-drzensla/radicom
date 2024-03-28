#include <jni.h>
#include "CRadicom.h"
#include "jradicom_JRadicom.h"

/* functions created by JNI are only wrappers for internal functions as their names depend on package name etc*/
JNIEXPORT jintArray JNICALL Java_jradicom_JRadicom_read_1header (JNIEnv *env, jobject obj, jintArray array)
{
    return read_hdr(env, obj, array);
}

JNIEXPORT jintArray JNICALL Java_jradicom_JRadicom_q_1read (JNIEnv *env, jobject obj)
{
    return q_read(env, obj);
}

JNIEXPORT jintArray JNICALL Java_jradicom_JRadicom_r_1read (JNIEnv *env, jobject obj)
{
    return r_read(env, obj);
}

JNIEXPORT jintArray JNICALL Java_jradicom_JRadicom_process_1read (JNIEnv *env, jobject obj, jintArray array)
{
    return process_read(env, obj, array);
}

JNIEXPORT jintArray JNICALL Java_jradicom_JRadicom_q_1memread (JNIEnv *env, jobject obj)
{
    return q_memread(env, obj);
}