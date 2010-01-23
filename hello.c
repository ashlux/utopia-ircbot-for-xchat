//  xchat_commandf(ph, "msg %s %s...0", "ashlux666", word_eol[0]);  

#include "xchat-plugin.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
  
/*
struct armies
{
  char *nick;
  char *army;
  int setTime;
  int returnTime;
  char *setBy;
}
*/

static xchat_plugin *ph; /* plugin handle */ 

/*
static xchat_hook *timers;
static int *returntimes;
*/

/*
static void initData()
{
  FILE *fp;
  fp = fopen("/root/armies.dat", "r");
  
  char nick[51], army[10], setBy[50];
  int setAt, returnAt;
  time_t currTime;
  time (&currTime);
  
  while (fscanf(fp, "%s %s %d %d %s", nick, army, &setAt, &returnAt, setBy) != EOF)
  {
    int timediff = returnAt - currTime;
    if (timediff > 0)
    {
     
     char ret[51];
     formatSeconds(timediff, ret);
        
     xchat_commandf(ph, "msg %s %s's %s will return in %s second(s).", usr, nick, army, ret);
    }
  }
  
  fclose(fp);
}
*/

void print(char *to, char *text)
{
  xchat_commandf(ph, "msg %s %s", to, text);
}

void formatSeconds(double seconds, char *ret)
{
  double sec = seconds;
  int h, m, s;
  
  h = sec / 3600;
  sec = sec - h * 3600;
  m = sec / 60;
  s = sec - m * 60;
  
  sprintf(ret, "%d hours, %d minutes, %d seconds", h, m, s);

}

static void displayhelp(char *usr, char *word[])
{
  print(usr, "Available commands:");
  print(usr, " ");
  print(usr, "  !HELP");
  print(usr, "  !ARMY      !ARMY1      !ARMY2      !ARMY3      !ARMY4");
  print(usr, "  !SETARMY   !SETARMY1   !SETARMY2   !SETARMY3   !SETARMY4");
  print(usr, "  !REMAIN    !FAGSWITHARMYHOME");
}

void setarmyaway(char *usr, char *word, int armyNumber)
{    
  char cmd[51], nick[51];
  double hours;
  sscanf(word, "%s %lf %s", cmd, &hours, nick);
       
  if (hours < 0.0 || hours > 36.0)
  {
    print(usr, "ERROR: Invalid away time entered.");
    return;
  }
  
  double numSeconds = hours * 3600;
  
  xchat_commandf(ph, "msg %s %f = %f", usr, hours, numSeconds);
  
  time_t currTime;
  time (&currTime);
  
  FILE *fp;
  fp = fopen("/root/armies.dat", "a+t");
  if (fp != NULL)
  {
    
    /* nick armyX setAt returnAt setBy */
    int returnTime = (int)(currTime) + numSeconds;
    fprintf(fp, "%s army%d %d %d %s\n", nick, armyNumber, (int)currTime, returnTime, usr);
    fclose(fp);
  }
  else
  {
    print(usr, "Error opening data file.");
    return;
  }

}

void remain(char *usr, char *word)
{
  char cmd[51], param_nick[51];
  strcpy(param_nick, "ALL");
  sscanf(word, "%s %s", cmd, param_nick);
   
  FILE *fp;
  fp = fopen("/root/armies.dat", "r");
    
  int all = 0;
  if (strcasecmp(param_nick, "") == 0 || strcasecmp(param_nick, "ALL") == 0 || strcasecmp(param_nick, "NULL") == 0)
  {
    all = 1;
  }
  
  char nick[51], army[10], setBy[50];
  int setAt, returnAt;
  time_t currTime;
  time (&currTime);
  
  while (fscanf(fp, "%s %s %d %d %s", nick, army, &setAt, &returnAt, setBy) != EOF)
  {
    if (all == 1 || strcasecmp(param_nick, nick) == 0)
    {    
      int timediff = returnAt - currTime;
      if (timediff > 0)
      {
       
       char ret[51];
       formatSeconds(timediff, ret);
        
	xchat_commandf(ph, "msg %s %s's %s will return in %s second(s).", usr, nick, army, ret);
      }
    }
  }
    
  fclose(fp);
}

static int dispatch(char *word[], void *userdata)
{
  /*print ("ashlux666", word[0]);
  print ("ashlux666", word[1]);
  print ("ashlux666", word[2]);*/
   
  char usr[51], msg[501], cmd[51];
  sscanf(word[1], "%s", usr);
  strncpy(msg, word[2], 500);
  sscanf(word[2], "%s", cmd);
  
  //print ("ashlux666", usr);
  //print ("ashlux666", msg);
  //print ("ashlux666", cmd);
    
  if ((strcasecmp(cmd, "!ARMY") == 0) || (strcasecmp(cmd, "!SETARMY") == 0))
  {
    setarmyaway(usr, msg, 0);
  }
  else if ((strcasecmp(cmd, "!ARMY1") == 0) || (strcasecmp(cmd, "!SETARMY1") == 0))
  {
    setarmyaway(usr, msg, 1);
  }
  else if ((strcasecmp(cmd, "!ARMY2") == 0) || (strcasecmp(cmd, "!SETARMY2") == 0))
  {
    setarmyaway(usr, msg, 2);
  }
  else if ((strcasecmp(cmd, "!ARMY3") == 0) || (strcasecmp(cmd, "!SETARMY3") == 0))
  { 
    setarmyaway(usr, msg, 3);
  }
  else if ((strcasecmp(cmd, "!ARMY4") == 0) || (strcasecmp(cmd, "!SETARMY4") == 0))
  {
    setarmyaway(usr, msg, 4);
  }
  else if ((strcasecmp(cmd, "!REMAIN") == 0))
  {
    remain(usr, msg);
  }
  else if ((strcasecmp(cmd, "!HELP") == 0) || (strcasecmp(cmd, "!COMMANDS") == 0))
  {
    displayhelp(usr, word);
  }
  else if (cmd[0] == '!')
  {
    xchat_commandf(ph, "msg %s Unreconized command: '%s'", usr, cmd);
  }
  
  return XCHAT_EAT_NONE;  /* eat this command so xchat and other plugins can't process it */
}

int xchat_plugin_init(xchat_plugin *plugin_handle, 
                      char **plugin_name,
		      char **plugin_desc,
		      char **plugin_version,
		      char *arg)
{
  /* need to save for use with any xchat_* functions */
  ph = plugin_handle;
  
  *plugin_name = "IRC Utopia";
  *plugin_desc = "";
  *plugin_version = "0.1";
  
  //xchat_hook_server(ph, "RAW LINE", XCHAT_PRI_NORM, dispatch, NULL);
  xchat_hook_print(ph, "Channel Message", XCHAT_PRI_NORM, dispatch, NULL);
  xchat_hook_print(ph, "Private Message", XCHAT_PRI_NORM, dispatch, NULL);
  xchat_hook_print(ph, "Private Message to Dialog", XCHAT_PRI_NORM, dispatch, NULL);
  xchat_print(ph, "IRC Utopia loaded successfully!\n");
  
  return 1;  /* return 1 on success */
}
