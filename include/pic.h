#ifndef PIC_H
#define PIC_H
void pic_init(void);
void pic_eoi(int irq);
void pic_unmask(int irq);
void pic_mask(int irq);
#define PIC1_CMD 0x20
#define PIC2_CMD 0xA0
#define PIC_EOI 0x20
#endif
