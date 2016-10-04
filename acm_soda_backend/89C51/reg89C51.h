#ifndef REG89C51_H
#define REG89C51_H

/* For a byte register: sfr <name> = 0x00;
 * For a bit register: sbit <name> = 0x00;
 */

// bank 0
sfr R0 = 0x00;
sfr R1 = 0x01;
sfr R2 = 0x02;
sfr R3 = 0x03;
sfr R4 = 0x04;
sfr R5 = 0x05;
sfr R6 = 0x06;
sfr R7 = 0x07;

// bank 1
sfr R0b1 = 0x08;
sfr R1b1 = 0x09;
sfr R2b1 = 0x0A;
sfr R3b1 = 0x0B;
sfr R4b1 = 0x0C;
sfr R5b1 = 0x0D;
sfr R6b1 = 0x0E;
sfr R7b1 = 0x0F;

// bank 2
sfr R0b2 = 0x10;
sfr R1b2 = 0x11;
sfr R2b2 = 0x12;
sfr R3b2 = 0x13;
sfr R4b2 = 0x14;
sfr R5b2 = 0x15;
sfr R6b2 = 0x16;
sfr R7b2 = 0x17;


// bank 3
sfr R0b3 = 0x18;
sfr R1b3 = 0x19;
sfr R2b3 = 0x1A;
sfr R3b3 = 0x1B;
sfr R4b3 = 0x1C;
sfr R5b3 = 0x1D;
sfr R6b3 = 0x1E;
sfr R7b3 = 0x1F;

sfr ACC = 0xE0; // Accumulator
sfr B   = 0xF0; // B register
sfr PSW = 0xD0; // Program status word

sfr DPL = 0x82; // DPTR low byte
sfr DPH = 0x83; // DPTR high byte

/* IO pins */
sfr P0 = 0x80;
sfr P1 = 0x90;
sfr P2 = 0xA0;
sfr P3 = 0xB0;

sfr IP = 0xB8; // Interrupt priority register
sfr IE = 0xA8; // Interrupt enable register

sfr TMOD  = 0x89; // Timer mode register
sfr TCON  = 0x88; // Timer control register
sfr T2MOD = 0xC9; // Timer 2 mode register
sfr T2CON = 0xC8; // Timer 2 control register

sfr TH0 = 0x8C; // timer 0
sfr TL0 = 0x8A;
sfr TH1 = 0x8D; // timer 1
sfr TL1 = 0x8B;
sfr TH2 = 0xCD; // timer 2
sfr TL2 = 0xCC;

sfr SCON = 0x98; // serial control
sfr SBUF = 0x99; // serial buffer
sfr PCON = 0x87; // power control

#endif
