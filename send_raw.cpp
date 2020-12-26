#include <unistd.h>
#include <iostream>
#include <map>
#include <functional>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <cstring>
#include <wiringPi.h>

#define PIN 0

typedef struct config {
    int short_pulse;
    int long_pulse;
    int extended_pulse;
    int bit_length;
    int repeat;
} config;

const uint8_t DEFAULT_REPEAT = 20;
std::map<std::string, config> NAMED_CONFIG = {
    {"MASTERPLUG", {320, 848, 848, 34, DEFAULT_REPEAT}},
    {"DEWENWILS", {180, 520, 5300, 25, DEFAULT_REPEAT}}
};

/*
  Print out a usage statement
*/
void usage() {
  std::cout << "Usage: send_raw -c <preloaded_config> -s <short_pulse> -l <long_pulse> -e <extended_pulse> -b <bit_length> -r <repeats> data \n"
               "Examples: \n\n"
               "  send_raw -c MASTERPLUG 0x1850BC26F                # Switch on Masterplug using hex\n"
               "  send_raw -c DEWENWILS 0b1111101110101010110011001 # Switch on DEWENWILS using binary string\n"
               "  semd_raw -s 100 -l 200 -e 300 -r 20 0b01010101010 # Transmit a bit string with custom delays\n"
               "\n"
               "Arguments:\n"
               "  -c <preloaded_config>\n"
               "     Use the pulse predefined pulse configuration. e.g MASTERPLUG or DEWENWILS \n"
               "  -s <short_pulse>\n"
               "     The short pulse time in microseconds\n"
               "  -l <long_pulse>\n"
               "     The long pulse time in microseconds\n"
               "  -e <extended_pulse>\n"
               "     The time to wait between repeat pulses transmisions in microseconds\n"
               "  -b <bit_length>\n"
               "     The length of the bitstring to transmit when using a Hex data string. Ignored when a binary string is supplied\n"
               "  -r <repeat>\n"
               "     How many times to repeat the transmission\n"
               "\n";
}

bool isValidConfig(config config) {
    return
        config.short_pulse > 0 &&
        config.long_pulse > 0 &&
        config.extended_pulse > 0 &&
        config.repeat > 0;
}

void transmit(config config, std::string bits) {
    for(int t=0; t<config.repeat; t++) {
        for (char &i : bits) {
            if(i == '1') {
                digitalWrite(PIN, 1);
                usleep(config.short_pulse);
                digitalWrite(PIN, 0);
                usleep(config.long_pulse);
            } else if(i == '0') {
                digitalWrite(PIN, 1);
                usleep(config.long_pulse);
                digitalWrite(PIN, 0);
                usleep(config.short_pulse);
            }
        }
        digitalWrite(PIN, 0);
        usleep(config.extended_pulse);
    }  
}

bool isbdigit(char onoff) {
    return onoff == '1' || onoff == '0';
}

bool isValidString(char* data, std::function<bool(char)> function) {
    for(int i=0; i<strlen(data); i++) {
        if(!function(data[i])) return false;
    }
    return true;
}

std::string hexToBinary(char xdigit) {
    switch(tolower(xdigit)) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'a': return "1010";
        case 'b': return "1011";
        case 'c': return "1100";
        case 'd': return "1101";
        case 'e': return "1110";
        case 'f': return "1111";
    }
    return {};
}

/*
  Parse the raw data and convert in to a string of bits.
*/
std::string parseBitString(char* data, config* config) {
    if (data[0] == 'b' && isValidString(&data[1], &isbdigit)) {
        config->bit_length = strlen(&data[1]);
        return &data[1];
    }
    if (data[0] == 'x' && isValidString(&data[1], &isxdigit)) {
        std::string toReturn;
        for(int i=1; i<strlen(data); i++) 
            toReturn.append(hexToBinary(data[i]));
        return toReturn;
    }
    return {};
}

int main(int argc, char *argv[]) {

    if (wiringPiSetup () == -1) return EXIT_FAILURE;
    piHiPri(99);
    pinMode(PIN, OUTPUT);

    config arguments = {0, 0, 0, 0, DEFAULT_REPEAT};
    config named_config = arguments;
    int opt;
    while ((opt = getopt(argc,argv,"c:s:l:e:b:r:")) != EOF)
        switch(opt) {
            case 'c': named_config = NAMED_CONFIG[optarg]; break;
            case 's': arguments.short_pulse = atoi(optarg); break;
            case 'l': arguments.long_pulse = atoi(optarg); break;
            case 'e': arguments.extended_pulse = atoi(optarg); break;
            case 'b': arguments.bit_length = atoi(optarg); break;
            case 'r': arguments.repeat = atoi(optarg); break;
            case '?': usage(); return EXIT_FAILURE;
            default: abort();
        }

    config combined = {
        arguments.short_pulse    ? arguments.short_pulse:    named_config.short_pulse,
        arguments.long_pulse     ? arguments.long_pulse:     named_config.long_pulse,
        arguments.extended_pulse ? arguments.extended_pulse: named_config.extended_pulse,
        arguments.bit_length     ? arguments.bit_length:     named_config.bit_length,
        arguments.repeat         ? arguments.repeat:         named_config.repeat,
    };
    char* data;
    if(argc - optind == 1 && strlen(data = argv[optind]) > 2 && data[0] == '0') {
        std::string bitstring = parseBitString(&data[1], &combined);
        if(isValidConfig(combined) && bitstring.length() >= combined.bit_length) {
            transmit(combined, bitstring);
            return EXIT_SUCCESS;
        }
    }

    usage(); // Failed somewhere above. Print out the usage
    return EXIT_FAILURE;
}
