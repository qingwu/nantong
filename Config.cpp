/**********************************************************************
** Copyright (C) 2004 MurphyTalk
**
** This file is part of the MurphyTalk PinYin Chinese input method,
** which is for the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**
** murphytalk@gmail.com
** $Date: 2004/07/20 11:28:52 $
**
----------------------------------------------------------------------
Q-Kevin update MurphyTalk to Qt 4.5.1 for Embedded Linux
$Date: May 2, 2009

Update by:  Q-Kevin
email:      qkevin@InsideQt.com
Homepage:   http://www.InsideQt.com/bbs

more detailed information about how to porting MurphyTalk to latest Qt/Embedded Linux,
and How to develop input method on Qt/Embedded Linux,
Please visit http://www.InsideQt.com/bbs
----------------------------------------------------------------------
refeved update MurphyTalk to Qt 4.5.2 for Embedded Linux
Now ,it can really work on QTE
$Date: Oct, 10, 2010

Update by:  refeved
email:      xiaobinl989@126.com
QQ:         45349959

more detailed information about how to porting MurphyTalk to latest Qt/Embedded Linux,
please read the read.doc in the sourcefile.
----------------------------------------------------------------------
**
**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "Config.h"
#include "public.h"

const int MAXLEN = 256;

Config::Config(const char* config_file)
{
	std::ifstream ifs(config_file);
	std::string line;
	char key[MAXLEN],value[MAXLEN];
	std::string  sKey,sValue;
		
	while(ifs)
	{
		ifs>>line;
                if(ifs.eof()) break;

		int sPos = line.find_first_of('=');
		if(sPos != -1){
			line.replace(sPos,1," ");
		}

		key[0]=value[0]=0;
		sscanf(line.c_str(),"%s %s",key,value);
		sKey=key;
		sValue=value;
		trim(sKey);
		trim(sValue);
		trim(sValue,'\n');
		
		if(sKey.size()>0&&sValue.size()>0){
			if(sKey[0]!=';'){
				m_values[sKey]=sValue;
                                printf("%s=%s\n",sKey.c_str(),sValue.c_str());
			}
		}
	}
}

std::string Config::get_as_string(const char* key_name)
{
	std::string s(key_name);
	if(m_values.find(s)!=m_values.end()){
		return m_values[s];
	}
	else{
		s="";
		return s;
	}
}

int Config::get_as_int(const char* key_name)
{
	std::string s=get_as_string(key_name);
	if(s.size()>0){
		return atoi(s.c_str());
	}
	else{
		return 0;
	}
}

/*
 * Revision history
 * 
 * $Log: Config.cpp,v $
 * Revision 1.1  2004/07/20 11:28:52  Lu Mu
 * created
 *
 */
