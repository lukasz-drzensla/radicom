#ifndef _CRADICOM_H
#define _CRADICOM_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

jintArray read_hdr (JNIEnv* env, jobject obj, jintArray array);
jintArray q_read (JNIEnv* env, jobject obj);
jintArray r_read(JNIEnv *env, jobject obj);
jintArray process_read(JNIEnv *env, jobject obj, jintArray array);

#ifdef __cplusplus
};
#endif

#endif /* _CRADICOM_H */