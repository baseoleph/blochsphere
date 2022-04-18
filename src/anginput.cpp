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

#include "anginput.hpp"
#include "mainwindow.hpp"
#include <QGridLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QString>
#include <QTime>

AngInput::AngInput(QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
    angEd = new QLineEdit;
    auto *angLab = new QLabel("Enter the angle in degrees:");

    angLab->setBuddy(angEd);
    angEd->setValidator(new QDoubleValidator);

    auto *bOk = new QPushButton("Ok");
    auto *bCl = new QPushButton("Cancel");
    connect(bOk, SIGNAL(clicked()), SLOT(accept()));
    connect(bCl, SIGNAL(clicked()), SLOT(reject()));

    auto *lay = new QVBoxLayout;
    lay->addWidget(angLab);
    lay->addWidget(angEd);

    auto *lay1 = new QHBoxLayout;
    lay1->addWidget(bOk);
    lay1->addWidget(bCl);

    lay->addLayout(lay1);
    setLayout(lay);
    setWindowTitle(((QPushButton *)this->parent())->text());
    setFixedWidth(155);
}

QString AngInput::ang() const { return angEd->text(); }