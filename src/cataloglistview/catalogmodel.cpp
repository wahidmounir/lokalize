/* ****************************************************************************
  This file is part of Lokalize

  Copyright (C) 2007-2014 by Nick Shaforostoff <shafff@ukr.net>

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

**************************************************************************** */

#include "catalogmodel.h"

#include "lokalize_debug.h"

#include "catalog.h"
#include "project.h"

#include <kcolorscheme.h>
#include <klocalizedstring.h>

#include <QApplication>
#include <QPalette>
#include <QFontMetrics>

#define DYNAMICFILTER_LIMIT 256

QVector<QVariant> CatalogTreeModel::m_fonts;


CatalogTreeModel::CatalogTreeModel(QObject* parent, Catalog* catalog)
    : QAbstractItemModel(parent)
    , m_catalog(catalog)
    , m_ignoreAccel(true)
{
    if (m_fonts.isEmpty()) {
        QVector<QFont> fonts(4, QApplication::font());
        fonts[1].setItalic(true); //fuzzy

        fonts[2].setBold(true);   //modified

        fonts[3].setItalic(true); //fuzzy
        fonts[3].setBold(true);   //modified

        m_fonts.reserve(4);
        for (int i = 0; i < 4; i++) m_fonts << fonts.at(i);
    }
    connect(catalog, &Catalog::signalEntryModified, this, &CatalogTreeModel::reflectChanges);
    connect(catalog, QOverload<>::of(&Catalog::signalFileLoaded), this, &CatalogTreeModel::fileLoaded);
}

QModelIndex CatalogTreeModel::index(int row, int column, const QModelIndex& /*parent*/) const
{
    return createIndex(row, column);
}

QModelIndex CatalogTreeModel::parent(const QModelIndex& /*index*/) const
{
    return QModelIndex();
}

int CatalogTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return DisplayedColumnCount;
}

void CatalogTreeModel::fileLoaded()
{
    beginResetModel();
    endResetModel();
}

void CatalogTreeModel::reflectChanges(DocPosition pos)
{
    emit dataChanged(index(pos.entry, 0), index(pos.entry, DisplayedColumnCount - 1));

#if 0
    I disabled dynamicSortFilter function
    //lazy sorting/filtering
    if (rowCount() < DYNAMICFILTER_LIMIT || m_prevChanged != pos) {
        qCWarning(LOKALIZE_LOG) << "first dataChanged emitment" << pos.entry;
        emit dataChanged(index(pos.entry, 0), index(pos.entry, DisplayedColumnCount - 1));
        if (!(rowCount() < DYNAMICFILTER_LIMIT)) {
            qCWarning(LOKALIZE_LOG) << "second dataChanged emitment" << m_prevChanged.entry;
            emit dataChanged(index(m_prevChanged.entry, 0), index(m_prevChanged.entry, DisplayedColumnCount - 1));
        }
    }
    m_prevChanged = pos;
#endif
}

int CatalogTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_catalog->numberOfEntries();
}

QVariant CatalogTreeModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case Key:       return i18nc("@title:column", "Entry");
    case Source:    return i18nc("@title:column Original text", "Source");
    case Target:    return i18nc("@title:column Text in target language", "Target");
    case Notes:     return i18nc("@title:column", "Notes");
    case Context:   return i18nc("@title:column", "Context");
    case TranslationStatus: return i18nc("@title:column", "Translation Status");
    }
    return QVariant();
}

QVariant CatalogTreeModel::data(const QModelIndex& index, int role) const
{
    if (m_catalog->numberOfEntries() <= index.row())
        return QVariant();

    if (role == Qt::SizeHintRole) {
        //no need to cache because of uniform row heights
        return QFontMetrics(QApplication::font()).size(Qt::TextSingleLine, QString::fromLatin1("          "));
    } else if (role == Qt::FontRole/* && index.column()==Target*/) {
        bool fuzzy = !m_catalog->isApproved(index.row());
        bool modified = m_catalog->isModified(index.row());
        return m_fonts.at(fuzzy * 1 | modified * 2);
    } else if (role == Qt::ForegroundRole) {
        if (m_catalog->isBookmarked(index.row())) {
            static KColorScheme colorScheme(QPalette::Normal);
            return colorScheme.foreground(KColorScheme::LinkText);
        }
        if (m_catalog->isObsolete(index.row())) {
            static KColorScheme colorScheme(QPalette::Normal);
            return colorScheme.foreground(KColorScheme::InactiveText);
        }
    } else if (role == Qt::UserRole) {
        switch (index.column()) {
        case TranslationStatus:   return m_catalog->isApproved(index.row());
        case IsEmpty:     return m_catalog->isEmpty(index.row());
        case State:     return int(m_catalog->state(index.row()));
        case IsModified:  return m_catalog->isModified(index.row());
        case IsPlural:  return m_catalog->isPlural(index.row());
        default:        role = Qt::DisplayRole;
        }
    } else if (role == StringFilterRole) { //exclude UI strings
        if (index.column() >= TranslationStatus)
            return QVariant();
        else if (index.column() == Source || index.column() == Target) {
            QString str = index.column() == Source ? m_catalog->msgidWithPlurals(index.row()) : m_catalog->msgstrWithPlurals(index.row());
            return m_ignoreAccel ? str.remove(Project::instance()->accel()) : str;
        }
        role = Qt::DisplayRole;
    }
    if (role != Qt::DisplayRole)
        return QVariant();



    switch (index.column()) {
    case Key:    return index.row() + 1;
    case Source:
        return m_catalog->msgidWithPlurals(index.row());
    case Target:
        return m_catalog->msgstrWithPlurals(index.row());
    case Notes: {
        QString result;
        foreach (const Note &note, m_catalog->notes(index.row()))
            result += note.content;
        return result;
    }
    case Context: return m_catalog->context(index.row());
    case TranslationStatus:
        static QString statuses[] = {i18nc("@info:status 'non-fuzzy' in gettext terminology", "Ready"),
                                     i18nc("@info:status 'fuzzy' in gettext terminology", "Needs review"),
                                     i18nc("@info:status", "Untranslated")
                                    };
        if (m_catalog->isEmpty(index.row()))
            return statuses[2];
        return statuses[!m_catalog->isApproved(index.row())];
    }
    return QVariant();
}

CatalogTreeFilterModel::CatalogTreeFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_filerOptions(AllStates)
    , m_individualRejectFilterEnable(false)
    , m_mergeCatalog(NULL)
{
    setFilterKeyColumn(-1);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterRole(CatalogTreeModel::StringFilterRole);
    setDynamicSortFilter(false);
}

void CatalogTreeFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &QAbstractItemModel::modelReset, this, QOverload<>::of(&CatalogTreeFilterModel::setEntriesFilteredOut));
    setEntriesFilteredOut(false);
}

void CatalogTreeFilterModel::setEntriesFilteredOut()
{
    return setEntriesFilteredOut(false);
}
void CatalogTreeFilterModel::setEntriesFilteredOut(bool filteredOut)
{
    m_individualRejectFilter.fill(filteredOut, sourceModel()->rowCount());
    m_individualRejectFilterEnable = filteredOut;
    invalidateFilter();
}

void CatalogTreeFilterModel::setEntryFilteredOut(int entry, bool filteredOut)
{
//     if (entry>=m_individualRejectFilter.size())
//         sourceModelReset();
    m_individualRejectFilter[entry] = filteredOut;
    m_individualRejectFilterEnable = true;
    invalidateFilter();
}

void CatalogTreeFilterModel::setFilerOptions(int o)
{
    m_filerOptions = o;
    setFilterCaseSensitivity(o & CaseInsensitive ? Qt::CaseInsensitive : Qt::CaseSensitive);
    static_cast<CatalogTreeModel*>(sourceModel())->setIgnoreAccel(o & IgnoreAccel);
    invalidateFilter();
}

bool CatalogTreeFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    int filerOptions = m_filerOptions;
    bool accepts = true;
    if (bool(filerOptions & Ready) != bool(filerOptions & NotReady)) {
        bool ready = sourceModel()->index(source_row, CatalogTreeModel::TranslationStatus, source_parent).data(Qt::UserRole).toBool();
        accepts = (ready == bool(filerOptions & Ready) || ready != bool(filerOptions & NotReady));
    }
    if (accepts && bool(filerOptions & NonEmpty) != bool(filerOptions & Empty)) {
        bool untr = sourceModel()->index(source_row, CatalogTreeModel::IsEmpty, source_parent).data(Qt::UserRole).toBool();
        accepts = (untr == bool(filerOptions & Empty) || untr != bool(filerOptions & NonEmpty));
    }
    if (accepts && bool(filerOptions & Modified) != bool(filerOptions & NonModified)) {
        bool modified = sourceModel()->index(source_row, CatalogTreeModel::IsModified, source_parent).data(Qt::UserRole).toBool();
        accepts = (modified == bool(filerOptions & Modified) || modified != bool(filerOptions & NonModified));
    }
    if (accepts && bool(filerOptions & Plural) != bool(filerOptions & NonPlural)) {
        bool modified = sourceModel()->index(source_row, CatalogTreeModel::IsPlural, source_parent).data(Qt::UserRole).toBool();
        accepts = (modified == bool(filerOptions & Plural) || modified != bool(filerOptions & NonPlural));
    }

    // These are the possible sync options of a row:
    // * SameInSync: The sync file contains a row with the same msgid and the same msgstr.
    // * DifferentInSync: The sync file contains a row with the same msgid and different msgstr.
    // * NotInSync: The sync file does not contain any row with the same msgid.
    //
    // The code below takes care of filtering rows when any of those options is not checked.
    //
    const int mask = (SameInSync | DifferentInSync | NotInSync);
    if (accepts && m_mergeCatalog && (filerOptions & mask) && (filerOptions & mask) != mask) {
        bool isPresent = m_mergeCatalog->isPresent(source_row);
        bool isDifferent = m_mergeCatalog->isDifferent(source_row);

        accepts = !
                  ((isPresent && !isDifferent && !bool(filerOptions & SameInSync))      ||
                   (isPresent &&  isDifferent && !bool(filerOptions & DifferentInSync)) ||
                   (!isPresent &&                 !bool(filerOptions & NotInSync))
                  );
    }

    if (accepts && (filerOptions & STATES) != STATES) {
        int state = sourceModel()->index(source_row, CatalogTreeModel::State, source_parent).data(Qt::UserRole).toInt();
        accepts = (filerOptions & (1 << (state + FIRSTSTATEPOSITION)));
    }

    accepts = accepts && !(m_individualRejectFilterEnable && source_row < m_individualRejectFilter.size() && m_individualRejectFilter.at(source_row));

    return accepts && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void CatalogTreeFilterModel::setMergeCatalogPointer(MergeCatalog* pointer)
{
    m_mergeCatalog = pointer;
}
