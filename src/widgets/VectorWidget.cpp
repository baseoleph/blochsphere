// A Bloch sphere emulator program.
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
#include "BlochDialog.h"
#include "src/quantum/UnitaryMatrix2x2.h"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

VectorWidget::VectorWidget(QWidget *parent, Vector *v) : QWidget(parent), _v(v) {
    topLay = new QGridLayout(this);
    topLay->addWidget(makeThePhiWid(), 0, 0);
    topLay->addWidget(makeAlpBetWid(), 0, 1);
    topLay->addWidget(makeBloVecWid(), 0, 2);
    topLay->setMargin(0);
    topLay->setSpacing(0);
    this->setLayout(topLay);
}

QWidget *VectorWidget::makeThePhiWid() {
    auto tpW = new QWidget(this);

    theEd = new QLineEdit("0", tpW);
    phiEd = new QLineEdit("0", tpW);
    auto theLab = new QLabel("<font face=symbol size=5>q</font>", tpW);
    auto phiLab = new QLabel("<font face=symbol size=5>f</font>", tpW);

    theEd->setMaximumWidth(55);
    theEd->setValidator(new QDoubleValidator(theEd));

    phiEd->setMaximumWidth(55);
    phiEd->setValidator(new QDoubleValidator(theEd));

    auto thePhiButton = new QPushButton("Set", tpW);
    thePhiButton->setFixedWidth(55);
    connect(thePhiButton, SIGNAL(clicked()), SLOT(slotThePhi()));

    tpW->setFixedSize(110, 90);

    auto tpLay = new QGridLayout(tpW);
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
    auto abW = new QWidget(this);
    alpEd = new QLineEdit("1", abW);
    betEd = new QLineEdit("0", abW);
    auto alpLab = new QLabel("<font face=symbol size=5>a</font>", abW);
    auto betLab = new QLabel("<font face=symbol size=5>b</font>", abW);

    alpEd->setFixedWidth(90);
    alpEd->setValidator(new QDoubleValidator(alpEd));

    betEd->setFixedWidth(90);
    betEd->setValidator(Utility::compValid());

    connect(betEd, SIGNAL(textEdited(const QString &)),
            SLOT(updateComplexLineEdit(const QString &)));

    auto bPsi = new QPushButton("Set", abW);
    bPsi->setFixedWidth(60);
    connect(bPsi, SIGNAL(clicked()), SLOT(slotAlpBet()));

    auto bRandPsi = new QPushButton("Random", abW);
    bRandPsi->setFixedWidth(60);
    connect(bRandPsi, SIGNAL(clicked()), SLOT(slotSetRandomPsi()));

    abW->setFixedSize(150, 90);

    auto abLay = new QGridLayout(abW);
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
    auto xyzW = new QWidget(this);
    xEd = new QLineEdit("0", xyzW);
    yEd = new QLineEdit("0", xyzW);
    zEd = new QLineEdit("1", xyzW);

    auto xLab = new QLabel("<font size=4>x</font>", xyzW);
    auto yLab = new QLabel("<font size=4>y</font>", xyzW);
    auto zLab = new QLabel("<font size=4>z</font>", xyzW);

    xEd->setFixedWidth(90);
    xEd->setValidator(new QDoubleValidator(xEd));
    yEd->setFixedWidth(90);
    yEd->setValidator(new QDoubleValidator(yEd));
    zEd->setFixedWidth(90);
    zEd->setValidator(new QDoubleValidator(zEd));

    auto bXyz = new QPushButton("Set", xyzW);
    bXyz->setFixedWidth(60);
    connect(bXyz, SIGNAL(clicked()), SLOT(slotBloVec()));

    xyzW->setFixedSize(150, 90);

    auto abLay = new QGridLayout(xyzW);
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

void VectorWidget::slotThePhi() {
    emptyToZeroLineEdit(theEd);
    emptyToZeroLineEdit(phiEd);

    double the = qDegreesToRadians(theEd->text().toDouble());
    double phi = qDegreesToRadians(phiEd->text().toDouble());
    Spike  sp = Vector::createSpike(the, phi);
    _v->changeVector(sp);

    emit signalUpdate();
    fillFieldsOfVector(sp, FIELD::THEPHI);
}

void VectorWidget::slotAlpBet() {
    emptyToZeroLineEdit(alpEd);
    emptyToZeroLineEdit(betEd);

    double  a = alpEd->text().toDouble();
    complex b = Utility::parseStrToComplex(betEd->text());
    if (std::abs(a) + std::abs(b) < EPSILON) {
        QMessageBox::warning(this, "Warning", "Vector must be non-zero");
        return;
    }
    double len = sqrt(a * a + b.real() * b.real() + b.imag() * b.imag());
    if (not Utility::fuzzyCompare(len, 1.)) {
        auto dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
        if (isAutoNormalize or dial->exec() == QDialog::Accepted) {
            a /= len;
            b = complex(b.real() / len, b.imag() / len);
            alpEd->setText(QString::number(a));
            betEd->setText(Utility::parseComplexToStr(b));
        } else {
            return;
        }
    }
    Spike sp = Vector::createSpike(a, b);
    _v->changeVector(sp);

    emit signalUpdate();
    fillFieldsOfVector(sp, FIELD::ALPBET);
}

void VectorWidget::slotBloVec() {
    emptyToZeroLineEdit(xEd);
    emptyToZeroLineEdit(yEd);
    emptyToZeroLineEdit(zEd);

    double x = xEd->text().toDouble();
    double y = yEd->text().toDouble();
    double z = zEd->text().toDouble();
    if (std::abs(x) + std::abs(y) + std::abs(z) < EPSILON) {
        QMessageBox::warning(this, "Warning", "Vector must be non-zero");
        return;
    }
    double len = sqrt(x * x + y * y + z * z);
    if (not Utility::fuzzyCompare(len, 1.)) {
        auto dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
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

    emit signalUpdate();
    fillFieldsOfVector(sp, FIELD::BLOVEC);
}

void VectorWidget::slotSetRandomPsi() {
    double the = Utility::random(0, 180);
    double phi = Utility::random(0, 360);

    Spike sp = Vector::createSpike(the, phi);
    _v->changeVector(sp);

    emit signalUpdate();
    fillFieldsOfVector(sp);
}

void VectorWidget::updateComplexLineEdit(const QString &) {
    Utility::updateComplexLineEdit(betEd);
}

void VectorWidget::fillFieldsOfVector(Spike sp, FIELD exclude) {
    Vector v;
    v.changeVector(sp);

    if (exclude != FIELD::THEPHI) {
        theEd->setText(Utility::numberToStr(qRadiansToDegrees(v.the())));
        phiEd->setText(Utility::numberToStr(qRadiansToDegrees(v.phi())));
    }

    if (exclude != FIELD::ALPBET) {
        alpEd->setText(Utility::numberToStr(v.a().real()));
        betEd->setText(Utility::parseComplexToStr(v.b()));
    }

    if (exclude != FIELD::BLOVEC) {
        xEd->setText(Utility::numberToStr(v.x()));
        yEd->setText(Utility::numberToStr(v.y()));
        zEd->setText(Utility::numberToStr(v.z()));
    }
}

void VectorWidget::emptyToZeroLineEdit(QLineEdit *lineEdit) {
    if (lineEdit->text() == "") {
        lineEdit->setText("0");
    }
}
