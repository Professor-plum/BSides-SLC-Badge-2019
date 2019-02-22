#include <WavPlayer.h>
#ifdef USE_FLASH
#include <SerialFlash.h>
SerialFlashFile wavFile;
#else
#include <SdFat.h>
extern SdFat SD;
File wavFile = NULL;
#endif

#define WAVBLOCK	512
#define	WAVDMA 		1

uint8_t wavDMABuf[2*WAVBLOCK]={0};

HardwareTimer wavTimer(4);
int wavChannel;

bool wavPlaying=false;

uint32_t wavOffset=0;

bool bufEmpty[2] = {false, false};

void wavCheckBuff() {
	int j=0;
	for (int i=0; i<2; ++i)
	{
		if (bufEmpty[i]) {
			j++;
			if ((!wavFile) || (!wavFile.available())) {
				wavPlaying = false;
				wavFile.close();
				//memset(&wavDMABuf[WAVBLOCK*i], 0, WAVBLOCK);
				memset(wavDMABuf, 0, WAVBLOCK*2);
			}
			else {
				int r = wavFile.read(&wavDMABuf[WAVBLOCK*i], WAVBLOCK);
				SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT)); //hack
				if (r != WAVBLOCK) {
					memset(&wavDMABuf[WAVBLOCK*i]+r, 0, WAVBLOCK-r);
				}
			}
			bufEmpty[i] =false;
		}
		
	}
	/*if (j==2) {
		Serial.println("catch up!");
	}*/
}

#ifdef WAVDMA

void WAV_ISR(void) {
	dma_irq_cause event = dma_get_irq_cause(DMA1, DMA_CH7);

	if (!wavPlaying) return;

	switch(event) {
	    //the event indicates that the transfer was successfully completed
	    case DMA_TRANSFER_COMPLETE:
	      bufEmpty[1]=true;
	      //wavCheckBuff();
	      break;
	    case DMA_TRANSFER_HALF_COMPLETE:
	   	  bufEmpty[0]=true;
	   	  //wavCheckBuff();
	      break;
	    //the event indicates that there was an error transmitting
	    case DMA_TRANSFER_ERROR:
	      //11. Disable DMA when we are done
	      dma_disable(DMA1,DMA_CH7);
	      break;
	}
}

void DMA_Setup()
{
	dma_init(DMA1);
	dma_disable(DMA1, DMA_CH7);
	uint32_t flags = (DMA_FROM_MEM | DMA_MINC_MODE | DMA_CIRC_MODE | DMA_TRNS_CMPLT | DMA_HALF_TRNS | DMA_TRNS_ERR );
	dma_attach_interrupt(DMA1, DMA_CH7, WAV_ISR);
	dma_setup_transfer(DMA1, DMA_CH7,
		&((TIMER4->regs).gen->CCR1), DMA_SIZE_16BITS, // peripheral
		wavDMABuf, DMA_SIZE_8BITS, // memory
		flags);
	dma_set_priority(DMA1, DMA_CH7, DMA_PRIORITY_LOW);
}

void DMA_start()
{
	dma_disable(DMA1, DMA_CH7);
	dma_set_num_transfers(DMA1, DMA_CH7, 2*WAVBLOCK);
	dma_enable(DMA1, DMA_CH7);
}

#else

void Timer_ISR()
{
	static int wavidx=0;

	if (complete) {
		wavTimer.pause();
		wavPlaying=false;
	}

	pwmWrite(wavChannel, wavDMABuf[wavidx++]);
	if (wavidx == WAVBLOCK) bufEmpty[0]=true;
	else if (wavidx == 2*WAVBLOCK) {
		bufEmpty[1]=true;
		wavidx=0;
	}
}

#endif

void wavInit() {
#if WAVDMA
	DMA_Setup();
#endif
	int prescaler = 6;
	int overflow = 250;
	pinMode(BUZZ,PWM);
	wavChannel = PIN_MAP[BUZZ].timer_channel;
	wavTimer.pause();
  
  	wavTimer.setPrescaleFactor(prescaler);
  	wavTimer.setOverflow(overflow);

#if WAVDMA
	wavTimer.enableDMA(0); 
	DMA_start();
#else
	wavTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    wavTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
	wavTimer.attachCompare1Interrupt(Timer_ISR);
#endif

	wavTimer.refresh();

	wavPlaying = false;
	for (int i=0; i<2*WAVBLOCK; ++i)
		wavDMABuf[i] = 0;
	bufEmpty[0]=bufEmpty[1]=false;

	wavTimer.resume();
}

bool wavPlay(char* filename) {

#ifdef USE_FLASH
	wavFile = SerialFlash.open(filename);
#else
	wavFile = SD.open(filename);
#endif
	if (!wavFile) {
		Serial.print("Can't open ");Serial.println(filename);
		return false;
	}

	/*wavFile.seek(8);
  	char wavStr[] = {'W','A','V','E'};
  	char wavHdr[4];
  	wavFile.read(wavHdr, 4);
  	for (byte i =0; i<4; i++){
  		if(wavHdr[i] != wavStr[i]){
		  Serial.print(filename);Serial.println(" doesn't appear to be a wav file");
		  return false;
		}
	}
	
	byte stereo;
	wavFile.seek(22);
	wavFile.read(&stereo, 1);
	wavFile.seek(24);

    uint16_t sampleRate;
    wavFile.read(&sampleRate, 2);
	//Serial.print("Sample Rate: ");Serial.println(sampleRate);

    //Set bits per sample and stereo/mono
	wavFile.seek(34);
	uint16_t bps;
	wavFile.read(&bps, 2);
	
	//Serial.print("Channels: ");Serial.print(stereo);Serial.print(" bps: ");Serial.println(bps);*/
	wavFile.seek(44);

	/*int prescaler = F_CPU/sampleRate;
	int overflow = prescaler/256;
	prescaler = prescaler/overflow;
	overflow--;

	if (sampleRate == 16000) {
		prescaler=18;//18;
		overflow=250;//250;
	}
	if (sampleRate == 8000) {
		prescaler=36;//18;
		overflow=251;//250;
	}


	pinMode(pin,PWM);
	wavChannel = PIN_MAP[pin].timer_channel;
	wavTimer.pause();
  
  	wavTimer.setPrescaleFactor(prescaler);
  	wavTimer.setOverflow(overflow);
	
	//Serial.print("Prescaler: ");Serial.println(prescaler);
	//Serial.print("Overflow: ");Serial.println(overflow);
#if WAVDMA
	wavTimer.enableDMA(0); 
	DMA_start();
#else
	wavTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    wavTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
	wavTimer.attachCompare1Interrupt(Timer_ISR);
#endif
	wavTimer.refresh();

	complete=false;
	for (int i=0; i<2*WAVBLOCK; ++i)
		wavDMABuf[i] = 0;
	bufEmpty[0]=bufEmpty[1]=false;
	//wavCheckBuff();
	wavTimer.resume();
*/
	SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
      
	wavPlaying = true;
	return true;
}

void wavStop(int pin) {
	//wavTimer.pause();
	pwmWrite(pin, 0);
	wavPlaying=false;
}

bool wavIsPlaying() {
	return wavPlaying;
}