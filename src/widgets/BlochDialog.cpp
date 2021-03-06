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

#include "BlochDialog.h"
#include "MainWindow.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QString>
#include <QTime>

BlochDialog::BlochDialog(QWidget *parent, DIALOG_TYPE dt)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
    if (dt == DIALOG_TYPE::ANGLE) {
        angleInput();
    } else {
        suggestNormalize();
    }
    setFixedWidth(200);
}

QString BlochDialog::ang() const { return angEd->text(); }

void BlochDialog::angleInput() {
    angEd = new QLineEdit(this);
    auto angLab = new QLabel("Enter the angle in degrees:", this);

    angLab->setBuddy(angEd);
    angEd->setValidator(new QDoubleValidator(this));

    auto bOk = new QPushButton("Ok", this);
    auto bCl = new QPushButton("Cancel", this);
    connect(bOk, SIGNAL(clicked()), SLOT(accept()));
    connect(bCl, SIGNAL(clicked()), SLOT(reject()));

    auto lay = new QVBoxLayout(this);
    lay->addWidget(angLab);
    lay->addWidget(angEd);

    auto lay1 = new QHBoxLayout(lay->widget());
    lay1->addWidget(bOk);
    lay1->addWidget(bCl);

    lay->addLayout(lay1);
    setLayout(lay);
}

void BlochDialog::suggestNormalize() {
    auto angLab = new QLabel("Vector is not unit. \nDo you want to normalize it?", this);

    auto bOk = new QPushButton("Ok", this);
    auto bCl = new QPushButton("Cancel", this);
    connect(bOk, SIGNAL(clicked()), SLOT(accept()));
    connect(bCl, SIGNAL(clicked()), SLOT(reject()));

    auto lay = new QVBoxLayout(this);
    lay->addWidget(angLab);

    auto lay1 = new QHBoxLayout(lay->widget());
    lay1->addWidget(bOk);
    lay1->addWidget(bCl);

    lay->addLayout(lay1);
    setLayout(lay);
}
