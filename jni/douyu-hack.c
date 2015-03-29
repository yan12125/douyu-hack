/*
 *  Collin's Dynamic Dalvik Instrumentation Toolkit for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *
 *  (c) 2012,2013
 *
 *  License: LGPL v2.1
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <sys/epoll.h>

#include <jni.h>
#include <stdlib.h>

#include "log.h"
#include "hook.h"
#include "dexstuff.h"
#include "dalvik_hook.h"
#include "base.h"

#define my_log(...) \
        {FILE *fp = fopen("/data/local/tmp/douyu-hack.log", "a+"); if (fp) {\
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);}}

static struct hook_t eph;
static struct dexstuff_t d;

static int debug;

static void my_log2(char *msg)
{
    if (debug)
        my_log("%s", msg)
}

static struct dalvik_hook_t sb1;

// helper function
void printString(JNIEnv *env, jobject str, char *l)
{
    const char *s = (*env)->GetStringUTFChars(env, str, 0);
    if (s) {
        my_log("%s%s\n", l, s)
        (*env)->ReleaseStringUTFChars(env, str, s); 
    } else {
        my_log("%s: unable to get string\n", l);
    }
}

// patches
static void* sb1_tostring(JNIEnv *env, jclass clazz, jstring str)
{
    dalvik_prepare(&d, &sb1, env);

    void *res = (*env)->CallStaticObjectMethod(clazz, sb1.mid, str);
    my_log("success calling : %s\n", sb1.method_name)
    dalvik_postcall(&d, &sb1);

    printString(env, str, "input = ");
    printString(env, res, "output = ");

    return res;
}

void do_patch()
{
    my_log("do_patch()\n")

    dalvik_hook_setup(&sb1, "Ltv/douyu/misc/util/MD5;", "a",  "(Ljava/lang/String;)Ljava/lang/String;", 1, sb1_tostring);
    dalvik_hook(&d, &sb1);
}

static int my_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    int (*orig_epoll_wait)(int epfd, struct epoll_event *events, int maxevents, int timeout);
    orig_epoll_wait = (void*)eph.orig;
    // remove hook for epoll_wait
    hook_precall(&eph);

    // resolve symbols from DVM
    dexstuff_resolv_dvm(&d);
    // insert hooks
    do_patch();
    
    // call dump class (demo)
    dalvik_dump_class(&d, "Ltv/douyu/misc/util/MD5;");
        
    // call original function
    int res = orig_epoll_wait(epfd, events, maxevents, timeout);    
    return res;
}

// set my_init as the entry point
void __attribute__ ((constructor)) my_init(void);

void my_init(void)
{
    my_log("libdouyu-hack: started\n")
 
    // set to 1 to turn on, this will be noisy
    debug = 1;

    // set log function for  libbase (very important!)
    set_logfunction(my_log2);
    // set log function for libdalvikhook (very important!)
    dalvikhook_set_logfunction(my_log2);

    hook(&eph, getpid(), "libc.", "epoll_wait", my_epoll_wait, 0);
}
