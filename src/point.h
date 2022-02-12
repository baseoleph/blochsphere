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

#ifndef POINT_H
#define POINT_H

class Point {
public:
    Point();
    Point(double x, double y, double z);
    Point(double the, double phi);

    inline double x() const { return x_; }
    inline double y() const { return y_; }
    inline double z() const { return z_; }
    inline double the() const { return the_; }
    inline double phi() const { return phi_; }

protected:
    void changePoint(double x, double y, double z);
    void changePoint(double the, double phi);

private:
    double x_;
    double y_;
    double z_;
    double the_; // radians
    double phi_; // radians

    void evalPT();
    void evalXYZ();
};

#endif // POINT_H
