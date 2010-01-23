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
#include "nap_commands.h"
#include "army_commands.h"
#include "user_commands.h"

using namespace std;

vector<CAwayArmy> awayArmies;
vector<CNap>      naps;
vector<CUser>     users;

static void help(string user, string msg)
{
  string option = "", cmd = "";

  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> option;
  
  if (option == "") // offer general help
  {
    sendMsg(user, "Possible commands:");
    sendMsg(user, "  !army    !army1    !army2     !army3    !army4    !remain !allremain");
    sendMsg(user, "  !delarmy !delarmy1 !delarmy2  !delarmy3 !delarmy4 !fagswitharmyhome");
    sendMsg(user, "  !nap     !setnap   !delnap");
    sendMsg(user, "  !adduser !deluser  !listusers !deladmin !setadmin !linknick");
    sendMsg(user, "  !help");
    sendMsg(user, " \n");
    sendMsg(user, "For more information on a given command x, type !help x.");
  }
  else if (option == "!army"      || option == "!army0"     || 
            option == "!army1"    || option == "!army2"     || 
	    option == "!army3"    || option == "!army4"     || 
	    option == "army"      || option == "army0"      || 
	    option == "army1"     || option == "army2"      || 
	    option == "army3"     || option == "army4")          {    help_army(user);              }
  else if (option == "!remain"    || option == "!allremain" ||
            option == "remain"    || option == "allremain")      {    help_remain(user);            }
  else if (option == "!help"      || option == "help")           {    sendMsg(user, "dumbass");     }
  else if (option == "!nap"       || option == "nap")            {    help_nap(user);               }
  else if (option == "!setnap"    || option == "setnap")         {    help_setnap(user);            }
  else if (option == "!fagswitharmyhome" || 
           option == "fagswitharmyhome")                         {    help_fagswitharmyhome(user);  }
  else if (option == "!delarmy"   || option == "delarmy"      ||     
            option == "!delarmy0" || option == "delarmy0"     ||    
            option == "!delarmy1" || option == "delarmy1"     ||    
            option == "!delarmy2" || option == "delarmy2"     ||    
            option == "!delarmy3" || option == "delarmy3"     ||    
            option == "!delarmy4" || option == "delarmy4")         {    help_delarmy(user);         }
  else if (option == "!delnap"    || option == "delnap")           {    help_delnap(user);          }
  else if (option == "!adduser"   || option == "adduser")          {    help_adduser(user);         }
  else if (option == "!setadmin"  || option == "setadmin")         {    help_setadmin(user);        }
  else if (option == "!deladmin"  || option == "deladmin")         {    help_deladmin(user);        }
  else if (option == "!deluser"   || option == "deluser")          {    help_deluser(user);         }
  else if (option == "!linknick"  || option == "linknick")         {    help_linknick(user);        }
  else if (option == "!listusers" || option == "listusers")        {    help_listusers(user);       }  
  else
  {
    sendMsg(user, "No help available for " + option);
  }
}


static int dispatch(char *word[], void *userdata)
{
  string usr = word[1], msg = word[2], cmd = "";

  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd;
  
  if (cmd == "!army" || 
      cmd == "!army0")             {    setarmy(usr, msg, 0, &awayArmies);               }
  else if (cmd == "!army1")        {    setarmy(usr, msg, 1, &awayArmies);               }
  else if (cmd == "!army2")        {    setarmy(usr, msg, 2, &awayArmies);               }
  else if (cmd == "!army3")        {    setarmy(usr, msg, 3, &awayArmies);               }
  else if (cmd == "!army4")        {    setarmy(usr, msg, 4, &awayArmies);               }
  else if (cmd == "!delarmy" ||
           cmd == "!delarmy0")     {    delarmy(usr, msg, 0, &awayArmies);               }
  else if (cmd == "!delarmy1")     {    delarmy(usr, msg, 1, &awayArmies);               }
  else if (cmd == "!delarmy2")     {    delarmy(usr, msg, 2, &awayArmies);               }
  else if (cmd == "!delarmy3")     {    delarmy(usr, msg, 3, &awayArmies);               }
  else if (cmd == "!delarmy4")     {    delarmy(usr, msg, 4, &awayArmies);               }
  else if (cmd == "!help")         {    help(usr, msg);                                  }
  else if (cmd == "!remain")       {    armyremaining(usr, msg, &awayArmies);            }
  else if (cmd == "!allremain")    {    armyremaining(usr, "!remain all", &awayArmies);  }
  else if (cmd == "!nap" || 
           cmd == "!naps")         {    napStatus(usr, msg, &naps);                      }
  else if (cmd == "!setnap")       {    setNap(usr, msg, &naps, &users);                 }
  else if (cmd == "!delnap")       {    delNap(usr, msg, &naps, &users);                 }
  else if (cmd == "!listusers")    {    listUsers(usr, msg, &users);                     }
  else if (cmd == "!adduser")      {    adduser(usr, msg, &users);                       }
  else if (cmd == "!setadmin")     {    setadmin(usr, msg, &users);                      }
  else if (cmd == "!deladmin")     {    deladmin(usr, msg, &users);                      }
  else if (cmd == "!linknick")     {    linknick(usr, msg, &users);                      }
  else if (cmd == "!deluser")      {    deluser(usr, msg, &users);                       }
  else if (cmd.c_str()[0] == '!')  {    sendMsg(usr, "Unreconized command: " + cmd);     }
  
   
  return XCHAT_EAT_NONE;
}


// THIS IS OUR ENTRY POINT FOR XCHAT

extern "C" {
  int xchat_plugin_init(xchat_plugin *plugin_handle, 
                        char **plugin_name,
		        char **plugin_desc,
		        char **plugin_version,
		        char *arg)
{
    // need to save for use with any xchat_* functions
    ph = plugin_handle;
  
    *plugin_name = "IRC Utopia";
    *plugin_desc = "";
    *plugin_version = "0.1";
  
    //xchat_hook_server(ph, "RAW LINE", XCHAT_PRI_NORM, dispatch, NULL);
    xchat_hook_print(ph, "Channel Message", XCHAT_PRI_NORM, dispatch, NULL);
    xchat_hook_print(ph, "Private Message", XCHAT_PRI_NORM, dispatch, NULL);
    xchat_hook_print(ph, "Private Message to Dialog", XCHAT_PRI_NORM, dispatch, NULL);
    
    loadUsers(&users);
    loadArmies(&awayArmies);
    loadNaps(&naps);
    
    xchat_print(ph, "IRC Utopia loaded successfully!\n");
    
    return 1;  /* return 1 on success */
  }
}
