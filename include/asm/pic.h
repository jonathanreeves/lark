/* 
 * reference used:
 * http://www.mega-tokyo.com/osfaq2/index.php
 * under "Can I remap the PIC"
 * and http://www.beyondlogic.com/interrupts
 */

#ifndef __PIC_H__
#define __PIC_H__

/* ports needed for programming */
#define PIC_MASTER_CMD_PORT  0x20
#define PIC_MASTER_DATA_PORT 0x21
#define PIC_SLAVE_CMD_PORT   0xa0
#define PIC_SLAVE_DATA_PORT  0xa1

/* possible bits for ICW1 */
#define PIC_ICW1_MAND        0x10 /* must always be used */
#define PIC_ICW1_POS_TRIGGER    3  /* 0 = edge trigger, 1 = level */
#define PIC_ICW1_POS_INTERVAL   2  /* 0 = interval 8, 1 = 4 */
#define PIC_ICW1_POS_SERIES     1  /* 0 = cascade, 1 = single */
#define PIC_ICW1_POS_ICW4       0  /* will or will not send ICW4 */

/* ICW2 is used to set the irq locations */

/* possible bits for master ICW3 */
#define PIC_MSTR_ICW3_POS_IR7     7 /* IR7 connected to slave */
#define PIC_MSTR_ICW3_POS_IR6     6 /* IR6 connected to slave */
#define PIC_MSTR_ICW3_POS_IR5     5 /* etc... */
#define PIC_MSTR_ICW3_POS_IR4     4
#define PIC_MSTR_ICW3_POS_IR3     3
#define PIC_MSTR_ICW3_POS_IR2     2 
#define PIC_MSTR_ICW3_POS_IR1     1
#define PIC_MSTR_ICW3_POS_IR0     0

/* possible values for slave ICW3 (no individual bits) */
#define PIC_SLAVE_ICW3_ID7     7 /* slave has ID 7 */
#define PIC_SLAVE_ICW3_ID6     6 /* slave has ID 6 */
#define PIC_SLAVE_ICW3_ID5     5 /* etc... */
#define PIC_SLAVE_ICW3_ID4     4
#define PIC_SLAVE_ICW3_ID3     3
#define PIC_SLAVE_ICW3_ID2     2 
#define PIC_SLAVE_ICW3_ID1     1
#define PIC_SLAVE_ICW3_ID0     0

/* possible bits for ICW4 */
#define PIC_ICW4_POS_NESTING   4  /* 0 = not special fully, 1 = ... */
#define PIC_ICW4_POS_BUF       2  /* 0 = non, 2 = slave, 3 = master */
#define PIC_ICW4_POS_EOI       1  /* 1 = auto EOI, 0 = normal */
#define PIC_ICW4_POS_MODE      0  /* 1 = 8086 mode, 0 = MCS-80 */
/* buffering in is handled differently */
#define PIC_ICW4_NON_BUF       0
#define PIC_ICW4_SLV_BUF       2
#define PIC_ICW4_MSTR_BUF      3

/* must follow with operation command word */
#define PIC_OCW1_MASK_ALL      0xff

#define PIC_EOI              0x20 /* end of inteerrupt */

/* now define the default configuration for the PICS */

/* ICW1: edge triggered, interval of 8, cascaded, sending ICW4 */
#define PIC_ICW1_DEFAULT \
	( PIC_ICW1_MAND | (1<<PIC_ICW1_POS_ICW4) | (0<<PIC_ICW1_POS_SERIES) | \
	 (0<<PIC_ICW1_POS_INTERVAL) | (0<<PIC_ICW1_POS_TRIGGER) )


/* ICW4: not special fully nested, non-buffered, normal EOI, 8086 */
#define PIC_ICW4_DEFAULT \
	( (1<<PIC_ICW4_POS_MODE) | (0<<PIC_ICW4_POS_EOI) | \
	  (PIC_ICW4_NON_BUF<<PIC_ICW4_POS_BUF) | (0<<PIC_ICW4_POS_NESTING) )

#ifndef __ASM__

#endif

#endif
