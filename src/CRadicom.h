#ifndef _CRADICOM_H
#define _CRADICOM_H

#include <jni.h>

jintArray read_hdr (JNIEnv* env, jobject obj, jintArray array);
jintArray q_read (JNIEnv* env, jobject obj);
jintArray r_read(JNIEnv *env, jobject obj);
jintArray process_read(JNIEnv *env, jobject obj, jintArray array);

#endif /* _CRADICOM_H */