/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:31:33
 * @LastEditTime: 2020-04-27 17:04:46
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 *--------------------------------------------------
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.6      v01        Lizile      修改文件
 *--------------------------------------------------
 */

#ifndef _PLATFORM_MUTEX_H_
#define _PLATFORM_MUTEX_H_

#include "FreeRTOS.h"
#include "semphr.h"

typedef struct platform_mutex {
    SemaphoreHandle_t mutex;
} platform_mutex_t;

int platform_mutex_init(platform_mutex_t* m);
int platform_mutex_lock(platform_mutex_t* m);
int platform_mutex_lock_timeout(platform_mutex_t* m, int timeout);  /* Lizile Add */
int platform_mutex_trylock(platform_mutex_t* m);
int platform_mutex_unlock(platform_mutex_t* m);
int platform_mutex_destroy(platform_mutex_t* m);

#endif