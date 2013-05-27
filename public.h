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
** $Date: 2004/07/26 18:18:26 $
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

#ifndef PUBLIC_H
#define PUBLIC_H

#include <string>

/*#ifdef X86
#define printX86(fmt,args...)       printf(fmt,## args)
#else
#define printX86(fmt,args...)
#endif*/

std::string trim(std::string &str, char ch = ' ');

#define VERSION		"0.03"
#define FILE_VERSION	"0.03"

#endif
