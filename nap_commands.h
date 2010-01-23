// !nap and !setnap related commands

#ifndef _NAP_COMMANDS_H_
#define _NAP_COMMANDS_H_

extern "C" {
  #include "xchat-plugin.h"
}

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "useful.h"
#include "user_commands.h"

using namespace std;

/* NAP CLASS */

class CNap
{
  public:
    int kingdomNum, islandNum, setTime, removeTime;
    string setBy;
    bool rang;
    
    xchat_hook *timer;
    
  CNap() { }
  CNap(int kNum, int iNum, int asetTime, int aremoveTime, string asetBy)
       : kingdomNum(kNum), islandNum (iNum), setTime(asetTime),
         removeTime(aremoveTime), setBy(asetBy), rang(false) { }
  CNap(string kingdomLocStr, double days, string asetBy)
       : setBy(asetBy), rang(false)
  {
    time_t currTime = time(NULL);
    setTime = (int)currTime;
    removeTime = setTime + (int)(days * 24.0);
    
    kingdomNum = -1;
    islandNum = -1;
    
    stringstream str(kingdomLocStr, ios_base::out|ios_base::in);
    str >> kingdomNum >> islandNum;
  }
};

/* NAP COMMAND STUFF */

static void help_setnap(string user)
{
  sendMsg(user, "USAGE: !setnap xx:yy [expire]");
  sendMsg(user, "  Set a nap for kingdom xx:yy.");
  sendMsg(user, "  Optional expire time in days (blank or 0 is infinite).");  
}

static void help_nap(string user)
{
  sendMsg(user, "USAGE: !nap [xx:yy|all]");
  sendMsg(user, "  Displays nap status for kingdom xx:yy.");
}

static void help_delnap(string user)
{
  sendMsg(user, "USAGE: !delnap xx:yy");
  sendMsg(user, "  Removes nap for xx:yy, if nap exists.");
}

static int nap_timeout(void *userdata)
{
  CNap *temp = (CNap*)(userdata);
  if (!temp->rang)
  {
    xchat_commandf(ph, "notice %s Nap with %d:%d has expired.",
                   CHANNELS.c_str(), temp->kingdomNum, temp->islandNum);
    xchat_commandf(ph, "echo %s :Nap with %d:%d has expired.",
                   CHANNELS.c_str(), temp->kingdomNum, temp->islandNum);
    temp->rang = true;
  }
  return XCHAT_EAT_NONE;
}

static void loadNaps(vector<CNap> *naps)
{
  CNap temp;
  ifstream file(NAPFILE.c_str(), ios::in);
  if (!file) return;
  time_t currTime = time(NULL);
  while (file >> temp.kingdomNum >> temp.islandNum >> temp.setTime >> temp.removeTime >> temp.setBy)
  {
    if (temp.removeTime >= currTime || temp.removeTime == 0)
    {
      naps->push_back(temp);
      if (temp.removeTime != 0)
      {
        int t = (temp.removeTime - currTime) * 1000;
        temp.timer = xchat_hook_timer(ph, t, nap_timeout, &naps->back());
      }
    }
  }
  file.close();
}

static void delNap(string user, string msg, vector<CNap> *naps, vector<CUser> *users)
{
  if (!isadmin(user, users))
  {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
    return;
  }

  string cmd, pt2 = "0";
  int kNum = 0, iNum = 0;
  
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> kNum >> pt2;
  pt2.erase(0,1); // removes ":"
  iNum = atoi(pt2.c_str());
  
  bool updates = false;
  
  // reset removeTime to current time, set rang to true (stops timer from printing)
  time_t currTime = time(NULL);
  for (unsigned int i = 0; i < naps->size(); ++i)
  {
    if (naps->operator[](i).kingdomNum == kNum && naps->operator[](i).islandNum == iNum)
    {
      naps->operator[](i).removeTime = (int)currTime - 1;
      naps->operator[](i).rang = true;
      updates = true;
    }
  }
  
  if (!updates)
  {
    xchat_commandf(ph, "notice %s Kingdom %d:%d was not deleted (does not exist in nap list).",
                   user.c_str(), kNum, iNum);
    return;
  }    
  
  // rewrite data file to reflect chagnes to nap
  fstream file(NAPFILE.c_str(), ios::out);
  for (unsigned int i = 0; i < naps->size(); ++i)
  {
    file << naps->operator[](i).kingdomNum << " " << naps->operator[](i).islandNum  << " " << 
            naps->operator[](i).setTime    << " " << naps->operator[](i).removeTime << " " << 
	    naps->operator[](i).setBy      << endl;
  }
  file.close();
  
  xchat_commandf(ph, "notice %s Nap for kingdom %d:%d has been removed.",
                 CHANNELS.c_str(), kNum, iNum);
}

static void setNap(string user, string msg, vector<CNap> *naps, vector<CUser> *users)
{
  if (!isadmin(user, users))
  {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
    return;
  }
  
  string cmd, pt2 = "0";
  double days = 0.0;
  int kNum = 0, iNum = 0;
  
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> kNum >> pt2 >> days;
  pt2.erase(0,1); // remove ":" from first position..
  iNum = atoi(pt2.c_str());
  
  if (iNum <= 0 || kNum <= 0 || iNum > 50 || kNum > 13)
  {
    sendMsg(user, "Invalid kingdom or island numbers.  Make sure the format is xx:yy.");
    return;
  }
  
  if (days < 0.01 && days != 0.0)
  {
    sendMsg(user, "Nap should last for at least 0.01 days.");
  }
  
  time_t currTime = time(NULL);
  int returnTime = (int)(currTime + floor(days * 86400.0));
  if (days == 0) { returnTime = 0; }

  fstream file(NAPFILE.c_str(), ios::out|ios::app);
  file << kNum << " " << iNum << " " << (int)currTime << " " <<
          returnTime << " " << user << endl;
  file.close();
    
  CNap temp(kNum, iNum, (int)currTime, returnTime, user);
  naps->push_back(temp);
  int t = (returnTime - currTime) * 1000;
  temp.timer = xchat_hook_timer(ph, t, nap_timeout, &naps->back());
  
  char f[256];
  formatSeconds(temp.removeTime - (int)currTime, f);
  xchat_commandf(ph, "notice %s Nap set for %d:%d (expires in %s).", 
                 CHANNELS.c_str(), kNum, iNum, f);
  xchat_commandf(ph, "echo %s :Nap set for %d:%d (expires in %s).", 
                 CHANNELS.c_str(), kNum, iNum, f);  
}


static void napStatus(string user, string msg, vector<CNap> *naps)
{
  // !nap [xx:yy]  
  string cmd, who = "NULL";
  int kNum = 0, iNum = 0;
  
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> kNum >> who;
  who.erase(0,1); // remove ":"..
  iNum = atoi(who.c_str());
  
  time_t currTime = time(NULL);
  
  bool found = false;
  
  for(unsigned int i = 0; i < naps->size(); ++i)
  {
    if ((naps->operator[](i).removeTime >= (int)currTime || naps->operator[](i).removeTime == 0) &&
        ((kNum == naps->operator[](i).kingdomNum && iNum == naps->operator[](i).islandNum) || iNum == 0))    
    {
      char f[256];
      formatSeconds(naps->operator[](i).removeTime - (int)currTime, f);
      xchat_commandf(ph, "notice %s Nap with %d:%d will end in %s.", 
                     user.c_str(), naps->operator[](i).kingdomNum, 
		     naps->operator[](i).islandNum, f);
      found = true;
    }
  }
  
  if (found)
  {
    sendMsg(user, "-End of nap list.-");
    return;
  }
  
  if (!found && kNum == 0 && iNum == 0)
  {
    xchat_commandf(ph, "notice %s No naps found.", user.c_str());
  }
  else if (!found)
  {
    xchat_commandf(ph, "notice %s No naps found for %d:%d.", user.c_str(), kNum, iNum); 
  }
}

#endif
