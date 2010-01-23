// user related commands

#ifndef _USER_COMMANDS_H_
#define _USER_COMMANDS_H_

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


// CUSER CLASS

class CUser
{
  public:
    string primaryNick, province;
    bool admin;
    vector<string> nicks;

  public:
    CUser(string apNick, string aprov, string aadmin, vector<string> anicks)
           : primaryNick(apNick), province(aprov), nicks(anicks)
    {
      if (aadmin == "Y" || aadmin == "y") admin = true;
      else admin = false;
    }
    
    CUser(string apNick, string aprov, string aadmin)
          : primaryNick(apNick), province(aprov)
    {
      if (aadmin == "Y" || aadmin == "y") admin = true;
      else admin = false;
    }
    
    CUser(string apNick, string aprov, bool aadmin, vector<string> anicks)
          : primaryNick(apNick), province(aprov), admin(aadmin), nicks(anicks) 
    { }
};

// USER COMMAND STUFF
static void help_adduser(string usr)
{
  sendMsg(usr, "USAGE: !adduser primary_nick province");
  sendMsg(usr, "  Setup an account for primary_nick linked to province.");
}

static void help_deluser(string usr)
{
  sendMsg(usr, "USAGE: !deluser nick");
  sendMsg(usr, "  Removes an account from records.");
}

static void help_linknick(string usr)
{
  sendMsg(usr, "USAGE: !linknick [nick] linked_nick");
  sendMsg(usr, "  Links a new nick to nick account.");
}

static void help_setadmin(string usr)
{
  sendMsg(usr, "USAGE: !setadmin nick");
  sendMsg(usr, "  Gives admin access to nick account.");
}

static void help_deladmin(string usr)
{
  sendMsg(usr, "USAGE: !deladmin nick");
  sendMsg(usr, "  Removes admin access to nick account.");
}

static void help_listusers(string usr)
{
  sendMsg(usr, "USAGE: !listusers");
  sendMsg(usr, "  Displays primary nick and province name of each account, as well as admin status.");
}

static void loadUsers(vector<CUser> *users)
{
  ifstream file(USERLISTFILE.c_str(), ios::in);
  if (!file) return;
  
  string nick;
  while (file >> nick)
  {    
    string admin, tempstr;
    vector<string> add_nicks;    
    ifstream ufile((nick + ".dat").c_str(), ios::in);
    if (ufile)
    {
      char prov[1024];
      ufile.getline(prov, 1024);      
      ufile.getline(prov, 1024);
      xchat_printf(ph, "Prov: %s", prov);
      ufile >> admin;
      while (ufile >> tempstr)
      {
        add_nicks.push_back(tempstr);
      }
      
      users->push_back(CUser(nick, prov, admin, add_nicks));
    }
  }
  file.close();
}

static bool isadmin(string user, vector<CUser> *users)
{
  for (unsigned int i = 0; i < users->size(); ++i)
  {
    if (cmp_nocase(users->operator[](i).primaryNick, user) == 0)
      return true;
//    for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
//      if (cmp_nocase(users->operator[](i).nicks[c], user) == 0)
//        return true;
  }
  
  return false;
}

static void listUsers(string user, string msg, vector<CUser> *users)
{
  string adminTag;
  for (unsigned int i = 0; i < users->size(); ++i)
  {    
    if (users->operator[](i).admin) adminTag = "*";
    else adminTag = " ";
    xchat_commandf(ph, "notice %s %s%s controls %s", user.c_str(), adminTag.c_str(),
                   users->operator[](i).primaryNick.c_str(), users->operator[](i).province.c_str());
  }
  
  sendMsg(user, "-End of user list-");
}

static void adduser(string user, string msg, vector<CUser> *users)
{
  // only admins can do this... is this user an admin?
  if (!isadmin(user, users))
  {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
    return;
  }

  string cmd, primnick, prov, tempstr;
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> primnick >> prov;
  
  while (str >> tempstr)
    prov += " " + tempstr;
  
  // does nick or linked nick alredy exist?
  for (unsigned int i = 0; i < users->size(); ++i)
  {
    if (cmp_nocase(users->operator[](i).primaryNick, primnick) == 0)
    {
      sendMsg(user, "Nick already exists.");
      return;
    }
    if (cmp_nocase(users->operator[](i).province, prov) == 0)
    {
      sendMsg(user, "Province already exists.");
      return;
    }
    for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
    {
      if (cmp_nocase(users->operator[](i).nicks[c], primnick) == 0)
      {
        sendMsg(user, "Nick already exists.");
	return;
      }
    }
  }
   
  // nick does not already exist, so add it
  fstream file(USERLISTFILE.c_str(), ios::out|ios::app);
  file << primnick << endl;
  file.close();
  
  fstream ufile((primnick + ".dat").c_str(), ios::out);
  ufile << primnick << endl;
  ufile << prov << endl;
  ufile << "N" << endl;
  ufile.close();
  
  users->push_back(CUser(primnick, prov, "N"));
  
  sendMsg(user, "User created.");
}

static void setadmin(string user, string msg, vector<CUser> *users)
{
  // only admins can do this... is this user an admin?
  if (!isadmin(user, users))
  {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
    return;
  }

  //sendMsg(usr, "USAGE: !setadmin nick");
  string cmd, nick;
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> nick;
  
  for (unsigned int i = 0; i < users->size(); ++i)
  {
    bool found = false;
    if (cmp_nocase(users->operator[](i).primaryNick, nick) == 0)
      found = true;
    for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
      if (cmp_nocase(users->operator[](i).nicks[c], nick) == 0)
        found = true;     
	
    if (found)
    {
      users->operator[](i).admin = true;
      sendMsg(user, "=Admin status updated=");
      fstream ufile((users->operator[](i).primaryNick + ".dat").c_str(), ios::out);
      ufile << users->operator[](i).primaryNick << endl;
      ufile << users->operator[](i).province << endl;
      ufile << "Y" << endl;
      ufile.close();
      return;
    }
  }
  
  sendMsg(user, "Cannot update admin status, nick not found.");
}

 static void deladmin(string user, string msg, vector<CUser> *users)
 {
  // only admins can do this... is this user an admin?
  if (!isadmin(user, users))
  {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
    return;
  }
  
  //sendMsg(usr, "USAGE: !deladmin nick");
  string cmd, nick;
  stringstream str(msg, ios_base::out|ios_base::in);
  str >> cmd >> nick;
  
  for (unsigned int i = 0; i < users->size(); ++i)
  {
    bool found = false;
    if (cmp_nocase(users->operator[](i).primaryNick, nick) == 0)
      found = true;
    for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
      if (cmp_nocase(users->operator[](i).nicks[c], nick) == 0)
        found = true;     
	
    if (found)
    {
      users->operator[](i).admin = false;
      sendMsg(user, "=Admin status updated=");
      fstream ufile((users->operator[](i).primaryNick + ".dat").c_str(), ios::out);
      ufile << users->operator[](i).primaryNick << endl;
      ufile << users->operator[](i).province << endl;
      ufile << "N" << endl;
      ufile.close();
      return;
    }
  }
  
  sendMsg(user, "Cannot update admin status, nick not found.");
 }
  
 static void linknick(string user, string msg, vector<CUser> *users)
 {
   //sendMsg(usr, "USAGE: !linknick nick linked_nick");   
   
   string cmd, nick, linknick = "";
   stringstream str(msg, ios_base::out|ios_base::in);
   str >> cmd >> nick >> linknick;
   
   if (linknick == "")
   {
     linknick = nick;
     nick = user;
   }
     
   
   if (linknick == "")
   {
     sendMsg(user, "Must provide a link nickname.");
     return;
   }
   
   // does linknick already exist?
   for (unsigned int i = 0; i < users->size(); ++i)
   {
     if (cmp_nocase(users->operator[](i).primaryNick, linknick) == 0)
     {
       sendMsg(user, "Specified link nick already exists.");
       return;
     }
     for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
     {
       if (cmp_nocase(users->operator[](i).nicks[c], linknick) == 0)
       {
         sendMsg(user, "Specified link nick already exists.");
	 return;
       }
     }
   }
   
   for (unsigned int i = 0; i < users->size(); ++i)
   {
     bool found = false;
     if (cmp_nocase(users->operator[](i).primaryNick, nick) == 0)
       found = true;
     for (unsigned int c = 0; c < users->operator[](i).nicks.size(); ++c)
       if (cmp_nocase(users->operator[](i).nicks[c], nick) == 0)
         found = true;
	 
     if (found)
     {
       users->operator[](i).nicks.push_back(linknick);
       fstream ufile((users->operator[](i).primaryNick + ".dat").c_str(), ios::out|ios::app);
       ufile << linknick << endl;
       sendMsg(user, "Nick linking complete.");
       return;
     }
   }
   
   sendMsg(user, "Cannot find nick to link."); 
 }
 
 static void deluser(string user, string msg, vector<CUser> *users)
 {
   // only admins can do this... is this user an admin?
   if (!isadmin(user, users))
   {
    sendMsg(user, "You must be an admin to execute this command.  If you are an admin, you MUST be logged in as your primary nick.");
     return;
   } 
 
   sendMsg(user, "Warning: !deluser command not yet available.");
 }

#endif
