/**
 * Модуль, содержащий процедуры настройки устройства.
 */

#include "hardware.h"
#include "../ecu_global_defines.h"


/**
 * Настройка каналов GPIO
 */
void EC_Hardware::ConfigurePins(void)
{
	// configure default GPIO
	InitGpio();

	EALLOW;

	// GPIO84 - xint1 - датчик Холла
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0;         // GPIO
	GpioCtrlRegs.GPCDIR.bit.GPIO84 = 0;          // input
	GpioCtrlRegs.GPCQSEL2.bit.GPIO84 = 0;        // ?? было XINT1 Synch to SYSCLKOUT only

	// GPIO86 - xint2 - датчик Холла
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;         // GPIO
	GpioCtrlRegs.GPCDIR.bit.GPIO86 = 0;          // input
	GpioCtrlRegs.GPCQSEL2.bit.GPIO86 = 0;        // ?? было XINT1 Synch to SYSCLKOUT only

	// 161-164 - индикаторы прерываний и др. состояний
	GpioDataRegs.GPFDAT.bit.GPIO161 = 0;         // Load the output latch
	GpioCtrlRegs.GPFMUX1.bit.GPIO161 = 0;        // GPIO
	GpioCtrlRegs.GPFDIR.bit.GPIO161 = 1;         // output

	GpioDataRegs.GPFDAT.bit.GPIO162 = 0;         // Load the output latch
	GpioCtrlRegs.GPFMUX1.bit.GPIO162 = 0;        // GPIO
	GpioCtrlRegs.GPFDIR.bit.GPIO162 = 1;         // output

	GpioDataRegs.GPFDAT.bit.GPIO163 = 0;         // Load the output latch
	GpioCtrlRegs.GPFMUX1.bit.GPIO163 = 0;        // GPIO
	GpioCtrlRegs.GPFDIR.bit.GPIO163 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO1 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO2 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO3 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;         // output

	GpioDataRegs.GPFDAT.bit.GPIO164 = 0;         // Load the output latch
	GpioCtrlRegs.GPFMUX1.bit.GPIO164 = 0;        // GPIO
	GpioCtrlRegs.GPFDIR.bit.GPIO164 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO0 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO4 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO5 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO6 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO7 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO8 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;         // output

	GpioDataRegs.GPADAT.bit.GPIO12 = 0;         // Load the output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;        // GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;         // output

	GpioDataRegs.GPBDAT.bit.GPIO34 = 0;         // Load the output latch
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;        // GPIO
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;         // output


	// Disable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1;   // Disable pull-up on GPIO20 (EPWM11a)
	GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;   // Disable pull-up on GPIO21 (EPWM11b)

	// Configure EPWM11 pins
	GPIO_SetupPinMux(20, GPIO_MUX_CPU1, 5);	// EPWM11a
	GPIO_SetupPinMux(21, GPIO_MUX_CPU1, 5);	// EPWM11b
	EDIS;
}

/**
 * Настройка прерываний
 */
void EC_Hardware::ConfigureInterrupts(void)
{
	// Clear all interrupts and initialize PIE vector table:
	DINT;
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	// настройка внешних прерываний
	EALLOW;
	PieVectTable.XINT1_INT = &xint1_isr;
	if (HALL_CRANKSHAFT)
		PieVectTable.XINT2_INT = &xint2_isr;
	PieVectTable.DCANA_1_INT = &canHwIsr;

	PieVectTable.TIMER1_INT = &cpu_timer1_isr;
	PieVectTable.TIMER2_INT = &cpu_timer2_isr;
	EDIS;
}

void EC_Hardware::ConfigureTimer(void)
{
	InitCpuTimers();   // For this example, only initialize the Cpu Timers
	ConfigCpuTimer(&CpuTimer0, TIMER_FREQ, S2US);

	CpuTimer0Regs.TCR.bit.TIE = 0;	// выключено прерывание таймера
	CpuTimer0Regs.TCR.bit.FREE = 0;
	CpuTimer0Regs.TCR.bit.TSS = 0;

	CpuTimer1Regs.TCR.bit.TIE = 1;	// включено прерывание таймера
	CpuTimer1Regs.TCR.bit.FREE = 0;
	CpuTimer1Regs.TCR.bit.TSS = 0;
	CpuTimer1Regs.PRD.all = TIM1_DIV*TIMER_FREQ;
	CpuTimer1Regs.TCR.bit.TRB = 1;

	CpuTimer2Regs.TCR.bit.TIE = 1;	// включено прерывание таймера
	CpuTimer2Regs.TCR.bit.FREE = 0;
	CpuTimer2Regs.TCR.bit.TSS = 0;
	CpuTimer2Regs.PRD.all = TIMER_FREQ*TIM2_DIV;
	CpuTimer2Regs.TCR.bit.TRB = 1;
}

/**
 * Настройка АЦП
 */
void EC_Hardware::ConfigureADC(void)
{
	/*EALLOW;
	//adc-a
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; 			// set ADCCLK divider to /4
	AdcaRegs.ADCCTL2.bit.RESOLUTION = 0; 		// 12-bit resolution
	AdcaRegs.ADCCTL2.bit.SIGNALMODE = 0; 		// single-ended channel conversions (12-bit mode only)
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;		// Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;			// power up the ADC
	DELAY_US(1000);								// delay for 1ms to allow ADC time to power up
	EDIS;*/
}

/**
 * Настройка ШИМ
 * ШИМ надо подать на ePWM11a - GPIO20. Частота подаваемого ШИМа должна быть в 32 раза больше,
 * чем необходимая частота ШИМа для управления форсунками.
 */
void EC_Hardware::ConfigureEPWM(void)
{
	EALLOW;

	// 200MHz - sysclk
	// div = 2
	// hdiv = 2
	// TBCLK = 50MHz
	// prd = 10
	// Fpwm = 5MHz - out to CPLD
	// 156.25kHz - out to injector

	// Setup TBCLK
	EPwm11Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm11Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm11Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
	EPwm11Regs.TBCTL.bit.CLKDIV = TB_DIV2;
	EPwm11Regs.TBPRD = 10;       				// Set timer period
	EPwm11Regs.TBPHS.bit.TBPHS = 0x0000;       // Phase is 0
	EPwm11Regs.TBCTR = 0x0000;                  // Clear counter

	// Setup shadow register load on ZERO
	EPwm11Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm11Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm11Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm11Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm11Regs.CMPA.bit.CMPA = 5;    // Set compare A value
	EPwm11Regs.CMPB.bit.CMPB = 5;    // Set Compare B value

	// Set actions
	EPwm11Regs.AQCTLA.bit.ZRO = AQ_CLEAR;        // Clear PWM11A on Zero
	EPwm11Regs.AQCTLA.bit.CAU = AQ_SET;          // Set PWM11A on event A, up count

	EDIS;
}

/**
 * Настройка ЦАП
 */
void EC_Hardware::ConfigureDAC(void)
{
    /*EALLOW;
    // dac-b
    DacbRegs.DACCTL.bit.DACREFSEL = 1;			// use ADC references
    DacbRegs.DACCTL.bit.MODE = 0;				// gain = x1
    DacbRegs.DACCTL.bit.LOADMODE = 0;			// load on next SYSCLK
    DacbRegs.DACVALS.all = 0x0800;				// set mid-range
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;			// enable DAC
    EDIS;*/
}

/**
 * Настройка опроса АЦП
 */
void EC_Hardware::SetupADC(void)
{
	/*EALLOW;
	// adc-a
	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;  		// SOC0 will convert pin A3
	AdcaRegs.ADCSOC0CTL.bit.ACQPS = 14; 		// sample window is 100 SYSCLK cycles
	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 1; 		// trigger on CPU1 Timer 0, TINT0n
	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; 		// end of SOC0 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   		// enable INT1 flag
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; 		// make sure INT1 flag is cleared
	EDIS;*/
}

void EC_Hardware::SetupInterrupts(void)
{
	// Enable CPU INT1 which is connected to CPU-Timer 0:
	IER |= M_INT1; 			// Enable group 1 interrupts
	IER |= M_INT13;
	IER |= M_INT14;

	// enable PIE interrupt
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4
	PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Gropu 1 INT5
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;

	IER |= 0x0100;

	// Enable global Interrupts and higher priority real-time debug events:
	EINT;  					// Enable Global interrupt INTM
	ERTM;  					// Enable Global real-time interrupt DBGM

	// связывание внешних прерываний
	GPIO_SetupXINT1Gpio(84);
	XintRegs.XINT1CR.bit.POLARITY = HALL_POLARITY;      // 0 - Falling edge interrupt
	XintRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1

	if (HALL_CRANKSHAFT)
	{
		GPIO_SetupXINT2Gpio(86);
		XintRegs.XINT2CR.bit.POLARITY = HALL_POLARITY;      // 1 - Rising edge interrupt
		XintRegs.XINT2CR.bit.ENABLE = 1;        // Enable XINT2
	}
}

/**
 * Таблицы с данными.
 * Потом следует сделать их подгрузку откуда-нибудь ещё. Например, из флеш-памяти.
 */
void EC_Hardware::LoadData(void)
{
	EGD::OUVT->init(0,	0.0002,	15,
					300,50,		15,	0);
	EGD::OUVT->set((double*)&EGD::OUVTdata);

	EGD::SpChar->init(400, 100, 7, 0.0028);
	EGD::SpChar->set((double*)&EGD::SpCharData);

	EGD::NDSteady->init(0, 35, 3, 400);
	EGD::NDSteady->set((double*)&EGD::NDSteadyData);

	EGD::QCStart->init(0, 25, 3, 0.0028);
	EGD::QCStart->set((double*)&EGD::QCStartData);
}

/**
 * Настройка CAN-сообщений
 */
void EC_Hardware::CanSetup(void)
{
	// Initialize the message object that will be used for recieving CAN
	// messages.
	EG::canReceiveMessage.ui32MsgID = 1;		// CAN message ID - use 1
	EG::canReceiveMessage.ui32MsgIDMask = 0;	// no mask needed for TX
	EG::canReceiveMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE;    // enable interrupt on RX
	EG::canReceiveMessage.ui32MsgLen = sizeof(EG::canReceiveData);     // size of message is 8
	EG::canReceiveMessage.pucMsgData = EG::canReceiveData;           // ptr to message content

	canInit(&EG::canReceiveMessage);

}

/**
 * Общая процедура инициализации блока
 */
void EC_Hardware::Initialise(void)
{
	// Initialize System Control: PLL, WatchDog, enable Peripheral Clocks
	InitSysCtrl();

	// - настройка GPIO
	ConfigurePins();
	// - настройка прерываний
	ConfigureInterrupts();
	// - настройка таймера
	ConfigureTimer();
	// - настройка АЦП
	//ConfigureADC();
	// Инициализация драйверов блока
	SetupInterrupts();
	dieselDrvInit();
	// - настройка ШИМ
	CanSetup();
	ConfigureEPWM();
	// - настройка опроса АЦП
	//SetupADC();
	// - настройка ЦАП
	//ConfigureDAC();
	// - загрузка данных из массивов в объекты
	LoadData();
}


