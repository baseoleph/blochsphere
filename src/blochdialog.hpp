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

#ifndef ANGINPUT_HPP
#define ANGINPUT_HPP

#include <QDialog>
#include <QLineEdit>

enum DIALOG_TYPE { ANGLE = 0, NORMALIZE };

class BlochDialog : public QDialog {
    Q_OBJECT
public:
    explicit BlochDialog(QWidget *parent, DIALOG_TYPE dt);

    // TODO not safe method
    QString ang() const;

private:
    QLineEdit *angEd = nullptr;
    void       angleInput();

    // TODO show before/after (xyz, ab)
    void       suggestNormalize();
};
#endif // ANGINPUT_HPP
