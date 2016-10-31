#include <arch/idt.h>
#include <arch/ports.h>
#include <arch/interrupts.h>

IDTR idtr;

void init_idt() {

    _cli();

	_get_idtr(&idtr);

	// Remap the irq table.
    // Initialize master PIC
    _outport(0x20, 0x11);
    // Initialize slave PIC
    _outport(0xA0, 0x11);
    // Tell the master PIC that interrupt 0x20 should be mapped to IRQ 0
    _outport(0x21, 0x20);
    // Tell the slace PIC that interrupt 0x28 should be mapped to IRQ 8
    _outport(0xA1, 0x28);
    // Tell the master PIC that IRQ line 2 is mapped to the slave PIC
    _outport(0x21, 0x04);
    // Tell the slave PIC that IRQ line 2 is mapped to the master PIC
    _outport(0xA1, 0x02);
    // Setting bit 0 enables 80x86 mode
    _outport(0x21, 0x01);
    // Setting bit 0 enables 80x86 mode
    _outport(0xA1, 0x01);
    // All done - Null out the data registers
    _outport(0x21, 0x0);
    _outport(0xA1, 0x0);

    

	_outport(PIC_DATA_PORT, 0xf8); // 1111 1000
	_outport(PIC2_DATA_PORT, 0xef); // 1110 1111

    _sti();
}


void idt_set_handler(byte entry, ddword handler) {

	_cli();

	_load_handler(entry, handler, idtr.base);

	_sti();

}
