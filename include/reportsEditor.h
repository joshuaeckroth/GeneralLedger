/* General Ledger, Copyright (C) 2004  Joshua Eckroth <josh@eckroth.net>
 * http://www.eckroth.net/software/genledg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/

#ifndef REPORTS_EDITOR_H
#define REPORTS_EDITOR_H

#include <vector>

#include <qobject.h>
#include <qwidget.h>

class QString;
class QIODevice;

class ReportsEditor : public QObject
{
    Q_OBJECT
    public:
        struct Entity {
            Q_UINT8 editor;
            Q_UINT8 section;
            Q_UINT16 index;
            QString accountBegin;
            QString accountEnd;
            QString desc;
        };

        /// The different editor groups
        enum Editor { BalanceSheet, IncomeSvc, IncomeRetail };

        /// This covers all the types for the three editor groups
        enum Section { Assets, Liabilities, Equities,
                Income, CostOfSales, Expenses };

        static ReportsEditor* instance();

        void destroy();
        
        void initiate();

        void readStream(QIODevice *device, Q_UINT16 editorEntityCount);

        void writeStream(QIODevice *device);

        void clearData();

        Q_UINT16 getEditorEntityCount() const;

        QWidget* createEditor(QWidget *parent, const char *name, Editor editor);

        std::vector<Entity*> getEntities(Editor editor, Section section);

        bool entityExists(Editor editor, Section section,
                        QString accountBegin, QString accountEnd, QString desc);

        Q_UINT16 getFirstIndex(Editor editor, Section section);

        Q_UINT16 getLastIndex(Editor editor, Section section);

        Q_UINT16 getNextIndex(Editor editor, Section section);

        void moveEntityUp(Entity *entity);
        void moveEntityDown(Entity *entity);
        void addEntity(Entity *entity);
        void removeEntity(Entity *entity);

    signals:
        void saveToDisk();

    private:
        ReportsEditor();
        ~ReportsEditor();

        static ReportsEditor *thisInstance;

        std::vector<Entity*> editorEntities;
};

#endif

