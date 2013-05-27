/**********************************************************************
** Copyright (C) 2004 MurphyTalk
**
** This file is part of the MurphyTalk PinYin Chinese input method,
** which is forthe Qtopia Environment.
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

#include <qapplication.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <QWSServer>
#include "PinyinFrame.h"
#include "PinyinImpl.h"

#define IME_WND_FLAG (Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool)

const int COMMA_MARK	=0xff0c;
const int PERIOD_MARK	=0x3002;
const int BOOK_MARK1	=0x300a;
const int BOOK_MARK2	=0x300b;
const int QUESTION_MARK	=0xff1f;
const int COLON_MARK	=0xff1a;

bool isWork = false;

/* XPM of inputmethod*/
static const char * pix_xpm[] = {
"16 13 3 1",
" 	c #FFFFFFFFFFFF",
"#	c #000000000000",
".	c #FFFFFFFFFFFF",
"                ",
"  # ####        ",
"  # ....#       ",
"  # ....#       ",
"  # ....#       ",
"  # # ##        ",
"  #      #...#  ",
"  #      #...#  ",
"  #       ####  ",
"  #       ...#  ",
"  #      #...#  ",
"  #       ###   ",
"                "};
struct QPinyinImplPrivate
{
        static QPinyinImpl* pInputMethod;
};


QPinyinImpl* QPinyinImplPrivate::pInputMethod = NULL;

void QPinyinImpl::installInputMethod()
{
    QPinyinImpl* pim = instance();

    if (pim)
    {
        QWSServer::setCurrentInputMethod(pim);
    }
}

void QPinyinImpl::releaseInputMethod()
{
    if (QPinyinImplPrivate::pInputMethod)
    {
        QWSServer::setCurrentInputMethod(NULL);
        //delete QPinyinImplPrivate::pInputMethod;
        QPinyinImplPrivate::pInputMethod = NULL;
    }
}

QPinyinImpl* QPinyinImpl::instance()
{
    if (NULL == QPinyinImplPrivate::pInputMethod)
    {
        QPinyinImplPrivate::pInputMethod = new QPinyinImpl();
    }

    return QPinyinImplPrivate::pInputMethod;
}


QPinyinImpl::QPinyinImpl()
{  
            m_pinyin_frame = new QPinyinFrame( 0, "MurphyTalk", IME_WND_FLAG );

}

QPinyinImpl::~QPinyinImpl()
{
	delete m_pinyin_frame;
        m_pinyin_frame = NULL;
	delete icn;
}

bool QPinyinImpl::filter (int unicode, int keycode, int modifiers,
                           bool isPress, bool /*autoRepeat*/)
{
    if (isPress && (Qt::AltModifier & modifiers) && (Qt::Key_Z == keycode))
    {
        toggleIME();
        if(isWork)
        {
            isWork = false;
        }
        else
        {
            isWork = true;
        }

        return true;
    }
    else
    {
        if (isWork && isPress) {
                if(m_pinyin_frame->m_bEnglishMode){
                        if(unicode == 9 && keycode == Qt::Key_Tab){
                                m_pinyin_frame->m_bEnglishMode = false;
                                m_pinyin_frame->update();
                                return true;
                        }
                        else if(unicode == 8 && keycode == Qt::Key_Backspace)
                        {
                                return false;
                        }
                        else{
                                SendKey(unicode,keycode);
                                return true;
                        }

                }

                if (GetKey (unicode, keycode))
                        return true;
                else
                        return false;
        }
        else
                return false;
    }

    return false;
}


void QPinyinImpl::toggleIME()
{
    if (m_pinyin_frame->isVisible())
    {
        m_pinyin_frame->hide();
        m_pinyin_frame->releaseKeyboard();
        m_pinyin_frame->resetState();
        m_pinyin_frame->m_engine.save_table();
    }
    else
    {
        m_pinyin_frame->show();
        m_pinyin_frame->grabKeyboard();
    }
}

bool QPinyinImpl::commit_selection(int k)
{
        bool bUpdate = false;
        unsigned int index = ((k-'0')+9)%10;

        if(index<m_pinyin_frame->m_ime_info.candidates_on_page){
                m_pinyin_frame->m_engine.hit(m_pinyin_frame->m_ime_info.first_visible+index);
                if(m_pinyin_frame->m_engine.isPhrase()){
                        QString phrase=m_pinyin_frame->get_phrase(m_pinyin_frame->m_ime_info.first_visible+index);
                        if(m_pinyin_frame->m_bMakingPhrase){
                                m_pinyin_frame->m_ime_info.phrase+=phrase;
                        }
                        else{
                                printf("phrase unicode:");
                                for(int i=0;i<phrase.length();i++){
                                        printf("%04X,",phrase[i].unicode());
                                        SendKey(phrase[i].unicode());
                                }
                                printf("\n");
                        }
                }
                else{
                        if(m_pinyin_frame->m_bMakingPhrase){
                                m_pinyin_frame->m_ime_info.phrase+=m_pinyin_frame->get_char(m_pinyin_frame->m_ime_info.first_visible+index);
                        }
                        else{
                                SendKey(m_pinyin_frame->get_charunicode(index));

                        }
                }
                resetState();
                bUpdate=true;
        }
        return bUpdate;
}

bool QPinyinImpl::send_hanzi_mark(int ascii_mark)
{
        int unicode = 0;
        switch(ascii_mark){
                case ',':
                        unicode = COMMA_MARK;
                        break;
                case '.':
                        unicode = PERIOD_MARK;
                        break;
                case '<':
                        unicode = BOOK_MARK1;
                        break;
                case '>':
                        unicode = BOOK_MARK2;
                        break;
                case '?':
                        unicode = QUESTION_MARK;
                        break;
                case ':':
                        unicode = COLON_MARK;
                        break;
        }
        if(unicode!=0){
                SendKey(unicode);
                return true;
        }
        else{
                return false;
        }
}

bool QPinyinImpl::GetKey(int u, int k/*,int m*/)/*int unicode, int keycode, int modifiers*/
{
        printf("key code is 0x%02X\n",k);
#if 0
        if(m_pinyin_frame->m_bEnglishMode){
                if(u == 9 && k == Qt::Key_Tab){
                        m_pinyin_frame->m_bEnglishMode = false;
                        m_pinyin_frame->update();
                }
                else{
                        SendKey(u,k);
                }
                return true;
        }
#endif
        bool bUpdate         = false;
        bool bKeyProcessed   = true;

        if( k >= '0' && k <= '9'){
commit:
                bUpdate=commit_selection(k);
                if(!bUpdate){
                        bKeyProcessed=false;
                }
        }
        else if(k >= 'a' && k <= 'z'){
input:
                m_pinyin_frame->m_ime_info.pinyin+=(char)k;
                m_pinyin_frame->search();
                printf("Here is IML::search.\n");
                bUpdate=true;
        }
        else if(k >= 'A' && k <= 'Z'){
                k = k - 'A'+ 'a';
                goto input; //m_pinyin_frame->hide();
        }
        else if(k == Qt::Key_Space){
                if(m_pinyin_frame->m_ime_info.candidates_on_page>0){
                        k='1';
                        goto commit;
                }
                else{
                        bKeyProcessed=false;
                }
        }
        else if(k == Qt::Key_Right||k == Qt::Key_Down){
                if(!m_pinyin_frame->next_page()){
                        bKeyProcessed=false;
                }
        }
        else if(k == Qt::Key_Left||k == Qt::Key_Up){
                if(!m_pinyin_frame->prev_page()){
                        bKeyProcessed=false;
                }
        }
        else if( u == 8 && k == Qt::Key_Backspace){
                if(m_pinyin_frame->m_ime_info.pinyin.size()>0){
                        String::iterator pos=m_pinyin_frame->m_ime_info.pinyin.end()-1;
                        m_pinyin_frame->m_ime_info.pinyin.erase(pos);
                        m_pinyin_frame->search();
                        bUpdate=true;
                }
                else{
                    bKeyProcessed=false;
                }
        }
        else if(k == Qt::Key_F12){
                if(m_pinyin_frame->m_bMakingPhrase){
                        //commit the new phrase
                        m_pinyin_frame->m_bMakingPhrase=false;
                        m_pinyin_frame->m_engine.append_phrase(m_pinyin_frame->m_ime_info.phrase,m_pinyin_frame->m_making_phrase_pinyin.c_str());
                        m_pinyin_frame->m_ime_info.phrase="";
                        m_pinyin_frame->resetState();
                        bUpdate=true;
                }
                else if(m_pinyin_frame->m_ime_info.pinyin.size()==0){
                        printf("entering making phrase mode...\n");
                        m_pinyin_frame->m_making_phrase_pinyin="";
                        m_pinyin_frame->m_bMakingPhrase=true;
                        bUpdate=true;
                }
        }
        else if(u == 9 && k == Qt::Key_Tab){
                m_pinyin_frame->m_bEnglishMode=true;
                bUpdate=true;
        }
        else if(  k == Qt::Key_Escape){
                if(m_pinyin_frame->m_bMakingPhrase){
                        m_pinyin_frame->m_ime_info.phrase="";
                        m_pinyin_frame->m_bMakingPhrase=false;
                }
                m_pinyin_frame->resetState();
                bUpdate=true;
        }
        else if(!send_hanzi_mark(k)){
                bKeyProcessed=false;
        }


        if(bUpdate){
                m_pinyin_frame->update();
        }

        if(!bKeyProcessed){
            return false;
        }

        return true;
}

void QPinyinImpl::SendKey(int u , int c)
{
        QString result = QString(QChar(u));
        sendCommitString(result);
        if (m_pinyin_frame->isVisible())
        {
            m_pinyin_frame->releaseKeyboard();
            m_pinyin_frame->resetState();
            //m_pinyin_frame->m_engine.save_table();
        }
}

void QPinyinImpl::resetState()
{
}

QPixmap *QPinyinImpl::icon()
{
	if ( !icn ){
		icn = new QPixmap( (const char **)pix_xpm );
	}
	return icn;
}

QString QPinyinImpl::name()
{
	return qApp->translate( "InputMethods", "MurphyTalk" );
}


