#include "RCSwitch.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <cstring>

namespace {
  const int PIN = 0;
}

/*
  Read in device to determine the correct device id.
  If the supplied parameter does not represent a vaild device,
  then this function returns -1
*/
int getDevice(char* device) {
  if(strlen(device) == 1) {
    switch(device[0]) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5': return atoi(device);
      default:  break;
    }
  }
  return -1;
}

/*
  Determine if the supplied char is a 1 or 0
*/
bool isSwitch(char onoff) {
  return onoff == '1' || onoff == '0';
}

/*
  Check that the supplied switches string is of the specified length
  and only contains 1s or 0s
*/
bool isSwitches(char* switches, int count) {
  int len = strlen(switches);
  if(len == count) {
    for (int i=0; i<len; i++) {
      if(!isSwitch(switches[i])) return false;
    }
    return true;
  }
  return false;
}

/*
  Print out a usage statement
*/
void usage() {
  std::cout << "Usage: send -p=<pulse_length> systemCode deviceId command \n"
               "Examples: \n\n"
               "  send 10010 2 0 # turn off device 2 in group 10010\n"
               "  send 10111 5 1 # turn on device 5 in group 10111\n"
               "\n"
               "Arguments:\n"
               "  -p=<pulse_length>\n"
               "     the length of the pulse in milliseconds\n"
               "  systemCode\n"
               "     the 5 bit code for group of devices. e.g 11011\n"
               "  deviceId\n"
               "     the id of the device as a value between 1-5\n"
               "  command\n"
               "     1 or 0 to power on/off the device respectively\n"
               "\n";
}

using namespace std;
int main(int argc, char *argv[]) {
  if (wiringPiSetup () == -1) return EXIT_FAILURE;
  piHiPri(99);

  RCSwitch rc = RCSwitch();
  rc.enableTransmit(PIN);

  int opt;
  while ((opt = getopt(argc,argv,"p:")) != EOF)
    switch(opt) {
      case 'p': rc.setPulseLength(atoi(optarg)); break;
      case '?': usage(); return EXIT_FAILURE;
      default: abort();
    }

  if(argc - optind == 3) { // There should be 3 arguments left after getopt
    char* systemcode = argv[optind];
    char* deviceStr  = argv[optind+1];
    char* command    = argv[optind+2];
    int device;
    if( isSwitches(systemcode,5) &&
        isSwitches(command,   1) &&
        (device = getDevice(deviceStr)) != -1) {
      switch(atoi(command)) {
        case 0: rc.switchOff(systemcode, device); return EXIT_SUCCESS;
        case 1: rc.switchOn(systemcode, device);  return EXIT_SUCCESS;
        default: break; // Shouldn't get here as command was validated with isSwitches
      }
    }
  }

  usage(); // Failed somewhere above. Print out the usage
  return EXIT_FAILURE;
}
