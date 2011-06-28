#include "sa.h"

unsigned bkgd = 0;
unsigned rti = 0;
unsigned tof = 0;

void
_start(void)
{
	/* set stack pointer just below EEPROM */
	asm ("lds #$effe");

	/* no COP */
	COPCTL = 0x00;

	/* setup internal resources.
	 * 1st write to these locks them permanently.
	 * vectors at ff80..ffff
	 */
	INITRG = 0x00;	/* registers block 0..1ff */
	INITRM = 0x08;	/* on-chip 1k 800..bff */
	INITEE = 0xf1;	/* EEPROM f000..ffff */

	/* start in Normal Single Chip. switch to Normal Exp Wide.
	 * N.B. this by itself will not move EEPROM
	 */
	MODE = 0xe0;
	MODE = 0xe0;

	/* enable chip selects so that:
	 * CS0:  encoder  = 200..2ff
	 * CS1:  motorcon = 300..37f
	 * CS2:  motormon = 380..3ff
	 * CS3:  485 PTT output
	 * CSP0: 8000..ffff
	 * CSP1: motor direction output
	 * CSD:  0000..7fff
	 * no clock stretching.
	 */
	PORTF = 0x37;
	DDRF = 0x48;
	CSCTL0 = 0x37;
	CSCTL1 = 0x10;
	CSSTR0 = 0x00;
	CSSTR1 = 0x00;

	/* enable expanded D and P paging:
	 * PPAGE 7 is always mapped to c000..ffff.
	 * Put PPAGE 0 at 8000..b777
	 * (leaves pages 1..6=96KB of ext mem unused)
	 * DPAGE 0..1f selects one of 32 4k pages in 7000..7fff.
	 * the 7 DPAGES 10..16 also appear at 0000..6777,
	 *   although 0..03ff are covered by registers and CS0/1/2.
	 * so to avoid these, only use DPAGE set to 0..f and 17..1f
	 */
	PPAGE = 0x00;
	DPAGE = 0x00;
	WINDEF = 0xc0;
	MXAR = 0x0f;
	MISC = 0x00;

	/* enable ByteLane and R/W bus signals.
	 * ECLK used for EncReset until next Altera rev, then not used.
	 * ARSIE used for Encoder direction input.
	 */
	PEAR = 0x1c;
	DDRE = 0x10;

	/* PORTH misc outputs,
	 * PORTJ misc inputs.
	 */
	DDRH = 0xff;
	PORTH = 0xff;
	DDRJ = 0x00;

	/* set up RTI @ 1ms */
	RTICTL = 0x81;

	/* set up timer overflow to interrupt @ 16 ms */
	TMSK2 = 0xb1;
	TSCR = 0x80;

	/* go */
	INTR_ON();
	while (1)
	    bkgd++;
}

#pragma interrupt_handler onTOF
void
onTOF (void)
{
	unsigned t0;
	unsigned i;

	/* clear this interrupt then enable other interrupts */
	TFLG2 = 0x80;
	INTR_ON();

	/* burn -- each loop takes about 2us */
	t0 = rti;
	for (i = 0; i < 5000; i++)
	    continue;

	tof = rti - t0;
}

/* clock interrupt: increment clock */
#pragma interrupt_handler onRTI
static void
onRTI (void)
{
	/* clear interrupt */
	RTIFLG = 0x80;

	rti++;

}

#pragma interrupt_handler onStray
static void
onStray(void)
{
}

#pragma abs_address:0xffce
static void (*interrupt_vectors[])(void) = {
    onStray,	/* Key Wakeup H */
    onStray,	/* Key Wakeup J */
    onStray,	/* ATD */
    onStray,	/* SCI 1 */
    onStray,	/* SCI 0 */
    onStray,	/* SPI */
    onStray,	/* PAIE */
    onStray,	/* PAO */
    onTOF,	/* TOF */
    onStray,	/* TC7 */
    onStray,	/* TC6 */
    onStray,	/* TC5 */
    onStray,	/* TC4 */
    onStray,	/* TC3 */
    onStray,	/* TC2 */
    onStray,	/* TC1 */
    onStray,	/* TC0 */
    onRTI,	/* RTI */
    onStray,	/* IRQ */
    onStray,	/* XIRQ */
    onStray,	/* SWI */
    onStray,	/* ILLOP */
    onStray,	/* COP */
    onStray,	/* CLM */
    _start	/* RESET */
};

#pragma end_abs_address

/* For RCS Only -- Do Not Edit
 * @(#) $RCSfile: rtitest.c,v $ $Date: 2001/04/19 21:11:57 $ $Revision: 1.1.1.1 $ $Name:  $
 */
