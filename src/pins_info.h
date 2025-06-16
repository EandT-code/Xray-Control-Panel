
// IO Pin Assign - Digital INPUTS
#define Button_mAs_Pin 2                 // mAs
#define Button_kV_Pin 3               // kV setting
#define Button_LEFT_Pin 4               // LEFT Button  - Disabled
#define Button_RIGHT_Pin 5              // RIGHT Button - Disabled
#define Button_Ready_Pin 1              // Ready Button     //Mentioned as Ready Read in schematic
#define Button_Exposure_Pin 15          // Exposure Button  //Mentioned as EXP_Detect in schematic
#define Button_POWER_Pin 6              // POWER Button  //Shutdown Detect ESP32 in schematic
#define Thermal_Cutoff_Pin 16           // Thermal Cut-off sensor pin

// IO Pin Assign - ADC INPUTS
#define NTC_T1 18                       // Tube Thermistor    //Mentioned as IO_TEMP_1 in schematic
#define NTC_T2 17                       // HV Housing Thermistor    //Mentioned as IO_TEMP_2 in schematic
#define HV_FEEDBACK 7                   // HV Feedback Voltage Sensing Pin
#define BATTERY_VOLTAGE 8               // Battery Voltage sensing Pin   //Mentioned as Bat_Sense in schematic

// IO Pin Assign - Digital OUTPUTS
#define LED 21                          // On-Board LED Pin
#define LASER_EN 33                     // LASER Disable Pin
#define PRECHARGE_EN 42                 // HV Driver Precharge Enable Pin
#define DAC_LS_EN 34                    // DAC Level Shifter Enable Pin
#define LDO_3V3_EN 35                   // 3V3 LDO Enable Pin  //Mentioned as 3V3P EN ESP 32
#define EXPOSURE_EN 36                  // Exposure Enable Pin  //Exposure Voltage EN ESP 32
#define SHUTDOWN_EN 37                  // Shutdown Enable Pin //Device Shutdown ESP 32
#define LDO_5V_EN 38                    // 5V LDO Enable Pin
#define FIL_LS_EN 39                    // Filament Level Shifter Enable Pin
#define HV_LS_EN 40                     // HV Level Shifter Enable Pin

// default pinactive states of OUTPUT Pins - Invert Logic
const boolean GPIO21_AS = LOW;             // On-Board LED LOW                      //Pull Up resistor used, default state high
const boolean GPIO33_AS = HIGH;             // LASER Disable Pin High                 //Laser off when the pin is high
const boolean GPIO42_AS = LOW;             // HV Driver Precharge Enable Pin LOW    //Precharge off when pin is LOW
const boolean GPIO34_AS = LOW;             // DAC Level Shifter Enable Pin LOW      //DAC Level Shifter off when pin is LOW
const boolean GPIO35_AS = LOW;             // 3V3 LDO Enable Pin LOW                //3V3 off when pin is LOW
const boolean GPIO36_AS = LOW;             // Exposure Enable Pin LOW               //Exposure enable off when pin is LOW
const boolean GPIO37_AS = LOW;             // Shutdown Enable Pin LOW               //Shutdown enable off when pin is LOW
const boolean GPIO38_AS = LOW;             // 5V LDO Enable Pin LOW                 //5V is off when pin is LOW
const boolean GPIO39_AS = HIGH;             // Filament Level Shifter Enable Pin LOW //Filament level shifter off when LOW
const boolean GPIO40_AS = HIGH;             // HV Level Shifter Enable Pin LOW       //HV Level shifter off when LOW

//  Device MODEs flag default status
//volatile boolean STARTUP = false;
volatile boolean STANDBY = true;
volatile boolean READY = false;
volatile boolean EXPOSURE = false;
volatile boolean COOLDOWN = false;
//volatile boolean SHUTDOWN = false;
//volatile boolean isSleepPressed = false;


// Parameter Default values
// volatile long cooldownTimeInMs = 3000;
volatile long cooldownTime = 2; //cooldown time of the system
static uint16_t xrt_duty = 30;             //duty cycle of xray tube
volatile long timec = 0;
const int delayTime = 0;        //Exposure delay time after pressing exposure for filament preheating

static float ref_voltage=5;   //Reference DAC voltage

// Other System Variables
float risetime = 50e-3;      //rise time in milliseconds
//volatile float kVValueIncrement = 2; //for use when duty cycle is being changed 
//volatile int timeValueIncrement = 5.1;
const float mAs_max = 6;          //max mAs of the ssytem
const float mAs_min = 1;          //min mAs of the system
const float mA = 3;              //fixed mA of the system
const float mAs_increment = 1;  //mAs increment step = 1

// const float kv_max = 70;        //max kV = 70
// const float kv_min = 5;        //min kV = 50
// const float kv_increment = 5; //kV step

const float kv_max = 50;        //max kV = 70
const float kv_min = 0;        //min kV = 50
const float kv_increment = 1;   //kV step

volatile uint16_t kv = kv_min;		//default kV value = min kV
volatile float mAs = mAs_min;	//default mAs value = min mAs
volatile float et = mAs/mA;		//default et value = min et
volatile float hv_dc = 0;       //default hv dc = 0

volatile float mcp_set_voltage = 0;         //set volatge in MCP

volatile long timerInMs = 2000;

volatile long lastReadyUpdate = 0;
int readySeconds = 0;
unsigned long readyStartTime = 0;  // Store when READY mode started
unsigned long systemtime = 0;

volatile long cooldownEndTime = 0;  //Initializing cooldown time to be zero for testing purposes
volatile long cooldownTimer = 0;    //Initializing cooldown start time
volatile long cooldownStartTime = 0;
volatile float dac_volt_target = 0;
volatile double fil_off_delay = 0;
/*
mA lookup table (filament duty cycle = )
kV      battery_voltage     mA  
*/


