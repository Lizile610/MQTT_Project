/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:27:19
 * @LastEditTime: 2020-04-27 22:22:27
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 *--------------------------------------------------
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.6      v01        Lizile      修改文件
 *--------------------------------------------------
 */
#include "platform_mutex.h"

/* 创建互斥量 */
int platform_mutex_init(platform_mutex_t* m)
{
//    m->mutex = xSemaphoreCreateMutex();
    m->mutex = xSemaphoreCreateBinary ();  // 用二进制型信号量来模拟互斥量
    xSemaphoreGive(m->mutex);              // 模拟互斥量初始值为 1 的特性
    return 0;
}

/* 永远等待互斥量，若取得互斥量，上锁 */
int platform_mutex_lock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, portMAX_DELAY);
	
}

/* Lizile Add
 * 一定时间内等待互斥量，若取得互斥量，上锁 
 */
int platform_mutex_lock_timeout(platform_mutex_t* m, int timeout)
{
    return xSemaphoreTake(m->mutex, timeout);
}

int platform_mutex_trylock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, 0);
}

/* 释放互斥量，解锁 */
int platform_mutex_unlock(platform_mutex_t* m)
{
    return xSemaphoreGive(m->mutex);
}

/* Lizile Add
 *在中断里释放互斥量，解锁 
 */
int platform_mutex_unlock_fromISR(platform_mutex_t* m)
{
	/* xHigherPriorityTaskWoken：一个标志，可以表示是否有高优先级的任务被唤醒
	 * 该标志需要初始化为 pdFALSE，只有当释放信号量导致有高优先级的任务被唤醒时，该标志才会变为 pdTRUE 
	 */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;  
	
    xSemaphoreGiveFromISR(m->mutex, &xHigherPriorityTaskWoken);  /* 释放信号量 */

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  /* 如果有高优先级的任务被唤醒了，在中断结束时切换到该任务 */

	return pdTRUE;  /* 如果成功，返回 pdTRUE */
}

/* 删除互斥量 */
int platform_mutex_destroy(platform_mutex_t* m)
{
    vSemaphoreDelete(m->mutex);
    return 0;
}
