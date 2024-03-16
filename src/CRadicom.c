#include <jni.h>
#include <stdio.h>
#include "JRadicom.h"   

unsigned char frame[100] = {6};

#define JHDR_LEN 4

JNIEXPORT jintArray JNICALL Java_JRadicom_read_1header (JNIEnv *env, jobject obj, jintArray array)
{
    jsize len = (*env)->GetArrayLength(env, array);
    jint *body = (*env)->GetIntArrayElements(env, array, 0);
    unsigned char header[2];
    header[0] = (unsigned char)body[0];
    header[1] = (unsigned char)body[1];

    //rc_read_header(header, hdr);

    jintArray jhdr = (*env)->NewIntArray(env, JHDR_LEN);
    int chdr[JHDR_LEN] = {0};
    chdr[0] = 1;
    chdr[1] = 0;
    chdr[2] = 0;
    chdr[3] = 3;

    (*env)->SetIntArrayRegion(env, jhdr, 0, JHDR_LEN, chdr);

    return jhdr;
}

JNIEXPORT jintArray JNICALL Java_JRadicom_q_1read (JNIEnv *env, jobject obj)
{
    jintArray my_jarray = (*env)->NewIntArray(env, 100);
    int my_array[100] = {0};
    for (int i = 0; i < 100; i++)
    {
        my_array[i] = (int)frame[i];
    }
    (*env)->SetIntArrayRegion(env, my_jarray, 0, 100, my_array);

    return my_jarray;
}

JNIEXPORT void JNICALL Java_JRadicom_print
  (JNIEnv *env, jobject obj, jint x){
    printf("Hello World %d!\n", (int)x);

    jclass javaDataClass = (*env)->FindClass(env, "JRadicom");
    jfieldID countField = (*env)->GetFieldID(env, javaDataClass, "count", "I");
    int y = (*env)->GetIntField(env, obj, countField);

    printf("Got hands on java class: %d\n", y);

    (*env)->SetIntField(env, obj, countField, 7);

    y = (*env)->GetIntField(env, obj, countField);

    printf("Got hands on java class again!: %d\n", y);

    return;
}

int main()
{
    printf("Hello from C!\n");
    return 0;
}