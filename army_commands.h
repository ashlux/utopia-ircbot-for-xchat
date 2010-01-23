// !armyN and !remain related commands

#ifndef _ARMY_COMMANDS_H_
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

using namespace std;

// CAwayArmy class

class CAwayArmy
{
  public:
    string nick, army, setBy;
    int setTime, returnTime;
    bool rang;
        
    xchat_hook *timer;
    
  public:
    CAwayArmy() { }   
    CAwayArmy(string pnick, string parmy, int psetTime, 
              int preturnTime, string psetBy) : nick(pnick),
	      army(parmy), setBy(psetBy), setTime(psetTime), 
	      returnTime(preturnTime), rang(false) { }
};

// COMMAND STUFF ..

static void help_remain(string user)
{
  sendMsg(user, "USAGE: !remain [nick|province|all]");    
}

static void help_army(string user)
{
  sendMsg(user, "USAGE: !armyN xx.yy [nick]");
  sendMsg(user, "  N - Single digit number 0-4 representing the number of generals out.  0 means all generals.");
  sendMsg(user, "  xx.yy - Hours until the troops are back.");
  sendMsg(user, "  nick - optional parameter specifying the nick in IRC.");
}

static void help_delarmy(string user)
{
  sendMsg(user, "USAGE: !delarmyN [nick]");
  sendMsg(user, "  N - Single digit number 0-4 representing the number of generals out.  0 means all generals (use with caution).");
  sendMsg(user, "  nick - optional parameter specifying the nick in IRC.");
}

static void help_fagswitharmyhome(string user)
{
  sendMsg(user, "USAGE: !fagswitharmyhome");
}

static int armyreturn_timeout (void *userdata)
{
      CAwayArmy *temp = (CAwayArmy*)(userdata);
      if (!temp->rang)
      {
	xchat_commandf(ph, "notice %s :%s's %s has returned!",
	               CHANNELS.c_str(), temp->nick.c_str(), temp->army.c_str());
        xchat_commandf(ph, "echo %s :%s's %s has returned!",
	               CHANNELS.c_str(), temp->nick.c_str(), temp->army.c_str());
	temp->rang = true;
      }

      return XCHAT_EAT_NONE;
}

static void loadArmies(vector<CAwayArmy> *awayArmies)
{
  CAwayArmy temp;
  ifstream file(AWAYARMYFILE.c_str(), ios::in);
  if (!file) return;
  time_t currTime = time(NULL);
  while (file >> temp.nick >> temp.army >> temp.setTime >> temp.returnTime >> temp.setBy)
  {
    if (temp.returnTime >= currTime)
    {
      //cout << temp.nick << " " << temp.army << " " << temp.setTime << " " << temp.returnTime << " " << temp.setBy << endl;        
      awayArmies->push_back(temp);
      int t = (temp.returnTime - currTime) * 1000;
      temp.timer = xchat_hook_timer(ph, t, armyreturn_timeout, &awayArmies->back());      
    }
  }
  file.close();
}

static void delarmy(string user, string msg, int armyNum, vector<CAwayArmy> *awayArmies)
{
  string armyN, nick = "NULL";
  
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> armyN >> nick;
  armyN.erase(0,4); // remove "!del" from command
 
  if (armyN == "army")
    armyN = "army0";
  
  if (nick == "NULL")
    nick = user;
  
  bool updates = false;
  
  time_t currTime = time(NULL);
  for (unsigned int i = 0; i < awayArmies->size(); ++i)
  {
    if (awayArmies->operator[](i).nick == nick && 
        (awayArmies->operator[](i).army == armyN || armyN == "army0"))
    {
      awayArmies->operator[](i).returnTime = (int)currTime -1;
      awayArmies->operator[](i).rang = true;
      updates = true;
      //xchat_commandf(ph, "notice %s %s's %s has been removed from away.",
      //               user.c_str(), nick.c_str(), awayArmies->operator[](i).army.c_str());
    }
  }
  
  if (!updates)
  {
    xchat_commandf(ph, "notice %s %s's %s is not away, and has not been removed from away.",
                   user.c_str(), nick.c_str(), armyN.c_str());
    return;
  }
  
  // rewrite data file to reflect changse to away armies
  fstream file(AWAYARMYFILE.c_str(), ios::out);
  for (unsigned int i = 0; i < awayArmies->size(); ++i)
  {
    file << awayArmies->operator[](i).nick    << " " << awayArmies->operator[](i).army       << " " <<
            awayArmies->operator[](i).setTime << " " << awayArmies->operator[](i).returnTime << " " <<
	    awayArmies->operator[](i).setBy   << endl;
  }
  file.close();
  
  sendMsg(user, "-Army removal complete-");
}

static void setarmy(string user, string msg, int armyNum, vector<CAwayArmy> *awayArmies)
{
  string armyN, nick = "NULL";
  double hours;

  stringstream str(msg, ios_base::out|ios_base::in);
  str >> armyN >> hours >> nick;
  
  if (nick == "NULL")
  {
    nick = user;
  }
  
  if (hours >= 36.0 || hours <= 0.01)
  {
    sendMsg(user, "Invalid time inputed (36.0 <= t >= 0.01).");
    return;
  }

  char buffer[33];  
  sprintf(buffer, "%d", armyNum);
  string army = string("army") + buffer;  
    
  time_t currTime = time(NULL);
  int returnTime = (int)(currTime + floor(hours * 3600.0));
  
  fstream file(AWAYARMYFILE.c_str(), ios::out|ios::app);
  file << nick << " " << army << " " << currTime << " " <<
          returnTime << " " << user << endl;
  file.close();
  
  xchat_print(ph, army.c_str());
  
  CAwayArmy temp(nick, army, currTime, returnTime, user);
  awayArmies->push_back(temp);
  int t = (returnTime - currTime) * 1000;
  temp.timer = xchat_hook_timer(ph, t, armyreturn_timeout, &awayArmies->back());
  
  char f[256];
  formatSeconds(temp.returnTime - (int) currTime, f);
  xchat_commandf(ph, "notice %s %s's %s will return in %s.", 
                 CHANNELS.c_str(), temp.nick.c_str(), temp.army.c_str(), f);
    xchat_commandf(ph, "echo %s :%s's %s will return in %s.", 
                 CHANNELS.c_str(), temp.nick.c_str(), temp.army.c_str(), f);
}

static void armyremaining(string user, string msg, vector<CAwayArmy> *awayArmies)
{
  //sendMsg(user, "USAGE: !remain [nick|province|all]");
  string who = user, cmd = "";

  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> who;
  
  time_t currTime = time(NULL);
  
  bool found = false;
  
  for (unsigned int i = 0; i < awayArmies->size(); ++i)
  {
    if (awayArmies->operator[](i).returnTime >= currTime && 
          (cmp_nocase(awayArmies->operator[](i).nick, who) == 0 || 
	   cmp_nocase(who, "all") == 0))
    {
      char f[256];
      formatSeconds(awayArmies->operator[](i).returnTime - (int) currTime, f);
      xchat_commandf(ph, "notice %s %s's %s will return in %s.", 
                     user.c_str(), awayArmies->operator[](i).nick.c_str(),
		     awayArmies->operator[](i).army.c_str(), f);
      found = true;
    }
  }
  
  if (!found)
  {
    xchat_commandf(ph, "notice %s No troops found.", user.c_str());
  }
}

#endif
