/* æ–‡ä»¶åç§°ï¼šring_buffer.h
 * æ‘˜è¦ï¼š
 *  
 * ä¿®æ”¹å†å²      ç‰ˆæœ¬å·      Author      ä¿®æ”¹å†…å®¹
 *--------------------------------------------------
 * 2024.11.3      v01        Lizile      åˆ›å»ºæ–‡ä»¶
 *--------------------------------------------------
*/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stm32f1xx_hal.h"   /* HALåº“ç›¸å…³ */

#define BUFFER_SIZE  1024    /* ç¯å½¢ç¼“å†²åŒºå¤§å° */

/* å®šä¹‰ç¯å½¢ç¼“å†²åŒº */
typedef struct
{
	unsigned char buffer[BUFFER_SIZE];  /* ç¯å½¢ç¼“å†²åŒºç©ºé—´ */
	volatile unsigned int pW;           /* å†™ä½ç½® */
	volatile unsigned int pR;           /* è¯»ä½ç½® */
} ring_buffer;

/* º¯ÊıÃû  £º ring_buffer_init
 * ÊäÈë²ÎÊı£º dst_buf --> Ö¸ÏòÄ¿±ê»·ĞÎ»º³åÇø
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ»ØÖµ  £º ÎŞ
 * º¯Êı¹¦ÄÜ£º ³õÊ¼»¯»º³åÇø
*/
extern void ring_buffer_init(ring_buffer *dst_buf);

/* º¯ÊıÃû  £º ring_buffer_write
 * ÊäÈë²ÎÊı£º dst_buf --> Ö¸ÏòÄ¿±ê»·ĞÎ»º³åÇø
 *            c --> ÒªĞ´ÈëµÄÊı¾İ
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ»ØÖµ  £º ÎŞ
 * º¯Êı¹¦ÄÜ£º ÏòÄ¿±ê»·ĞÎ»º³åÇøĞ´ÈëÒ»¸ö×Ö½ÚµÄÊı¾İ£¬Èç¹û»º³åÇøÂú£¬Ôò¶ªÆú´ËÊı¾İ
*/
extern void ring_buffer_write(unsigned char c, ring_buffer *dst_buf);

/* º¯ÊıÃû  £º ring_buffer_read
 * ÊäÈë²ÎÊı£º dst_buf --> Ö¸ÏòÄ¿±ê»·ĞÎ»º³åÇø
 *            c --> ´ÓÄ¿±ê»·ĞÎ»º³åÇø¶Áµ½µÄÊı¾İ±£´æµ½ÄÚ´æÖĞÄÄ¸öµØÖ·
 * Êä³ö²ÎÊı£º ÎŞ
 * ·µ»ØÖµ  £º ¶ÁÈ¡µ½Êı¾İ£¬·µ»Ø 0£»·ñÔò£¬·µ»Ø -1
 * º¯Êı¹¦ÄÜ£º ´ÓÄ¿±ê»·ĞÎ»º³åÇø¶ÁÈ¡Ò»¸ö×Ö½ÚµÄÊı¾İ±£´æµ½ÄÚ´æÖĞ£¬    \
              Èç¹ûÄ¿±ê»·ĞÎ»º³åÇøÒÑ¿Õ£¬·µ»Ø -1£¬±íÊ¾¶ÁÈ¡Ê§°Ü
*/
extern int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf);

#endif /* __RING_BUFFER_H */

