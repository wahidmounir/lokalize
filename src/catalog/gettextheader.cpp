/*
This file is part of Lokalize

Copyright 2008 Nick Shaforostoff <shaforostoff@kde.ru>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gettextheader.h"

#include "project.h"

#include "version.h"
#include "prefs_lokalize.h"

#include <QProcess>
#include <QString>
#include <QMap>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdatetime.h>

#include <kio/netaccess.h>
#include <ktemporaryfile.h>

#if 0
pInfo.insert("cs", "nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;");
pInfo.insert("da", "nplurals=2; plural=(n != 1);");
pInfo.insert("de", "nplurals=2; plural=(n != 1);");
pInfo.insert("el", "nplurals=2; plural=(n != 1);");
pInfo.insert("en", "nplurals=2; plural=(n != 1);");
pInfo.insert("en_GB", "nplurals=2; plural=(n != 1);");
pInfo.insert("en_US", "nplurals=2; plural=(n != 1);");
pInfo.insert("eo", "nplurals=2; plural=(n != 1);");
pInfo.insert("es", "nplurals=2; plural=(n != 1);");
pInfo.insert("et", "nplurals=2; plural=(n != 1);");
pInfo.insert("fi", "nplurals=2; plural=(n != 1);");
pInfo.insert("fo", "nplurals=2; plural=(n != 1);");
pInfo.insert("fr", "nplurals=2; plural=(n > 1);");
pInfo.insert("ga", "nplurals=3; plural=n==1 ? 0 : n==2 ? 1 : 2;");
pInfo.insert("he", "nplurals=2; plural=(n != 1);");
pInfo.insert("hr", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("hu", "nplurals=2; plural=(n != 1);");
pInfo.insert("it", "nplurals=2; plural=(n != 1);");
pInfo.insert("ja", "nplurals=1; plural=0;");
pInfo.insert("ko", "nplurals=1; plural=0;");
pInfo.insert("lt", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("lv", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n != 0 ? 1 : 2);");
pInfo.insert("nb", "nplurals=2; plural=(n != 1);");
pInfo.insert("nl", "nplurals=2; plural=(n != 1);");
pInfo.insert("nn", "nplurals=2; plural=(n != 1);");
pInfo.insert("pl", "nplurals=3; plural=(n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("pt", "nplurals=2; plural=(n != 1);");
pInfo.insert("pt_BR", "nplurals=2; plural=(n > 1);");
pInfo.insert("ro", "nplurals=3; plural=n==1 ? 0 : (n==0 || (n%100 > 0 && n%100 < 20)) ? 1 : 2;");
pInfo.insert("ru", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("sk", "nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;");
pInfo.insert("sl", "nplurals=4; plural=(n%100==1 ? 1 : n%100==2 ? 2 : n%100==3 || n%100==4 ? 3 : 0);");
pInfo.insert("sr", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("sr@latin", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("sv", "nplurals=2; plural=(n != 1);");
pInfo.insert("tr", "nplurals=1; plural=0;");
pInfo.insert("uk", "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);");
pInfo.insert("vi", "nplurals=1; plural=0;");
#endif

/**
 * this data was obtained by running GNUPluralForms()
 * on all languages KDE knows of
**/
#define NUM_LANG_WITH_INFO 38
static const char* langsWithPInfo[NUM_LANG_WITH_INFO]={
"cs",
"da",
"de",
"el",
"en",
"en_GB",
"en_US",
"eo",
"es",
"et",
"fi",
"fo",
"fr",
"ga",
"he",
"hr",
"hu",
"it",
"ja",
"ko",
"lt",
"lv",
"nb",
"nl",
"nn",
"pl",
"pt",
"pt_BR",
"ro",
"ru",
"sk",
"sl",
"sr",
"sr@latin",
"sv",
"tr",
"uk",
"vi"
// '\0'
};

static const char* pInfo[NUM_LANG_WITH_INFO]={
"nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n > 1);",
"nplurals=3; plural=n==1 ? 0 : n==2 ? 1 : 2;",
"nplurals=2; plural=(n != 1);",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=1; plural=0;",
"nplurals=1; plural=0;",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n != 0 ? 1 : 2);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n != 1);",
"nplurals=3; plural=(n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=2; plural=(n != 1);",
"nplurals=2; plural=(n > 1);",
"nplurals=3; plural=n==1 ? 0 : (n==0 || (n%100 > 0 && n%100 < 20)) ? 1 : 2;",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=3; plural=(n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2;",
"nplurals=4; plural=(n%100==1 ? 1 : n%100==2 ? 2 : n%100==3 || n%100==4 ? 3 : 0);",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=2; plural=(n != 1);",
"nplurals=1; plural=0;",
"nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);",
"nplurals=1; plural=0;"
};


int numberOfPluralFormsFromHeader(const QString& header)
{
    QRegExp rxplural("Plural-Forms:\\s*nplurals=(.);");
    if (rxplural.indexIn(header) == -1)
        return 0;
    bool ok;
    int result=rxplural.cap(1).toShort(&ok);
    return ok?result:0;

}

int numberOfPluralFormsForLangCode(const QString& langCode)
{
    QString expr=GNUPluralForms(langCode);

    QRegExp rxplural("nplurals=(.);");
    if (rxplural.indexIn(expr) == -1)
        return 0;
    bool ok;
    int result=rxplural.cap(1).toShort(&ok);
    return ok?result:0;

}

QString GNUPluralForms(const QString& lang)
{
    QByteArray l(lang.toUtf8());
    int i=NUM_LANG_WITH_INFO;
    while(--i>=0)
    {
        if (l==langsWithPInfo[i])
            break;
    }
    //if (KDE_ISLIKELY( langsWithPInfo[i]))
    if (KDE_ISLIKELY( i>=0 ))
        return QString::fromLatin1(pInfo[i]);


    //BEGIN alternative
    // NOTE does this work under M$ OS?
    QString def="nplurals=2; plural=n != 1;";

    QStringList arguments;
    arguments << "-l" << lang
              << "-i" << "-"
              << "-o" << "-"
              << "--no-translator"
              << "--no-wrap";
    QProcess msginit;
    msginit.start("msginit", arguments);

    msginit.waitForStarted(5000);
    if (KDE_ISUNLIKELY( msginit.state()!=QProcess::Running ))
    {
        kWarning()<<"msginit error";
        return def;
    }

    msginit.write(
                   "# SOME DESCRIPTIVE TITLE.\n"
                   "# Copyright (C) YEAR Free Software Foundation, Inc.\n"
                   "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n"
                   "#\n"
                   "#, fuzzy\n"
                   "msgid \"\"\n"
                   "msgstr \"\"\n"
                   "\"Project-Id-Version: PACKAGE VERSION\\n\"\n"
                   "\"POT-Creation-Date: 2002-06-25 03:23+0200\\n\"\n"
                   "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n"
                   "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n"
                   "\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n"
                   "\"MIME-Version: 1.0\\n\"\n"
                   "\"Content-Type: text/plain; charset=UTF-8\\n\"\n"
                   "\"Content-Transfer-Encoding: ENCODING\\n\"\n"
//                   "\"Plural-Forms: nplurals=INTEGER; plural=EXPRESSION;\\n\"\n"
                  );
    msginit.closeWriteChannel();

    if (KDE_ISUNLIKELY( !msginit.waitForFinished(5000) ))
    {
        kWarning()<<"msginit error";
        return def;
    }


    QByteArray result = msginit.readAll();
    int pos = result.indexOf("Plural-Forms: ");
    if (KDE_ISUNLIKELY( pos==-1 ))
    {
        kWarning()<<"msginit error"<<result;
        return def;
    }
    pos+=14;

    int end = result.indexOf('"',pos);
    if (KDE_ISUNLIKELY( pos==-1 ))
    {
        kWarning()<<"msginit error"<<result;
        return def;
    }

    return QString( result.mid(pos,end-pos-2) );
    //END alternative
}


void updateHeader(QString& header,
                  QString& comment,
                  QString& langCode,
                  int& numberOfPluralForms,
                  const QString& CatalogProjectId,
                  bool generatedFromDocbook,
                  bool forSaving)
{
    QStringList headerList(header.split('\n',QString::SkipEmptyParts));
    QStringList commentList(comment.split('\n',QString::SkipEmptyParts));

//BEGIN header itself
    QStringList::Iterator it,ait;
    QString temp;


    bool found=false;

    temp="Last-Translator: "+Settings::authorName();
    if (!Settings::authorEmail().isEmpty())
        temp+=(" <"+Settings::authorEmail()+'>');
    temp+="\\n";


    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *Last-Translator:.*")))
        {
            if (forSaving)
                (*it) = temp;
            found=true;
        }
    }

    if (KDE_ISUNLIKELY( !found ))
        headerList.append(temp);

    QString dateTimeString = KDateTime::currentLocalDateTime().toString("%Y-%m-%d %H:%M%z");
    found=false;
    temp="PO-Revision-Date: "+dateTimeString+"\\n";

    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *PO-Revision-Date:.*")))
        {
            if (forSaving)
                (*it) = temp;
            found=true;
        }
    }
    if (KDE_ISUNLIKELY( !found ))
        headerList.append(temp);

    found=false;

    temp="Project-Id-Version: "+CatalogProjectId+"\\n";
    temp.remove(".pot");
    temp.remove(".po");
    //temp.replace( "@PACKAGE@", packageName());

    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *Project-Id-Version:.*")))
        {
            if (it->contains("PACKAGE VERSION"))
                (*it) = temp;
            found=true;
        }
    }
    if (KDE_ISUNLIKELY( !found ))
        headerList.append(temp);

    found=false;
    langCode=Project::instance()->isLoaded()?
             Project::instance()->langCode():
             Settings::defaultLangCode();

    KConfig lll("all_languages", KConfig::NoGlobals, "locale");
    lll.setLocale("");
    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *Language-Team:.*")))
        {
            found=true;
            //really parse header
            QMap<QString,QString> map;
            QStringList langlist = KGlobal::locale()->languageList();
            QStringList::const_iterator myit;
            for (myit=langlist.begin();myit!=langlist.end();++myit)
            {
                KConfigGroup cg(&lll, *myit);
                map[cg.readEntry("Name")]=*myit;
            }

            QRegExp re("^ *Language-Team: *(.*) *<");
            QString val;
            if ((re.indexIn(*it) != -1) && (map.contains( val=re.cap(1).trimmed() )))
                langCode=map.value(val);

            ait=it;
        }
    }
    //language=locale.languageCodeToName(d->_langCode);
    KConfigGroup cg(&lll, langCode);
    QString language=cg.readEntry("Name");
    if (language.isEmpty())
        language=langCode;

    temp="Language-Team: "+language;
    if (Project::instance()->isLoaded())
        temp+=" <"+Project::instance()->mailingList()+'>';
    else
        temp+=" <"+Settings::defaultMailingList()+'>';
/*    if (!identityOptions->readEntry("DefaultMailingList").isEmpty())
        temp+=(" <"+identityOptions->readEntry("DefaultMailingList")+'>');*/
    temp+="\\n";

    if (KDE_ISLIKELY( found ))
        (*ait) = temp;
    else
        headerList.append(temp);


//     if(!usePrefs || saveOptions.updateCharset)
//     {
//         found=false;
//
//         QString encodingStr;
//         if(saveOptions.useOldEncoding && d->fileCodec)
//         {
//             encodingStr = charsetString(d->fileCodec);
//         }
//         else
//         {
//             encodingStr=charsetString(saveOptions.encoding);
//         }
//         it = headerList.begin();
//         while( it != headerList.end() )
//         {
//             if( it->contains( QRegExp( "^ *Content-Type:.*" ) ) )
//             {
//                 if ( found )
//                 {
//                     // We had already a Content-Type, so we do not need a duplicate
//                     it = headerList.erase( it );
//                 }
//                 else
//                 {
//                     found=true;
//                     QRegExp regexp( "^ *Content-Type:(.*/.*);?\\s*charset=.*$" );
//                     QString mimeType;
//                     if ( regexp.indexIn( *it )!=-1 )
//                     {
//                         mimeType = regexp.cap( 1 ).trimmed();
//                     }
//                     if ( mimeType.isEmpty() )
//                     {
//                         mimeType = "text/plain";
//                     }
//                     temp = "Content-Type: ";
//                     temp += mimeType;
//                     temp += "; charset=";
//                     temp += encodingStr;
//                     temp += "\\n";
//                     (*it) = temp;
//                 }
//             }
//             ++it;
//         }
//         if(!found)
//         {
//             headerList.append(temp);
//         }
//     }
    found=false;
    temp="Content-Transfer-Encoding: 8bit\\n";

    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *Content-Transfer-Encoding:.*")))
            found=true;
    }
    if (!found)
        headerList.append(temp);

    temp="X-Generator: Lokalize %1\\n";
    temp=temp.arg(KAIDER_VERSION);
    found=false;

    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *X-Generator:.*")))
        {
            (*it) = temp;
            found=true;
        }
    }
    if (KDE_ISUNLIKELY( !found ))
        headerList.append(temp);

    // ensure MIME-Version header
    temp="MIME-Version: 1.0\\n";
    found=false;
    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *MIME-Version:")))
        {
            (*it) = temp;
            found=true;
        }
    }
    if (KDE_ISUNLIKELY( !found ))
    {
        headerList.append(temp);
    }


    found=false;

    kDebug()<<"testing for GNUPluralForms";
    // update plural form header
    for ( it = headerList.begin(); it != headerList.end(); ++it )
    {
        if (it->contains(QRegExp("^ *Plural-Forms:")))
        {
            found=true;
            break;
        }
    }
    if (found)
    {
        kDebug()<<"GNUPluralForms found";
        int num=numberOfPluralFormsFromHeader(header);
        if (!num)
        {
            if (generatedFromDocbook)
                num=1;
            else
            {
                kWarning()<<"No plural form info in header, using project-defined one"<<langCode;
                QString t=GNUPluralForms(langCode);
                kWarning()<<"generated: " << t;
                if ( !t.isEmpty() )
                {
                    QRegExp pf("^ *Plural-Forms:\\s*nplurals.*\\\\n");
                    pf.setMinimal(true);
                    temp=QString("Plural-Forms: %1\\n").arg(t);
                    it->replace(pf,temp);
                    num=numberOfPluralFormsFromHeader(temp);
                }
                else
                {
                    kWarning()<<"no... smth went wrong :(\ncheck your gettext install";
                    num=2;
                }
            }
        }
        //setNumberOfPluralForms(num);
        numberOfPluralForms=num;

    }
    else if ( !generatedFromDocbook)
    {
        kDebug()<<"generating GNUPluralForms"<<langCode;
        QString t= GNUPluralForms(langCode);
        kDebug()<<"here it is:";
        if ( !t.isEmpty() )
            headerList.append(QString("Plural-Forms: %1\\n").arg(t));
    }

    //m_header.setMsgstr( headerList.join( "\n" ) );
    header=headerList.join("\n");

//END header itself

//BEGIN comment = description, copyrights
    found=false;

    for ( it = commentList.begin(); it != commentList.end(); ++it )
    {
        // U+00A9 is the Copyright sign
        if ( it->contains( QRegExp("^# *Copyright (\\(C\\)|\\x00a9).*Free Software Foundation, Inc") ) )
        {
            found=true;
            break;
        }
    }
    if (found)
    {
        if ( it->contains( QRegExp("^# *Copyright (\\(C\\)|\\x00a9) YEAR Free Software Foundation, Inc\\.") ) )
        {
            //template string
//     		if( saveOptions.FSFCopyright == ProjectSettingsBase::Remove)
            it->remove(" YEAR Free Software Foundation, Inc");
            /*		else
            		    it->replace("YEAR", QDate::currentDate().toString("yyyy"));*/
        } /*else
                        	    if( saveOptions.FSFCopyright == ProjectSettingsBase::Update )
                        	    {
                        		    //update years
                        		    QString cy = QDate::currentDate().toString("yyyy");
                        		    if( !it->contains( QRegExp(cy)) ) // is the year already included?
                        		    {
                        			int index = it->lastIndexOf( QRegExp("[\\d]+[\\d\\-, ]*") );
                        			if( index == -1 )
                        			{
                        			    KMessageBox::information(0,i18n("Free Software Foundation Copyright does not contain any year. "
                        			    "It will not be updated."));
                        			} else {
                        			    it->insert(index+1, QString(", ")+cy);
                        			}
                        		    }
                        	    }*/
    }
#if 0
    if ( ( !usePrefs || saveOptions.updateDescription )
            && ( !saveOptions.descriptionString.isEmpty() ) )
    {
        temp = "# "+saveOptions.descriptionString;
        temp.replace( "@PACKAGE@", packageName());
        temp.replace( "@LANGUAGE@", identityOptions.languageName);
        temp = temp.trimmed();

        // The description strings has often buggy variants already in the file, these must be removed
        QString regexpstr = "^#\\s+" + QRegExp::escape( saveOptions.descriptionString.trimmed() ) + "\\s*$";
        regexpstr.replace( "@PACKAGE@", ".*" );
        regexpstr.replace( "@LANGUAGE@", ".*" );
        //kDebug() << "REGEXPSTR: " <<  regexpstr;
        QRegExp regexp ( regexpstr );

        // The buggy variants exist in English too (of a time before KBabel got a translation for the corresponding language)
        QRegExp regexpUntranslated ( "^#\\s+translation of .* to .*\\s*$" );


        kDebug () << "Temp is '" << temp << "'";

        found=false;
        bool foundTemplate=false;

        it = commentList.begin();
        while ( it != commentList.end() )
        {
            kDebug () << "testing '" << (*it) << "'";
            bool deleteItem = false;

            if ( (*it) == temp )
            {
                kDebug () << "Match ";
                if ( found )
                    deleteItem = true;
                else
                    found=true;
            }
            else if ( regexp.indexIn( *it ) >= 0 )
            {
                // We have a similar (translated) string (from another project or another language (perhaps typos)). Remove it.
                deleteItem = true;
            }
            else if ( regexpUntranslated.indexIn( *it ) >= 0 )
            {
                // We have a similar (untranslated) string (from another project or another language (perhaps typos)). Remove it.
                deleteItem = true;
            }
            else if ( (*it) == "# SOME DESCRIPTIVE TITLE." )
            {
                // We have the standard title placeholder, remove it
                deleteItem = true;
            }

            if ( deleteItem )
                it = commentList.erase( it );
            else
                ++it;
        }
        if (!found) commentList.prepend(temp);
    }
#endif
    // kDebug() << "HEADER COMMENT: " << commentList;

    /*    if ( (!usePrefs || saveOptions.updateTranslatorCopyright)
            && ( ! identityOptions->readEntry("authorName","").isEmpty() )
            && ( ! identityOptions->readEntry("Email","").isEmpty() ) ) // An email address can be used as ersatz of a name
        {*/
//                        return;
    QStringList foundAuthors;

    temp = "# ";
    //temp += identityOptions->readEntry("authorName","");
    temp += Settings::authorName();
    if (!Settings::authorEmail().isEmpty())
        temp+=(" <"+Settings::authorEmail()+'>');
    temp+=", "+QDate::currentDate().toString("yyyy")+'.';

    // ### TODO: it would be nice if the entry could start with "COPYRIGHT" and have the "(C)" symbol (both not mandatory)
    QRegExp regexpAuthorYear( "^#.*(<.+@.+>)?,\\s*([\\d]+[\\d\\-, ]*|YEAR)" );
    QRegExp regexpYearAlone( "^# , \\d{4}.?\\s*$" );
    if (commentList.isEmpty())
    {
        commentList.append(temp);
        commentList.append(QString());
    }
    else
    {
        it = commentList.begin();
        while ( it != commentList.end() )
        {
            bool deleteItem = false;
            if ( it->indexOf( "copyright", 0, Qt::CaseInsensitive ) != -1 )
            {
                // We have a line with a copyright. It should not be moved.
            }
            else if ( it->contains( QRegExp("#, *fuzzy") ) )
                deleteItem = true;
            else if ( it->contains( regexpYearAlone ) )
            {
                // We have found a year number that is preceded by a comma.
                // That is typical of KBabel 1.10 (and earlier?) when there is neither an author name nor an email
                // Remove the entry
                deleteItem = true;
            }
            else if ( it->contains( "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.") )
                deleteItem = true;
            else if ( it->contains( "# SOME DESCRIPTIVE TITLE"))
                deleteItem = true;
            else if ( it->contains( regexpAuthorYear ) ) // email address followed by year
            {
                if ( !foundAuthors.contains( (*it) ) )
                {
                    // The author line is new (and not a duplicate), so add it to the author line list
                    foundAuthors.append( (*it) );
                }
                // Delete also non-duplicated entry, as now all what is needed will be processed in foundAuthors
                deleteItem = true;
            }

            if ( deleteItem )
                it = commentList.erase( it );
            else
                ++it;
        }

        if ( !foundAuthors.isEmpty() )
        {
            found = false;
            bool foundAuthor = false;

            const QString cy = QDate::currentDate().toString("yyyy");

            ait = foundAuthors.end();
            for ( it = foundAuthors.begin() ; it!=foundAuthors.end(); ++it )
            {
                if ( it->indexOf( QRegExp(
                                      QRegExp::escape( Settings::authorName() )+".*"
                                      + QRegExp::escape( Settings::authorEmail() ) ) ) != -1 )
                {
                    foundAuthor = true;
                    if ( it->indexOf( cy ) != -1 )
                        found = true;
                    else
                        ait = it;
                }
            }
            if ( !found )
            {
                if ( !foundAuthor )
                    foundAuthors.append(temp);
                else if ( ait != foundAuthors.end() )
                {
                    //update years
                    const int index = (*ait).lastIndexOf( QRegExp("[\\d]+[\\d\\-, ]*") );
                    if ( index == -1 )
                        (*ait)+=", "+cy;
                    else
                        ait->insert(index+1, QString(", ")+cy);
                }
                else
                    kDebug() << "INTERNAL ERROR: author found but iterator dangling!";
            }

        }
        else
            foundAuthors.append(temp);


        for (ait=foundAuthors.begin();ait!=foundAuthors.end();++ait)
        {
            QString s = (*ait);

            // ensure dot at the end of copyright
            if ( !s.endsWith('.') ) s += '.';
            commentList.append(s);
        }
    }

    //m_header.setComment( commentList.join( "\n" ) );
    comment=commentList.join("\n");

//END comment = description, copyrights
}
