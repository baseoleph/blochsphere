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

#include "VectorWidget.h"
#include "blochdialog.hpp"
#include "unitaryMatrix2x2.hpp"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

// TODO duplicate
QRegExpValidator
    compValid2(QRegExp(QString::fromUtf8("^[+-]?[0-9]*\\.?[0-9]*[+-]?[0-9]*\\.?[0-9]*[iIшШ]?$")));

VectorWidget::VectorWidget(QWidget *parent, Vector *v) : QWidget(parent), _v(v) {
    auto topLay = new QGridLayout();
    topLay->addWidget(makeThePhiWid(), 0, 0);
    topLay->addWidget(makeAlpBetWid(), 0, 1);
    topLay->addWidget(makeBloVecWid(), 0, 2);
    topLay->setMargin(0);
    topLay->setSpacing(0);
    this->setLayout(topLay);
}

QWidget *VectorWidget::makeThePhiWid() {
    theEd = new QLineEdit("0");
    phiEd = new QLineEdit("0");
    auto *theLab = new QLabel("<font face=symbol size=5>q</font>");
    auto *phiLab = new QLabel("<font face=symbol size=5>f</font>");

    theEd->setMaximumWidth(55);
    theEd->setValidator(new QDoubleValidator);

    phiEd->setMaximumWidth(55);
    phiEd->setValidator(new QDoubleValidator);

    auto *thePhiButton = new QPushButton("Set");
    thePhiButton->setFixedWidth(55);
    connect(thePhiButton, SIGNAL(clicked()), SLOT(slotThePhi()));

    auto *tpW = new QWidget();
    tpW->setFixedHeight(90);

    auto *qfThePhi = new QFrame(tpW);
    qfThePhi->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfThePhi->move(0, 0);
    qfThePhi->setFixedSize(tpW->size());

    auto *tpLay = new QGridLayout();
    tpLay->addWidget(theLab, 0, 0);
    tpLay->addWidget(theEd, 0, 1, 1, 3);
    tpLay->addWidget(phiLab, 1, 0);
    tpLay->addWidget(phiEd, 1, 1, 1, 3);
    tpLay->addWidget(thePhiButton, 4, 1, 1, 2);
    tpLay->setSpacing(2);
    tpLay->setContentsMargins(5, 5, 5, 5);
    tpW->setLayout(tpLay);

    return tpW;
}

QWidget *VectorWidget::makeAlpBetWid() {
    alpEd = new QLineEdit("1");
    betEd = new QLineEdit("0");
    auto *alpLab = new QLabel("<font face=symbol size=5>a</font>");
    auto *betLab = new QLabel("<font face=symbol size=5>b</font>");

    alpEd->setFixedWidth(90);
    alpEd->setValidator(new QDoubleValidator);

    betEd->setFixedWidth(90);
    betEd->setValidator(&compValid2);

    connect(betEd, SIGNAL(textEdited(const QString &)),
            SLOT(updateComplexLineEdit(const QString &)));

    auto *bPsi = new QPushButton("Set");
    bPsi->setFixedWidth(60);
    connect(bPsi, SIGNAL(clicked()), SLOT(slotAlpBet()));

    auto *bRandPsi = new QPushButton("Random");
    bRandPsi->setFixedWidth(60);
    connect(bRandPsi, SIGNAL(clicked()), SLOT(slotSetRandomPsi()));

    auto *abW = new QWidget();
    abW->setFixedHeight(90);

    auto *qfAlpBet = new QFrame(abW);
    qfAlpBet->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfAlpBet->move(0, 0);
    qfAlpBet->setFixedSize(abW->size());

    auto *abLay = new QGridLayout();
    abLay->addWidget(alpLab, 1, 0);
    abLay->addWidget(alpEd, 1, 1, 1, 3);
    abLay->addWidget(betLab, 2, 0);
    abLay->addWidget(betEd, 2, 1, 1, 3);
    abLay->addWidget(bRandPsi, 4, 0, 1, 2);
    abLay->addWidget(bPsi, 4, 2);
    abLay->setContentsMargins(10, 5, 5, 5);
    abLay->setSpacing(2);
    abW->setLayout(abLay);

    return abW;
}

QWidget *VectorWidget::makeBloVecWid() {
    xEd = new QLineEdit("0");
    yEd = new QLineEdit("0");
    zEd = new QLineEdit("1");

    auto *xLab = new QLabel("<font size=4>x</font>");
    auto *yLab = new QLabel("<font size=4>y</font>");
    auto *zLab = new QLabel("<font size=4>z</font>");

    xEd->setFixedWidth(90);
    xEd->setValidator(new QDoubleValidator);
    yEd->setFixedWidth(90);
    yEd->setValidator(new QDoubleValidator);
    zEd->setFixedWidth(90);
    zEd->setValidator(new QDoubleValidator);

    auto *bXyz = new QPushButton("Set");
    bXyz->setFixedWidth(60);
    connect(bXyz, SIGNAL(clicked()), SLOT(slotBloVec()));

    auto *xyzW = new QWidget();
    xyzW->setFixedHeight(90);

    auto *qfAlpBet = new QFrame(xyzW);
    qfAlpBet->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfAlpBet->move(0, 0);
    qfAlpBet->setFixedSize(xyzW->size());

    auto *abLay = new QGridLayout();
    abLay->addWidget(xLab, 1, 0);
    abLay->addWidget(xEd, 1, 1, 1, 3);
    abLay->addWidget(yLab, 2, 0);
    abLay->addWidget(yEd, 2, 1, 1, 3);
    abLay->addWidget(zLab, 3, 0);
    abLay->addWidget(zEd, 3, 1, 1, 3);
    abLay->addWidget(bXyz, 4, 2);

    abLay->setContentsMargins(10, 5, 5, 5);
    abLay->setSpacing(2);
    xyzW->setLayout(abLay);

    return xyzW;
}

// TODO add check ranges
// DOTO push while animating
void VectorWidget::slotThePhi() {
    emptyToZeroLineEdit(theEd);
    emptyToZeroLineEdit(phiEd);

    double the = qDegreesToRadians(theEd->text().toDouble());
    double phi = qDegreesToRadians(phiEd->text().toDouble());
    Spike  sp = Vector::createSpike(the, phi);
    _v->changeVector(sp);

    fillFieldsOfVector(sp, FIELD::THEPHI);
}

// TODO add check if normalized
// DOTO push while animating
void VectorWidget::slotAlpBet() {
    emptyToZeroLineEdit(alpEd);
    emptyToZeroLineEdit(betEd);

    double  a = alpEd->text().toDouble();
    complex b = parseStrToComplex(betEd->text());
    if (abs(a) + abs(b) < EPSILON) {
        QMessageBox::warning(this, "Warning", "Vector must be non-zero");
        return;
    }
    double len = sqrt(a * a + b.real() * b.real() + b.imag() * b.imag());
    if (not UnitaryMatrix2x2::fuzzyCompare(len, 1.)) {
        auto *dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
        if (isAutoNormalize or dial->exec() == QDialog::Accepted) {
            a /= len;
            b = complex(b.real() / len, b.imag() / len);
            alpEd->setText(QString::number(a));
            betEd->setText(parseComplexToStr(b));
        } else {
            return;
        }
    }
    Spike sp = Vector::createSpike(a, b);
    _v->changeVector(sp);

    fillFieldsOfVector(sp, FIELD::ALPBET);
}

void VectorWidget::slotBloVec() {
    emptyToZeroLineEdit(xEd);
    emptyToZeroLineEdit(yEd);
    emptyToZeroLineEdit(zEd);

    double x = xEd->text().toDouble();
    double y = yEd->text().toDouble();
    double z = zEd->text().toDouble();
    if (abs(x) + abs(y) + abs(z) < EPSILON) {
        QMessageBox::warning(this, "Warning", "Vector must be non-zero");
        return;
    }
    double len = sqrt(x * x + y * y + z * z);
    if (not UnitaryMatrix2x2::fuzzyCompare(len, 1.)) {
        // TODO  (QWidget *)sender() ? qt4 fails with "this"
        auto *dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
        if (isAutoNormalize or dial->exec() == QDialog::Accepted) {
            x /= len;
            y /= len;
            z /= len;
            xEd->setText(QString::number(x));
            yEd->setText(QString::number(y));
            zEd->setText(QString::number(z));
        } else {
            return;
        }
    }
    Spike sp = Vector::createSpike(x, y, z);
    _v->changeVector(sp);

    fillFieldsOfVector(sp, FIELD::BLOVEC);
}

void VectorWidget::slotSetRandomPsi() {
    double the = random(0, 180);
    double phi = random(0, 360);

    Spike sp = Vector::createSpike(the, phi);
    _v->changeVector(sp);

    fillFieldsOfVector(sp);
}

void VectorWidget::updateComplexLineEdit(const QString &) {
    QRegExp re(QString::fromUtf8("[IШш]"));
    betEd->setText(betEd->text().replace(re, "i"));
}

void VectorWidget::fillFieldsOfVector(Spike sp, FIELD exclude) {
    Vector v;
    v.changeVector(sp);

    if (exclude != FIELD::THEPHI) {
        theEd->setText(numberToStr(qRadiansToDegrees(v.the())));
        phiEd->setText(numberToStr(qRadiansToDegrees(v.phi())));
    }

    if (exclude != FIELD::ALPBET) {
        // TODO maybe should create function that converts double to str
        alpEd->setText(numberToStr(v.a().real()));
        betEd->setText(numberToStr(v.b().real()) + (v.b().imag() >= 0 ? "+" : "") +
                       numberToStr(v.b().imag()) + "i");
    }

    if (exclude != FIELD::BLOVEC) {
        xEd->setText(numberToStr(v.x()));
        yEd->setText(numberToStr(v.y()));
        zEd->setText(numberToStr(v.z()));
    }
}

void VectorWidget::emptyToZeroLineEdit(QLineEdit *lineEdit) {
    if (lineEdit->text() == "") {
        lineEdit->setText("0");
    }
}
