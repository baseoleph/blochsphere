// The program helps to see a geometric representation of qubits
// Copyright (C) 2022 Vasiliy Stephanov <baseoleph@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VECTORTABWIDGET_H
#define VECTORTABWIDGET_H

#include "src/quantum/Vector.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QTabWidget>

enum FIELD { NOTHIN = 0, THEPHI, ALPBET, BLOVEC };

class VectorWidget : public QWidget {
    Q_OBJECT
public:
    VectorWidget(QWidget *parent, Vector *v);

    void    setAutoNormalise(bool f) { isAutoNormalize = f; }
    void    fillFieldsOfVector(Spike sp, FIELD exclude = FIELD::NOTHIN);
    Vector *getVector() { return _v; }

signals:
    void signalUpdate();

public slots:
    void slotThePhi();
    void slotAlpBet();
    void slotBloVec();
    void slotSetRandomPsi();
    void updateComplexLineEdit(const QString &);

private:
    Vector *_v = nullptr;
    bool    isAutoNormalize = true;

    QGridLayout *topLay = nullptr;
    QLineEdit   *theEd = nullptr;
    QLineEdit   *phiEd = nullptr;

    QLineEdit *alpEd = nullptr;
    QLineEdit *betEd = nullptr;

    QLineEdit *xEd = nullptr;
    QLineEdit *yEd = nullptr;
    QLineEdit *zEd = nullptr;

    QWidget *makeThePhiWid();
    QWidget *makeAlpBetWid();
    QWidget *makeBloVecWid();

    void emptyToZeroLineEdit(QLineEdit *lineEdit);
};

#endif // VECTORTABWIDGET_H
