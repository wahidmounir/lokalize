/* ****************************************************************************
  This file is based on the one from KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002	  by Stanislav Visnovsky <visnovsky@nenya.ms.mff.cuni.cz>
  Copyright (C) 2006      by Nicolas GOUTTE <goutte@kde.org> 
                2007      by Nick Shaforostoff <shafff@ukr.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */

#include "catalogitem.h"
#include "catalogitem_private.h"

#include <kdebug.h>


CatalogItem::CatalogItem()
 : d(new CatalogItemPrivate())
{
}

CatalogItem::CatalogItem(const CatalogItem& item)
 : d(new CatalogItemPrivate())
{
    *d=*(item.d);
}

CatalogItem::~CatalogItem()
{
    delete d;
}



const QString& CatalogItem::comment() const
{
    return d->_comment;
}

const QString& CatalogItem::msgctxt(const bool noNewlines) const
{
    if (noNewlines)
        return (d->_msgctxt).replace('\n', ' '); //" " or "" ?
    else
        return d->_msgctxt;
}

const QString& CatalogItem::msgid(const int form, const bool /*noNewlines*/) const
{
    return d->msgid(form);
}

const QVector<QString>& CatalogItem::msgidPlural(const bool /*noNewlines*/) const
{
    return d->_msgidPlural;
}

const QString& CatalogItem::msgstr(const int form, const bool /*noNewlines*/) const
{
    if (KDE_ISLIKELY (form<d->_msgstrPlural.size()))
        return d->_msgstrPlural.at(form);
    else
        return d->_msgstrPlural.last();
}

const QVector<QString>& CatalogItem::msgstrPlural(const bool /*noNewlines*/) const
{
    return d->_msgstrPlural;
}

bool CatalogItem::isValid() const
{
    return d->_valid;
}

void CatalogItem::setValid(bool a)
{
    d->_valid=a;
}

void CatalogItem::setMsgctxt(const QString& msg)
{
    d->_msgctxt=msg;
    d->_msgctxt.squeeze();
}

void CatalogItem::setMsgid(const QString& msg, const int form)
{
    if (d->_msgidPlural.size()>=form)
        d->_msgidPlural[form]=msg;
    else
        d->_msgidPlural.append(msg);
//      d->_msgid=msg;
//      d->_msgid.squeeze();
}

void CatalogItem::setMsgidPlural(const QStringList& msg)
{
    d->_msgidPlural=msg.toVector(); //TODO
    for (QVector<QString>::iterator it=d->_msgidPlural.begin();it!=d->_msgidPlural.end();++it)
        it->squeeze();
}

void CatalogItem::setMsgid(const QStringList& msg)
{
    d->_msgidPlural=msg.toVector(); //TODO
    for (QVector<QString>::iterator it=d->_msgidPlural.begin();it!=d->_msgidPlural.end();++it)
        it->squeeze();
}

void CatalogItem::setMsgid(const QVector<QString>& msg)
{
    d->_msgidPlural=msg;
    for (QVector<QString>::iterator it=d->_msgidPlural.begin();it!=d->_msgidPlural.end();++it)
        it->squeeze();
}

void CatalogItem::setMsgstr(const QString& msg, const int form)
{
//     d->_msgstr=msg;
    d->_msgstrPlural[form]=msg;
}

void CatalogItem::setMsgstrPlural(const QStringList& msg)
{
    //TODO
    d->_msgstrPlural=msg.toVector();
}

void CatalogItem::setMsgstr(const QStringList& msg)
{
    //TODO
    d->_msgstrPlural=msg.toVector();
}

void CatalogItem::setMsgstr(const QVector<QString>& msg)
{
    d->_msgstrPlural=msg;
}

void CatalogItem::setComment(const QString& com)
{
    d->_comment=com;
    d->_comment.squeeze();
}

void CatalogItem::setPluralFormType( PluralFormType type )
{
    d->_pluralFormType = type;
}

PluralFormType CatalogItem::pluralFormType() const
{
    return d->_pluralFormType;
}

bool CatalogItem::isPlural() const
{
    return d->_pluralFormType==Gettext;
}

bool CatalogItem::isFuzzy() const
{
    return d->_comment.contains( QRegExp(",\\s*fuzzy") );
}

bool CatalogItem::isUntranslated() const
{
    return d->isUntranslated();
}

bool CatalogItem::isUntranslated(uint form) const
{
    return d->isUntranslated(form);
}

#if 0
QStringList CatalogItem::errors() const
{
    return d->_errors;
}

bool CatalogItem::isCformat() const
{
    // Allow "possible-c-format" (from xgettext --debug) or "c-format"
    // Note the regexp (?: ) is similar to () but it does not capture (so it is faster)
    return d->_comment.indexOf( QRegExp(",\\s*(?:possible-)c-format") ) == -1;
}

bool CatalogItem::isNoCformat() const
{
    return d->_comment.indexOf( QRegExp(",\\s*no-c-format") ) == -1;
}

bool CatalogItem::isQtformat() const
{
    return d->_comment.indexOf( QRegExp(",\\s*qt-format") ) == -1;
}

bool CatalogItem::isNoQtformat() const
{
    return d->_comment.indexOf( QRegExp(",\\s*no-qt-format") ) == -1;
}

bool CatalogItem::isUntranslated() const
{
   return d->_msgstr.first().isEmpty();
}

int CatalogItem::totalLines() const
{
   int lines=0;
   if(!d->_comment.isEmpty())
   {
      lines = d->_comment.count('\n')+1;
   }
   int msgctxtLines=0;
   if(!d->_msgctxt.isEmpty())
   {
      msgctxtLines=d->_msgctxt.count('\n')+1;
   }
   int msgidLines=0;
   QStringList::ConstIterator it;
   for(it=d->_msgid.begin(); it != d->_msgid.end(); ++it)
   {
      msgidLines += (*it).count('\n')+1;
   }
   int msgstrLines=0;
   for(it=d->_msgstr.begin(); it != d->_msgstr.end(); ++it)
   {
      msgstrLines += (*it).count('\n')+1;
   }

   if(msgctxtLines>1)
      msgctxtLines++;
   if(msgidLines>1)
      msgidLines++;
   if(msgstrLines>1)
      msgstrLines++;

   lines+=( msgctxtLines+msgidLines+msgstrLines );

   return lines;
}


void CatalogItem::setSyntaxError(bool on)
{
	if(on && !d->_errors.contains("syntax error"))
		d->_errors.append("syntax error");
	else
		d->_errors.removeAll("syntax error");
}

#endif

void CatalogItem::clear()
{
    d->clear();
}

void CatalogItem::operator=(const CatalogItem& rhs)
{
    d->assign(*rhs.d);
}


QStringList CatalogItem::msgstrAsList() const
{
    if (d->_msgstrPlural.isEmpty())
    {
        kWarning()<<"This should never happen!";
        return QStringList();
    }
    QStringList list(d->_msgstrPlural.first().split('\n', QString::SkipEmptyParts ));

    if(d->_msgstrPlural.first()=="\n")
        list.prepend(QString());

    if(list.isEmpty())
        list.append(QString());

    return list;
}



void CatalogItem::setFuzzy()
{
    QString& comment=d->_comment;
    if (comment.isEmpty())
    {
        comment="#, fuzzy";
        return;
    }

    int p=comment.indexOf("#,");
    if(p!=-1)
    {
        comment.replace(p,2,"#, fuzzy,");
        return;
    }

    QRegExp a("\\#\\:[^\n]*\n");
    p=a.indexIn(comment);
    if (p!=-1)
    {
        comment.insert(p+a.matchedLength(),"#, fuzzy\n");
        return;
    }

    {
        if( !(comment.endsWith('\n')) )
            comment+='\n';
        comment+="#, fuzzy";
    }
}

void CatalogItem::unsetFuzzy()
{
    QString& comment=d->_comment;

    comment.remove( QRegExp(",\\s*fuzzy"));

    // remove empty comment lines
    comment.remove( QRegExp("\n#\\s*$") );
    comment.remove( QRegExp("^#\\s*$") );
    comment.remove( QRegExp("#\\s*\n") );
    comment.remove( QRegExp("^#\\s*\n") );

}




#if 0
QString CatalogItem::nextError() const
{
    return d->_errors.first();
}

void CatalogItem::clearErrors()
{
    d->_errors.clear();
}

void CatalogItem::appendError(const QString& error )
{
    if( !d->_errors.contains( error ) )
	d->_errors.append(error);
}

void CatalogItem::removeError(const QString& error )
{
    d->_errors.removeAt( d->_errors.indexOf( error ) );
}
#endif

// kate: space-indent on; indent-width 4; replace-tabs on;