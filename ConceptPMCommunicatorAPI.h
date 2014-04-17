/**********************************************************\

  Auto-generated ConceptPMCommunicatorAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "ConceptPMCommunicator.h"

#include "PM3DDICP.h"
#include "PM3CSafeCP.h"
#include "PM3USBCP.h"
#include "csafe.h"

#ifndef H_ConceptPMCommunicatorAPI
#define H_ConceptPMCommunicatorAPI

class ConceptPMCommunicatorAPI : public FB::JSAPIAuto
{
public:
  ConceptPMCommunicatorAPI(const ConceptPMCommunicatorPtr& plugin, const FB::BrowserHostPtr& host);
  virtual ~ConceptPMCommunicatorAPI();

  ConceptPMCommunicatorPtr getPlugin();

  // Read/Write property ${PROPERTY.ident}
  std::string get_testString();
  void set_testString(const std::string& val);

  // Read/Write property ${PROPERTY.ident}
  bool get_mockData();
  void set_mockData(const bool& val);
  
  
  // Read-only property ${PROPERTY.ident}
  std::string get_version();

  // Method echo
  FB::variant echo(const FB::variant& msg);
  
  // Event helpers
  FB_JSAPI_EVENT(fired, 3, (const FB::variant&, bool, int));
  FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));
  FB_JSAPI_EVENT(notify, 0, ());

  // Method test-event
  void testEvent(const FB::variant& s);
  
  // Get version of CSafeLib
  long getCSafeDllVersion(void);
  
  int initDevices(void);
  std::string getSerialNumber(int port);
  std::string getFirmwareRevision(int port);

  void displayMessage(int port, std::string message);

  std::string getTransactionStatus(int port);
  std::string getWorkoutUpdate(int port);
  std::string getHeartRate(int port);

  void startTwoKWorkout(int port);
  void startFiveKWorkout(int port);
  void startTenKWorkout(int port);
    
  void startFixedDistanceWorkout(int port, int distance);
  void startFixedTimeWorkout(int port, int hours, int minutes, int seconds);
  
  void startWorkout(int port, int workout);
  
private:
  
  int *runCSafeCommand(int port, int commands[64], int commandCount);
  int *runCSafeCommand(int port, int commands[64], int commandCount, bool readOutput);
  
  FB::VariantList asList(int *values, int length);
  FB::VariantList charsAsList(char *values, int length);
  void logCommandOutput(int *list, int count);
  void logString(std::string message);
  void logError(int errorcode);
  int discoverDevices(void);
  
  /*
  ###########################
  #
  # Properties
  #
  ###########################
  */
  
  std::string m_testString;
  bool m_mockData;
  bool m_distanceMock;
  
  FB::BrowserHostPtr m_host;
  ConceptPMCommunicatorWeakPtr m_plugin;

  /* Member vars for mocking real data */
  
  float m_Distance;
  float m_Time;
  float m_DragFactor;
  float m_Pace;
  float m_Power;
  float m_Cadence;
    
  /*
  ###########################
  #
  # Mock Data Functions
  #
  ###########################
  */
  
  std::string generateString(int length);
  std::string generateWorkoutData();
  std::string generateStatus();
  std::string generateHeartRate();
  
  
};

#endif // H_ConceptPMCommunicatorAPI

