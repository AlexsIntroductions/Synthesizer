/*
 * main.c
 *
 * Created: 8/2/2023 6:45:01 PM
 *  Author: alexp
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define BIT(X) (1<<X)

int16_t sineTable[] = { 
0x800,0x832,0x864,0x896,0x8c8,0x8fa,0x92c,0x95e,
0x98f,0x9c0,0x9f1,0xa22,0xa52,0xa82,0xab1,0xae0,
0xb0f,0xb3d,0xb6b,0xb98,0xbc5,0xbf1,0xc1c,0xc47,
0xc71,0xc9a,0xcc3,0xceb,0xd12,0xd39,0xd5f,0xd83,
0xda7,0xdca,0xded,0xe0e,0xe2e,0xe4e,0xe6c,0xe8a,
0xea6,0xec1,0xedc,0xef5,0xf0d,0xf24,0xf3a,0xf4f,
0xf63,0xf76,0xf87,0xf98,0xfa7,0xfb5,0xfc2,0xfcd,
0xfd8,0xfe1,0xfe9,0xff0,0xff5,0xff9,0xffd,0xffe,
0xfff,0xffe,0xffd,0xff9,0xff5,0xff0,0xfe9,0xfe1,
0xfd8,0xfcd,0xfc2,0xfb5,0xfa7,0xf98,0xf87,0xf76,
0xf63,0xf4f,0xf3a,0xf24,0xf0d,0xef5,0xedc,0xec1,
0xea6,0xe8a,0xe6c,0xe4e,0xe2e,0xe0e,0xded,0xdca,
0xda7,0xd83,0xd5f,0xd39,0xd12,0xceb,0xcc3,0xc9a,
0xc71,0xc47,0xc1c,0xbf1,0xbc5,0xb98,0xb6b,0xb3d,
0xb0f,0xae0,0xab1,0xa82,0xa52,0xa22,0x9f1,0x9c0,
0x98f,0x95e,0x92c,0x8fa,0x8c8,0x896,0x864,0x832,
0x800,0x7cd,0x79b,0x769,0x737,0x705,0x6d3,0x6a1,
0x670,0x63f,0x60e,0x5dd,0x5ad,0x57d,0x54e,0x51f,
0x4f0,0x4c2,0x494,0x467,0x43a,0x40e,0x3e3,0x3b8,
0x38e,0x365,0x33c,0x314,0x2ed,0x2c6,0x2a0,0x27c,
0x258,0x235,0x212,0x1f1,0x1d1,0x1b1,0x193,0x175,
0x159,0x13e,0x123,0x10a,0xf2,0xdb,0xc5,0xb0,
0x9c,0x89,0x78,0x67,0x58,0x4a,0x3d,0x32,
0x27,0x1e,0x16,0xf,0xa,0x6,0x2,0x1,
0x0,0x1,0x2,0x6,0xa,0xf,0x16,0x1e,
0x27,0x32,0x3d,0x4a,0x58,0x67,0x78,0x89,
0x9c,0xb0,0xc5,0xdb,0xf2,0x10a,0x123,0x13e,
0x159,0x175,0x193,0x1b1,0x1d1,0x1f1,0x212,0x235,
0x258,0x27c,0x2a0,0x2c6,0x2ed,0x314,0x33c,0x365,
0x38e,0x3b8,0x3e3,0x40e,0x43a,0x467,0x494,0x4c2,
0x4f0,0x51f,0x54e,0x57d,0x5ad,0x5dd,0x60e,0x63f,
0x670,0x6a1,0x6d3,0x705,0x737,0x769,0x79b,0x7cd 
};

extern void clock_init(void);
void dac_init(void);
void dma_init(void);
void tcc0_init(uint16_t num);
void tcc1_init(void);
void tcc0_disable(void);
void tcc1_disable(void);
void usartd0_init(void);

uint8_t temp;
uint8_t lastChar;

int main(void){
	
	clock_init();
	usartd0_init();
	dac_init();
	dma_init();
	
	PORTC.OUTSET = BIT(7);
	PORTC.DIRSET = BIT(7);
	
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;
	sei();
	
	while(1){
	}
}

void dac_init(void){
	DACA.EVCTRL = DAC_EVSEL_0_gc;
	DACA.CTRLB = DAC_CHSEL_SINGLE1_gc | DAC_CH1TRIG_bm;
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	DACA.CTRLA = DAC_CH1EN_bm | DAC_ENABLE_bm;
}

void tcc1_init(void){
	TCC1.INTCTRLA = TC_OVFINTLVL_MED_gc;
	//TCC1.PER = 50000;
	TCC1.PER = 2500;
	TCC1.CTRLA = TC_CLKSEL_DIV1024_gc;
	//TCC1.CTRLA = TC_CLKSEL_DIV1_gc;
}

void tcc0_init(uint16_t num){
	TCC0.PER = num;
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
}

void tcc0_disable(void){
	TCC0.CTRLA = TC_CLKSEL_OFF_gc;
}

void tcc1_disable(void){
	TCC1.CTRLA = TC_CLKSEL_OFF_gc;
}

void dma_init(void){
	DMA.CTRL |= DMA_RESET_bm;
	
	DMA.CH0.REPCNT = 0;
	
	DMA.CH0.CTRLA =  DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_2BYTE_gc | DMA_CH_REPEAT_bm;
	
	DMA.CH0.ADDRCTRL = DMA_CH_SRCRELOAD_BLOCK_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_INC_gc;
	
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_EVSYS_CH0_gc;
	
	DMA.CH0.TRFCNT = (uint16_t)(sizeof(sineTable));

	DMA.CH0.SRCADDR0 = (uint8_t)((uintptr_t) sineTable);
	DMA.CH0.SRCADDR1 = (uint8_t)(((uintptr_t) sineTable) >> 8);
	DMA.CH0.SRCADDR2 = (uint8_t)((uint32_t)((uintptr_t) sineTable) >> 16);
	
	DMA.CH0.DESTADDR0 = (uint8_t)((uintptr_t)&DACA.CH1DATA);
	DMA.CH0.DESTADDR1 = (uint8_t)(((uintptr_t)&DACA.CH1DATA) >> 8);
	DMA.CH0.DESTADDR2 = (uint8_t)((uint32_t)((uintptr_t)&DACA.CH1DATA) >> 16);
	
	DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm;
	DMA.CTRL |= DMA_ENABLE_bm;
}

void usartd0_init(void){
	PORTD.OUTSET = BIT(3);
	PORTD.DIRSET = BIT(3);
	int8_t BSCALE = -3;
	uint16_t BSEL = 129;
	USARTD0.BAUDCTRLA = (uint8_t)BSEL;
	USARTD0.BAUDCTRLB = ((uint8_t)(BSCALE<<4) | (BSEL >> 8));
	USARTD0.CTRLA = USART_RXCINTLVL_HI_gc;
	USARTD0.CTRLC = (USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_ODD_gc | USART_CHSIZE_8BIT_gc);
	USARTD0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

ISR(USARTD0_RXC_vect){
	temp = USARTD0.DATA;
	//what needs to happen
	
	//when a key is pressed
		//if it is a new key or there isnt a timer going start one
	if(temp != lastChar){
		lastChar = temp;
		//c6
		if(temp == 'e'){
			tcc0_init(121);
		}
		else if(temp == '4'){
			tcc0_init(114);
		}
		else if(temp == 'r'){
			tcc0_init(107);
		}
		else if(temp == '5'){
			tcc0_init(100);
		}
		else if(temp == 't'){
			tcc0_init(96);
		}
		else if(temp == 'y'){
			tcc0_init(90);
		}
		else if(temp == '7'){
			tcc0_init(85);
		}
		else if(temp == 'u'){
			tcc0_init(80);
		}
		else if(temp == '8'){
			tcc0_init(76);
		}
		else if(temp == 'i'){
			tcc0_init(71);
		}
		else if(temp == '9'){
			tcc0_init(67);
		}
		else if(temp == 'o'){
			tcc0_init(62);
		}
		else if(temp == 'p'){
			tcc0_init(60);
		}
		tcc1_init();
		TCC1.CNT = 0;
	}
	else if(temp == lastChar){
		TCC1.CNT = 0;
	}
}

ISR(TCC1_OVF_vect){
	tcc0_disable();
	tcc1_disable();
	lastChar = 0;
}