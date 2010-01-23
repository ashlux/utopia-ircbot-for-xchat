// required variables

#ifndef _VARS_H_
#define _VARS_H_

extern "C" {
  #include "xchat-plugin.h"
}

#include <string>

using namespace std;

xchat_plugin *ph;

const string CHANNELS     = "#ashtest";		// list channels or nicks, delimited by comma
const string NAPFILE      = "/root/naps.dat";
const string AWAYARMYFILE = "/root/armies.dat";
const string USERLISTFILE = "/root/users.dat";

#endif
