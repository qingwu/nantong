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
** $Date: 2004/07/10 15:02:23 $
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
#ifndef PINYIMPL_H
#define PINYIMPL_H

//#include <qpe/inputmethodinterface.h>
#include <QWSInputMethod>

class QPinyinFrame;
class QPixmap;

class QPinyinImpl : public QWSInputMethod
{
public:
    QPinyinImpl();
    virtual ~QPinyinImpl();

// #ifndef QT_NO_COMPONENT
//     QRESULT queryInterface( const QUuid&, QUnknownInterface** );
//     Q_REFCOUNT
// #endif

//  By refeved
//  virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f );

    virtual void resetState();
    virtual QPixmap *icon();
    virtual QString name();
//    virtual void onKeyPress( QObject *receiver, const char *slot );

//update by refeved

    static void installInputMethod();
    static void releaseInputMethod();
    static QPinyinImpl* instance();

    virtual bool filter(int unicode, int keycode, int modifiers,
                        bool isPress, bool autoRepeat);
    void toggleIME();

    bool send_hanzi_mark(int ascii_mark);
    bool GetKey(int,int);
    void SendKey ( int  , int c = 0);
    bool commit_selection(int k);

private:
    QPinyinFrame *m_pinyin_frame;
    QPixmap *icn;
    ulong ref;
};

#endif

/*
 * Revision history
 * 
 * $Log: PinyinImpl.h,v $
 * Revision 1.2  2004/07/10 15:02:23  Lu Mu
 * v0.0.1 released
 * TODO: phase support
 *
 * Revision 1.1.1.1  2004/07/07 16:24:13  Lu Mu
 * Created
 *
 *
 */
