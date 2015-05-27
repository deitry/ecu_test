#include "ecu_global_defines.h"
#include "drv\diesel_drv.h"

float EG::injPhi[DIESEL_N_CYL] = {720};
int EG::injZ[DIESEL_N_CYL] = {0};
int EG::dTime[DIESEL_N_CYL] = {0};
Uint32 EG::injCnt[DIESEL_N_CYL] = {0};
Uint8 EG::injSw[DIESEL_N_CYL] = {0};
Uint32 EG::injN[DIESEL_N_CYL] = {0};

float EG::err;
float EG::errI = 0;
float EG::errD = 0;

float EG::kP = 1e-5;
float EG::kI = 2e-6;
float EG::kD = 2e-6;

float EG::nR = 400/HMLTP;
float EG::nU = 400/HMLTP;
float EG::omegaR = 0;

float EG::pInj = 0;

float EG::QC = 1e-4;
float EG::QCmin = 1e-5;
float EG::QCmax = 0.0028;
float EG::QCadop = 0.0028;
float EG::alphaDop = 1.3;
float EG::kQc = 2.3622;

volatile Uint8 EG::g_duty1 = 50;
volatile Uint8 EG::g_duty2 = 10;
volatile Uint16 EG::g_step1Us = 1300;
volatile Uint16 EG::g_step2Us = 5000;
volatile Uint16 EG::g_step3Us = 100;

#ifndef ZAVOD
int EG::manQC = 0;
int EG::manInj = 0;
int EG::manPed = 0;
int EG::manQCt = 0;
#else
int EG::manQC = 1;
int EG::manInj = 1;
int EG::manPed = 1;
int EG::manQCt = 1;
#endif

int EG::manQCRelay = 0;
int EG::manPhi = 0;
int EG::manOUVT = 1;
int EG::manMode = 1;

int EG::pedStep = 5;
float EG::pedal = 400/HMLTP;
int EG::injOnce = 0;
int EG::injCyl = 0x0000;
int EG::manCnt = 0;
int EG::manN = 0;
int EG::manAngle = 0;
float EG::injAngle = 3.5;
float EG::injOuvt = 0;

int EG::finTime = 1000000;

float32 EG::val = 0;
Uint16 EG::sens = TEMPERATURE_SENSOR;
Uint8 EG::chan = TEMP_SENS_CHANNEL_2;

int EG::inOut = 2;

int EG::canSend = 1;
int EG::canTime = 1;
int EG::manFdbk = 0;
int EG::manDur = 1;
int EG::manLed = 0;
int EG::manSens = 0;

float EG::delta_time = 0.0;
Uint32 EG::ttim = 0;

float EG::int_time = 0.0;
float EG::int_time1 = 0.0;
float EG::prog_time = 0.0;

int EG::cntCanTransmit = 1;
int EG::cntCanTransmit2 = 0;
PAR_ID_BYTES EG::curCanId = {1,0};

float32 EG::valX[SENSCNT] = {1000,1000,1000,1000,1000,1000,1000,1000};
Uint16 EG::sensX[SENSCNT] = {1,1,1,1,1,1,1,1};
Uint8 EG::chanX[SENSCNT] = {0,0,0,0,0,0,0,0};

float32 EG::valP = 0;
Uint16 EG::sensP = CURRENT_SENSOR;
Uint8 EG::chanP = CURR_SENS_CHANNEL_8;
float EG::Pk_koeff = 1640;
float32 EG::valT = 0;
Uint16 EG::sensT = CURRENT_SENSOR;
Uint8 EG::chanT = CURR_SENS_CHANNEL_7;
float EG::Tv_koeff = 1464;

float EG::dZone = 0.5;

float EG::Pk = 0;
float EG::Tv = 0;
float EG::anVMT = 0;

float EG::dOmega;
//float EG::errDMax = 1;
float EG::errRelayMax = 3/HMLTP;
float EG::QCprev = 0;
float EG::muN = 0.00005;//0.000005;
float EG::nU0 = 400;

int EG::progCnt = 0;
int EG::fdbkTCnt = 0;
int EG::getFdbk = 0;

Uint16 EG::n1 = 0;
Uint16 EG::n2 = 0;
Uint16 EG::n3 = 0;
Uint16 EG::d1 = 0;
Uint16 EG::d2 = 0;
Uint16 EG::d3 = 0;

Uint8 EG::zCnt = 0;
Uint16 EG::nCnt = 0;
Uint32 EG::timeCnt = 0;
int EG::canLock = 0;
int EG::canLockM = 2;
int EG::fdbkLock = 0;
int EG::fdbkAll = 0;
Uint8 EG::fdbkCyl = 0;

int EG::tmpi = 0;
PAR_ID_BYTES EG::tparid = {0,0};

Uint8 EG::canReceiveData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
tCANMsgObject EG::canReceiveMessage;
CAN_DATA EG::canTransmitMessage;

PAR_ID_BYTES EG::canTransmitId[PARIDMAX] =
{
	{0x01,0x00},
	{0x02,0x00},
	{0x02,0x01},
	{0x02,0x03},
	{0x03,0x02},
	{0x03,0x01},
	{0x03,0x03},
	{0x03,0x04},
	{0x06,0x00},
	{0x10,0x00},
	{0x10,0x01},
	{0x10,0x02},
	{0x10,0x03},
	{0x11,0x00},
	{0x12,0x00},
	{0x13,0x00},
	{0x14,0x00},
	{0x15,0x00},
	{0x21,0x00},
	{0x22,0x00},
	{0x22,0x01},
	{0x22,0x03},
	{0x23,0x00},
	{0x23,0x01},
	{0x23,0x02},
	{0x24,0x00},
	{0x25,0x00},
	{0x25,0x01},
	{0x26,0x00},
	{0x26,0x01},
	{0x27,0x00},
	{0x28,0x00},
	{0x41,0x00},
	{0x42,0x00},
	{0x43,0x00},
	{0x44,0x00},
	{0x45,0x00},
	{0x46,0x00},
	{0x51,0x00},
	{0x52,0x00},
	{0x53,0x00},
	{0x53,0x00}		// [PARIDMAX-1] будет изменяемым
};

//#pragma DATA_SECTION("eg_data")
float32 EG::fdbkBuf[FDBK_BUF] = {0};
int EG::fdbkTBuf[FDBK_BUF] = {0};

CAN_DATA EG::can_data =
{
		0
};

#pragma DATA_SECTION("eg_data")
const double EGD::OUVTdata[15][15] =
{
		{-5, -5.5, -6, -6.5, -7, -7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12},
		{-5.5, -6, -6.5, -7, -7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5},
		{-6, -6.5, -7, -7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13},
		{-6.5, -7, -7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5},
		{-7, -7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, -14},
		{-7.5, -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5},
		{-8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15},
		{-8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5},
		{-9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16},
		{-9.5, -10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5},
		{-10, -10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5, -17},
		{-10.5, -11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5, -17, -17.5},
		{-11, -11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5, -17, -17.5, -18},
		{-11.5, -12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5, -17, -17.5, -18, -19},
		{-12, -12.5, -13, -13.5, 14, -14.5, 15, -15.5, -16, -16.5, -17, -17.5, -18, -18.5, -20}
};

#pragma DATA_SECTION("eg_data")
const double EGD::SpCharData[7] =
{
		0.0018, 0.00205, 0.0023, 0.00255, 0.0028, 0.00265, 0.00245
};

const double EGD::NDSteadyData[3] =
{
		450, 425, 400
};

const double EGD::QCStartData[3] =
{
		2.8, 2.3, 1.8
};

FMField2* EGD::OUVT = new FMField2;
FMField1* EGD::SpChar = new FMField1;
FMField1* EGD::NDSteady = new FMField1;
FMField1* EGD::QCStart = new FMField1;

PAR_ID_BYTES EG::tPid = {
		EC_TIME, 0
};
