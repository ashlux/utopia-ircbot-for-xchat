// Useful methods..

#ifndef _USEFUL_H_
#define _USEFUL_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "vars.h"

using namespace std;

int cmp_nocase(const string& s1, const string& s2)
{
  string::const_iterator p1 = s1.begin();
  string::const_iterator p2 = s2.begin();
 
  while (p1!=s1.end() && p2!=s2.end())
  {
    if (toupper(*p1) != toupper(*p2))
      return (toupper(*p1) < toupper(*p2)) ? -1 : 1;
      ++p1;
      ++p2;
  }
  return (s2.size()==s1.size()) ? 0 : (s1.size() < s2.size()) ? -1 : 1;
}

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

static void sendMsg(string to, string msg)
{
  xchat_commandf(ph, "notice %s %s", to.c_str(), msg.c_str());
}

void formatSeconds(double seconds, char *ret)
{
  if (seconds < 0)
  {
    sprintf(ret, "inf");
    return;
  }

  double sec = seconds;
  int w, d, h, m, s;
  
  w = (int)(sec / 604800.0);
  sec = sec - w * 604800.0;
  d = (int)(sec / 86400.0);
  sec = sec - d * 86400.0;
  h = (int)(sec / 3600.0);
  sec = sec - h * 3600.0;
  m = (int)(sec / 60.0);
  s = (int)(sec - m * 60.0);
  
  if (w == 0 && d == 0 && h == 0 && m == 0)
  {
    sprintf(ret, "%ds", s);
  }
  else if (w == 0 && d == 0 && h == 0)
  {
    sprintf(ret, "%dm, %ds", m, s);  
  }
  else if (w == 0 && d == 0)
  {
    sprintf(ret, "%dh, %dm, %ds", h, m, s);  
  }
  else if (w == 0)
  {
    sprintf(ret, "%dd, %dh, %dm, %ds", d, h, m, s);  
  }
  else
  {
    sprintf(ret, "%dw, %dd, %dh, %dm, %ds", w, d, h, m, s);
  }

}

#endif

