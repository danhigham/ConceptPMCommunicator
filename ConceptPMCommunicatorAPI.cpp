/**********************************************************\

  Auto-generated ConceptPMCommunicatorAPI.cpp

\**********************************************************/



#include "JSObject.h"
#include "variant_list.h"
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

using namespace std;

#include "DOM/Document.h"

#include "ConceptPMCommunicatorAPI.h"

/* Include PM3 stuff */

#include "PM3DDICP.h"
#include "PM3CSafeCP.h"
#include "PM3USBCP.h"
#include "csafe.h"

#define PM_SERIALNUMSTRING_LEN            16
#define PM_FWIDSTRING_LEN                 20

#define TWO_K_WORKOUT                     0x01
#define FIVE_K_WORKOUT                     0x02
#define TEN_K_WORKOUT                     0x03

///////////////////////////////////////////////////////////////////////////////
/// @fn ConceptPMCommunicatorAPI::ConceptPMCommunicatorAPI(const ConceptPMCommunicatorPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
ConceptPMCommunicatorAPI::ConceptPMCommunicatorAPI(const ConceptPMCommunicatorPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{  
  /* Concept device methods */
  registerMethod("getCSafeDllVersion", make_method(this, &ConceptPMCommunicatorAPI::getCSafeDllVersion));
  registerMethod("initDevices", make_method(this, &ConceptPMCommunicatorAPI::initDevices));  
  registerMethod("getSerialNumber", make_method(this, &ConceptPMCommunicatorAPI::getSerialNumber));  
  registerMethod("getFirmwareRevision", make_method(this, &ConceptPMCommunicatorAPI::getFirmwareRevision));  
  registerMethod("getStatus", make_method(this, &ConceptPMCommunicatorAPI::getTransactionStatus));  
  registerMethod("getWorkoutUpdate", make_method(this, &ConceptPMCommunicatorAPI::getWorkoutUpdate));
  registerMethod("getHeartRate", make_method(this, &ConceptPMCommunicatorAPI::getHeartRate));
  
  registerMethod("startWorkout", make_method(this, &ConceptPMCommunicatorAPI::startWorkout));
  registerMethod("startTwoKWorkout", make_method(this, &ConceptPMCommunicatorAPI::startTwoKWorkout));
  registerMethod("startFiveKWorkout", make_method(this, &ConceptPMCommunicatorAPI::startFiveKWorkout));
  registerMethod("startTenKWorkout", make_method(this, &ConceptPMCommunicatorAPI::startTenKWorkout));
  registerMethod("startFixedDistanceWorkout", make_method(this, &ConceptPMCommunicatorAPI::startFixedDistanceWorkout));
  registerMethod("startFixedTimeWorkout", make_method(this, &ConceptPMCommunicatorAPI::startFixedTimeWorkout));
  
  // Read-write property
  registerProperty("mockData",
                 make_property(this,
                               &ConceptPMCommunicatorAPI::get_mockData,
                               &ConceptPMCommunicatorAPI::set_mockData));

  // Read-only property
  registerProperty("version",
                   make_property(this,
                      &ConceptPMCommunicatorAPI::get_version));
  
  // Set mockData to false
  set_mockData(false);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn ConceptPMCommunicatorAPI::~ConceptPMCommunicatorAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
ConceptPMCommunicatorAPI::~ConceptPMCommunicatorAPI()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @fn ConceptPMCommunicatorPtr ConceptPMCommunicatorAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
ConceptPMCommunicatorPtr ConceptPMCommunicatorAPI::getPlugin()
{
    ConceptPMCommunicatorPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

// Read/Write property testString
bool ConceptPMCommunicatorAPI::get_mockData()
{
    return m_mockData;
}
void ConceptPMCommunicatorAPI::set_mockData(const bool& val)
{
    m_mockData = val;
    m_distanceMock = false;
}


// Read-only property version
std::string ConceptPMCommunicatorAPI::get_version()
{
    return "CURRENT_VERSION";
}

long ConceptPMCommunicatorAPI::getCSafeDllVersion ()
{
  return tkcmdsetCSAFE_get_dll_version();
}

std::string ConceptPMCommunicatorAPI::getSerialNumber(int port) 
{ 
  if (m_mockData) return generateString(PM_SERIALNUMSTRING_LEN);
  
  char rspbuf[250];
  int ecode = tkcmdsetDDI_serial_number(port, rspbuf, PM_SERIALNUMSTRING_LEN);
  std::string result;
  stringstream s;
  
  if (!ecode) {
    s << rspbuf;  
  } else {
    s << "Couldn't get serial number ";
    logError(ecode);
  }
  
  result = s.str();  
  return result;
}

std::string ConceptPMCommunicatorAPI::getFirmwareRevision(int port) 
{ 
  if (m_mockData) return generateString(PM_FWIDSTRING_LEN);
  
  char rspbuf[250];
  int ecode = tkcmdsetDDI_fw_version(port, rspbuf, PM_FWIDSTRING_LEN);
  std::string result;
  stringstream s;
  
  if (!ecode) {
    s << rspbuf;  
  } else { 
    s << "Couldn't get firmware revision ";
    logError(ecode);
  }

  result = s.str();  
  return result;
}

void ConceptPMCommunicatorAPI::startTwoKWorkout(int port) { startWorkout(port, TWO_K_WORKOUT); }
void ConceptPMCommunicatorAPI::startFiveKWorkout(int port) { startWorkout(port, FIVE_K_WORKOUT); }
void ConceptPMCommunicatorAPI::startTenKWorkout(int port) { startWorkout(port, TEN_K_WORKOUT); }

void ConceptPMCommunicatorAPI::startFixedDistanceWorkout(int port, int distance) 
{
    if (m_mockData){ //If mocking reset all vars
        
        m_distanceMock = false;
        
        m_Distance = distance;
        m_Time = 0;
        m_DragFactor = 0;
        m_Pace = 0;
        m_Power = 0;
        m_Cadence = 0;
        
        return;
    }
    
    //Calculate distance bits
    unsigned char distanceByte0 = (distance & 0xFF); // LSB
    unsigned char distanceByte1 = (distance & 0xFF00) >> 8; //MSB
    
    //Calculate split bits
    
    int splitMeters = distance;
    
    unsigned char splitByte0 = (splitMeters & 0xFF);
    unsigned char splitByte1 = (splitMeters & 0xFF00) >> 8;
    unsigned char splitByte2 = (splitMeters & 0xFF0000) >> 16;
    unsigned char splitByte3 = (splitMeters & 0xFF000000) >> 24;
        
    int commands[21] = {
        CSAFE_GOFINISHED_CMD, //0x86
        CSAFE_RESET_CMD, //0x81
        CSAFE_SETHORIZONTAL_CMD, //0x21
        0x03, 
        distanceByte0,
        distanceByte1,
        0x20, // set distance in km
        CSAFE_SETUSERCFG1_CMD, //0x1A
        0x07, 
        CSAFE_PM_SET_SPLITDURATION,
        0x05,
        0x80, //Distance based splits
        splitByte0, //LSB
        splitByte1, 
        splitByte2, 
        splitByte3, //MSB       
        CSAFE_SETPROGRAM_CMD,
        0x02,
        0x00,    
        0x00,    
        CSAFE_GOINUSE_CMD
    };
    
    int commandSize = sizeof(commands) / sizeof(int);
    int *result = runCSafeCommand(port, commands, commandSize, false); 
}


void ConceptPMCommunicatorAPI::startWorkout(int port, int workOut) 
{
  if (m_mockData){ //If mocking reset all vars
    
    if (workOut == TWO_K_WORKOUT) m_Distance = 2000;
    if (workOut == FIVE_K_WORKOUT) m_Distance = 5000;
    if (workOut == TEN_K_WORKOUT) m_Distance = 10000;
    
    m_distanceMock = true;
    
    m_Time = 0;
    m_DragFactor = 0;
    m_Pace = 0;
    m_Power = 0;
    m_Cadence = 0;
    
    return;
  }
  
  int commands[6] = {
    CSAFE_GOFINISHED_CMD,
    CSAFE_RESET_CMD,
    CSAFE_SETPROGRAM_CMD,
    0x01,
    workOut,
    CSAFE_GOINUSE_CMD
  };
  
  int commandSize = sizeof(commands) / sizeof(int);
  int *result = runCSafeCommand(port, commands, commandSize, false); 
  
}

void ConceptPMCommunicatorAPI::startFixedTimeWorkout(int port, int hours, int minutes, int seconds)
{
  if (m_mockData){ //If mocking reset all vars
    
    m_distanceMock = false;
    
    m_Distance = 0;
    m_Time = (minutes * 60) + seconds;
    m_DragFactor = 0;
    m_Pace = 0;
    m_Power = 0;
    m_Cadence = 0;
    
    return;
  }
  
  int totalSeconds = (minutes * 60) + seconds;
  
  //Add in 10 splits per workout
  
  int splitInterval = totalSeconds * 100;

  unsigned char inByte0 = (splitInterval & 0xFF);
  unsigned char inByte1 = (splitInterval & 0xFF00) >> 8;
  unsigned char inByte2 = (splitInterval & 0xFF0000) >> 16;
  unsigned char inByte3 = (splitInterval & 0xFF000000) >> 24;
  
  int commands[21] = {
    CSAFE_GOFINISHED_CMD, //0x86
    CSAFE_RESET_CMD, //0x81
    CSAFE_SETTWORK_CMD, //0x20
    0x03, 
    hours,
    minutes,
    seconds,
    CSAFE_SETUSERCFG1_CMD, //0x1A
    0x07, 
    CSAFE_PM_SET_SPLITDURATION,
    0x05,
    0x00, //Time based splits
    inByte0, //LSB
    inByte1, 
    inByte2, 
    inByte3, //MSB       
    CSAFE_SETPROGRAM_CMD,
    0x02,
    0x00,    
    0x00,    
    CSAFE_GOINUSE_CMD
  };
  
  int commandSize = sizeof(commands) / sizeof(int);
  int *result = runCSafeCommand(port, commands, commandSize, false); 
}

/*
 
 Custom workout code :-
 
 addStageTime
 addStageDistance
 addRest
 
*/


int ConceptPMCommunicatorAPI::initDevices(void)
{ 
  int ecode = 0;
    
  FB::VariantList result;
  
  string scode = boost::lexical_cast<std::string>(CSAFE_RESET_CMD);
  logString(scode);
  
  ecode = tkcmdsetDDI_init();
  if (ecode) logError(ecode);

  ecode = tkcmdsetCSAFE_init_protocol(1000);
  if (ecode) logError(ecode);
  
  int deviceCount = discoverDevices();
    
  return deviceCount;
}


std::string ConceptPMCommunicatorAPI::getTransactionStatus(int port)
{
  int result = tkcmdsetCSAFE_get_status();
  
  if (result == STATUS_OK) return "OK";
  if (result == STATUS_PREV_REJECT) return "PREV_REJECT";
  if (result == STATUS_PREV_BAD) return "PREV_BAD";
  if (result == STATUS_PREV_NOT_READY) return "PREV_NOT_READY";

  stringstream s;
  s << result;
  return s.str();
}

std::string ConceptPMCommunicatorAPI::getHeartRate(int port)
{
  if (m_mockData) return generateHeartRate();
  
  int heartRate = 0;
  int commands[1] = {
    // Get heart rate
    CSAFE_GETHRCUR_CMD
  };
  
  
  int commandSize = sizeof(commands) / sizeof(int);
  int *result = runCSafeCommand(port, commands, commandSize); 
  
  int size = result[0];
  
  int datalength = 0;
  
  for (int x=1; x <= size; x++) { 
       
    //Heart rate
    if (result[x] == CSAFE_GETHRCUR_CMD) {
      x++;
      datalength = result[x];
      x++;
      
      if (result[x] > 0) heartRate = result[x];
      
      x+=datalength;
    }
  }
  
  stringstream s;
  
  s << "{";
  s << "\"heart_rate\":";
  s << heartRate;
  s << "}";
  
  return s.str();
  
}

std::string ConceptPMCommunicatorAPI::getWorkoutUpdate(int port)
{
  if (m_mockData) return generateWorkoutData();
  
  int dragFactor;
  int workDistance = 0;
  float workTime;
  int pace;
  float power;
  int cadence_spm;
  
  int commands[8] = {
    // Command wrapper ?
    CSAFE_SETUSERCFG1_CMD,
    0x03,
    
    // PM3 Commands.
    CSAFE_PM_GET_DRAGFACTOR,
    CSAFE_PM_GET_WORKDISTANCE,
    CSAFE_PM_GET_WORKTIME,
    
    // Standard commands.
    CSAFE_GETPACE_CMD,
    CSAFE_GETPOWER_CMD,
    CSAFE_GETCADENCE_CMD
  };
  
  int commandSize = sizeof(commands) / sizeof(int);
  int *result = runCSafeCommand(port, commands, commandSize); 
  
  int size = result[0];
  
  int datalength = 0;
  
  for (int x=1; x <= size; x++) { 
    
    if (result[x] == CSAFE_SETUSERCFG1_CMD) {
      x+=2;
    }
    
    //Drag factor
    if (result[x] == CSAFE_PM_GET_DRAGFACTOR) {
      x++;
      datalength = result[x];
      x++;
      
      dragFactor = result[x];
      x+=datalength;
    }
    
    //Work distance
    if (result[x] == CSAFE_PM_GET_WORKDISTANCE) {
      x++;
      datalength = result[x];
      x++;

      workDistance = (result[x] + (result[x + 1] << 8) + (result[x + 2] << 16) + (result[x + 3] << 24)) / 10;
      
      x+=datalength;
    }

    //Work time
    if (result[x] == CSAFE_PM_GET_WORKTIME) {
      x++;
      datalength = result[x];
      x++;
      
      workTime = (result[x] + (result[x + 1] << 8) + (result[x + 2] << 16) + (result[x + 3] << 24)) / 100;
      float fraction = (result[x + 4] / 100.0);
      
      workTime += fraction;
      
      x+=datalength;
    }

    //Pace
    if (result[x] == CSAFE_GETPACE_CMD) {
      x++;
      datalength = result[x];
      x++;
      
      // Pace is in seconds/Km
      pace = result[x] + (result[x + 1] << 8);

      // get pace in seconds / 500m
    
      pace = pace / 2.0;
      
      // convert it to mins/500m
//      double minutes = floor(fPace / 60);
//      double seconds = fPace - (minutes * 60);
      
      x+=datalength;
    }

    //Power
    if (result[x] == CSAFE_GETPOWER_CMD) {
      x++;
      datalength = result[x];
      x++;
      
      power = result[x] + (result[x + 1] << 8);
                                           
      x+=datalength;
    }

    //Cadence
    if (result[x] == CSAFE_GETCADENCE_CMD) {
      x++;
      datalength = result[x];
      x++;
      
      cadence_spm = result[x];
      
      x+=datalength;
    }
    
  }
  stringstream s;
  
  s << "{";
  s << "\"drag_factor\":";
  s << dragFactor;
  
  s << ",\"work_distance\":";
  s << workDistance;
  
  s << ",\"work_time\":";
  s << workTime;
  
  s << ",\"pace\":";
  s << pace;
  
  s << ",\"power\":";
  s << power;
  
  s << ",\"cadence_spm\":";
  s << cadence_spm;
  s << "}";
  
  
  return s.str();
}

/*
 ###########################
 #
 # Private methods
 #
 ###########################
 */

int *ConceptPMCommunicatorAPI::runCSafeCommand(int port, int commands[64], int commandCount) 
{
  return runCSafeCommand(port, commands, commandCount, true);
}

int *ConceptPMCommunicatorAPI::runCSafeCommand(int port, int commands[64], int commandCount, bool readOutput)
{
  UINT16_T cmdDataSize = (UINT16_T)commandCount;
  UINT32_T rspData[64], cmdData[64];
  UINT16_T rspSize = 100;
  
  stringstream s;
  s << "Running CSAFE command\n";
  s << "Command size is ";
  s << commandCount;
  s << "\nCommands are;";
  
  logString(s.str());
  logCommandOutput(commands, commandCount);
  
  int result[1024];
  
  for (int i = 0; i < commandCount; i++) 
    cmdData[i] = (UINT32_T) commands[i];
  
  int ecode = tkcmdsetCSAFE_command(port, cmdDataSize, cmdData, &rspSize, rspData);
  
  if (!ecode){
    
    if (!readOutput) 
    {
      result[0] = 0;
      return result;
    }
    
    result[0] = (int)rspSize;
  
    for (int i = 0; i < rspSize; i++) 
      result[i + 1] = (int) rspData[i];
 
    stringstream s;
    
    s << "Command Ouput;";
    logString(s.str());
    
    logCommandOutput(result, rspSize + 1);
    
  } else {
    
    stringstream s;
    s << "Error retrieving response ";
    logString(s.str());
    logError(ecode);
  }

  return result;
}

int ConceptPMCommunicatorAPI::discoverDevices(void)
{
  int numOldPM3Devices = 0;
  int numNewPM3Devices = 0;
  int numPM4Devices = 0;
  int ecode = 0;
  UINT16_T numCommunicating = 0;
  
  // Look for PM3 devices
  ecode = tkcmdsetDDI_discover_pm3s(TKCMDSET_PM3_PRODUCT_NAME2, 0, &numCommunicating);
  
  if (!ecode && numCommunicating)
  {
    // We discovered one or more PM3's
    numNewPM3Devices = numCommunicating;
  }
  
  // Look for old style PM3 devices, starting numbering after the previous
  ecode = tkcmdsetDDI_discover_pm3s(TKCMDSET_PM3_PRODUCT_NAME, numCommunicating, &numCommunicating);
  
  if (!ecode && numCommunicating)
  {
    // We discovered one or more old PM3's
    numOldPM3Devices = numCommunicating - numNewPM3Devices;
  }
  
  // Look for PM4 devices
  ecode = tkcmdsetDDI_discover_pm3s(TKCMDSET_PM4_PRODUCT_NAME, numCommunicating, &numCommunicating);
  if (!ecode && numCommunicating)
  {
    // We discovered one or more PM4's
    numPM4Devices = numCommunicating - numNewPM3Devices - numOldPM3Devices;
  }
  
  return numCommunicating;
}


void ConceptPMCommunicatorAPI::logString(string message) 
{
  stringstream s;
  s << "ConceptPMCommunicatorAPI : ";
  s << message;
  
  m_host->htmlLog(s.str());
}

void ConceptPMCommunicatorAPI::logCommandOutput(int *list, int count)
{
  stringstream s;
  
  for(int x=0; x<count; x++){
    if (x>0) s << ", ";
    s << list[x];
  }
  m_host->htmlLog(s.str());
}

void ConceptPMCommunicatorAPI::logError(int ecode) 
{
  char ename[40];
  char etext[200];
  
  tkcmdsetDDI_get_error_name(ecode,ename,sizeof(ename));
  tkcmdsetDDI_get_error_text(ecode,etext,sizeof(etext));
  stringstream s;
  s << ename;
  s << " - ";
  s << etext;

  logString(s.str());
}

FB::VariantList ConceptPMCommunicatorAPI::asList(int *values, int length)
{
  FB::VariantList output;
  
  for(int x=0; x<length; x++)
  {
    output.push_back(values[x]);
  }
  
  return output;
}

FB::VariantList ConceptPMCommunicatorAPI::charsAsList(char *values, int length)
{
  FB::VariantList output;
  
  for(int x=0; x<length; x++)
  {
    std::string s = boost::lexical_cast<std::string>(values[x]);
    output.push_back(s);
  }
  
  return output;
}

/*
 ###########################
 #
 # Mock Data Functions
 #
 ###########################
 */

string ConceptPMCommunicatorAPI::generateString(int length) 
{
  string sample = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  stringstream s;
  
  for(int x=0; x<length; x++){
    s << sample[x]; 
  }
  
  return s.str();
}

string ConceptPMCommunicatorAPI::generateStatus(void)
{
  int status = 1;
  stringstream s;
  
  
  s << "{";
  s << "\"current_status\":";
  s << status;
  s << "}";
  
  return s.str();
}

string ConceptPMCommunicatorAPI::generateHeartRate(void)
{
  /* random seed */
  srand( time(NULL) );
  
  int heartRate = ( rand() % 40 + 110 );
 
  stringstream s;
  
  s << "{";
  s << "\"heart_rate\":";
  s << heartRate;
  s << "}";
  
  return s.str();
}

string ConceptPMCommunicatorAPI::generateWorkoutData(void)
{
  /* random seed */
  srand( time(NULL) );
  
  if (m_distanceMock) {
    m_Distance -= 1;
    m_Time += 1;
  } else {
    m_Distance += 1;
    m_Time -= 1;      
  }
  
  m_DragFactor = ( rand() % 40 + 110 );
  m_Pace = ( rand() % 100 + 250 );
  m_Power = ( rand() % 100 + 300 );
  m_Cadence = ( rand() % 40 + 20 );
  
  
  stringstream s;
  
  s << "{";
  s << "\"drag_factor\":";
  s << m_DragFactor;
  
  s << ",\"work_distance\":";
  s << m_Distance;

  s << ",\"work_time\":";
  s << m_Time;

  s << ",\"pace\":";
  s << m_Pace;

  s << ",\"power\":";
  s << m_Power;

  s << ",\"cadence_spm\":";
  s << m_Cadence;
  s << "}";
  
  return s.str();
}
 

