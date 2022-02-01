#include "qubitvect.hpp"

// -------------------------------------
//         Реализация QBPoint
// -------------------------------------

QBPoint::QBPoint(double phi_, double the_)
    // Конструктор по полярным углам
    // -----------------------------
    : the(the_), phi(phi_) {
    evalXYZ();
}

QBPoint::QBPoint(double x_, double y_, double z_)
    // Конструктор по прямоугольным координатам
    // ----------------------------------------
    : x(x_), y(y_), z(z_) {
    evalPT();
}

void QBPoint::set(double phi_, double the_) {
    phi = phi_, the = the_;
    evalXYZ();
}

void QBPoint::evalXYZ()
// Вычисление прямоугольных координат
// ----------------------------------
{
    x = cos(phi) * sin(the);
    y = sin(phi) * sin(the);
    z = cos(the);
}

void QBPoint::evalPT()
// Вычисление полярных углов
// -------------------------
{
    the = acos(z / sqrt(x * x + y * y + z * z));
    phi = atan2(y, x);
}

void QBPoint::rotateX(double ng)
// Вращение вокруг OX на угол ng
// -----------------------------
{
    double vYo = y, vZo = z;
    y = vYo * cos(ng) - vZo * sin(ng);
    z = vYo * sin(ng) + vZo * cos(ng);
    evalPT();
}

void QBPoint::rotateY(double ng)
// Вращение вокруг OY на угол ng
// -----------------------------
{
    double vXo = x, vZo = z;
    x = vXo * cos(ng) + vZo * sin(ng);
    z = -vXo * sin(ng) + vZo * cos(ng);
    evalPT();
}

void QBPoint::rotateZ(double ng)
// Вращение вокруг OZ на угол ng
// -----------------------------
{
    double vXo = x, vYo = y;
    x = vXo * cos(ng) - vYo * sin(ng);
    y = vXo * sin(ng) + vYo * cos(ng);
    evalPT();
}

// -------------------------------------
//           Реализация Qubit
// -------------------------------------

Qubit::Qubit(double a_, comp_d b_)
    // Конструктор
    // -----------
    : a(a_), b(b_) {
    evalVertex();
};

void Qubit::setPsi(double a_, comp_d b_)
// Установить значения а и b
// -------------------------
{
    a = a_, b = b_;
    evalVertex();
}

void Qubit::set(double p_, double t_)
// Установить значения фи и тета
// -----------------------------
{
    QBPoint::set(p_, t_);
    evalAB();
}

void Qubit::randomize()
// Задать слуайно
// Автор: Швецкий М.В.
// -------------------
{
    a = (double)rand() / RAND_MAX;
    // -----------------------
    double b1 = sqrt(1 - a * a),               // Модуль b
        b2 = M_PI * (double)rand() / RAND_MAX; // Аргумент b
    b = comp_d(b1 * (cos(b2) + C_I * sin(b2)));
    evalVertex();
}

void Qubit::evalVertex()
// Вычислить вершину
// Автор: Швецкий М.В.
// -------------------
{
    if (abs(b) < EPS) {
        the = 2.0 * (M_PI / 2.0 - atan(a / abs(b)));
        if (!the || the == M_PI)
            phi = 0.0;
        else
            phi = -arg(conj(b));
    } else {
        the = 2.0 * atan(abs(b) / a);
        if (!the || the == M_PI)
            phi = 0.0;
        else
            phi = arg(b);
    }
    evalXYZ();
}

void Qubit::evalAB()
// Вычислить альфа и бета
// ----------------------
{
    comp_d csin(sin(the / 2.0), 0.0);
    a = cos(the / 2.0);
    b = pow(C_E, C_I * phi) * csin;
}

void Qubit::rotateX(double ng)
// Поворот вокруг ОХ
// -----------------
{
    QBPoint::rotateX(ng);
    evalAB();
}

void Qubit::rotateY(double ng)
// Поворот вокруг ОУ
// -----------------
{
    QBPoint::rotateY(ng);
    evalAB();
}
void Qubit::rotateZ(double ng)
// Поворот вокруг OZ
// -----------------
{
    QBPoint::rotateZ(ng);
    evalAB();
}

void Qubit::applyQUOp(QUOperator &op)
// Применение оператора к кубиту
// -----------------------------
{
    rotateZ(op._del());
    rotateY(op._gam());
    rotateZ(op._bet());
}

// -------------------------------------
//        Реализация QUOperator
// -------------------------------------

QUOperator::QUOperator(comp_d a_, comp_d b_, comp_d c_, comp_d d_)
    // Конструктор
    // -----------
    : mA(a_), mB(b_), mC(c_), mD(d_) {
    toZYdec(); // по умолчанию - ZY
}

void QUOperator::set(comp_d a_, comp_d b_, comp_d c_, comp_d d_) {
    mA = a_;
    mB = b_;
    mC = c_;
    mD = d_;
    toZYdec();
}

void QUOperator::toZYdec()
// ZY-разложение
// Автор: Швецкий М.В.
// -------------------
{
    if (abs(mA) && abs(mB)) {
        alpha = arg(mA * mD - mB * mC) / 2.0;
        beta = (arg(mD / mA) + arg(-mC / mB)) / 2.0;

        comp_d z = -2.0 * mA * mB * exp(-C_I * (2.0 * alpha - beta));
        gamma = real(std::asin(z));

        if (real(exp(-C_I * 2.0 * alpha) * (mA * mD + mB * mC)) < EPS)
            gamma = M_PI - gamma;

        delta = (arg(mD / mA) - arg(-mC / mB)) / 2.0;
    } else if (abs(mB) < EPS && abs(mC) < EPS) {
        alpha = arg(mA * mD) / 2.0;
        gamma = 0;
        delta = 0; // delta - любое вещественное число
        beta = arg(mD / mA) - delta;
    } else if (abs(mA) < EPS && abs(mD) < EPS) {
        if (abs(1.0 - mB * mC) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(-mB * mC) / 2.0;
        beta = 0; // beta - любое вещественное число
        gamma = M_PI;
        delta = beta + arg(-mB / mC);
    }

    // Корректировка значения alpha при изменении знака
    // элемента a матрицы U, вычисленного  по найденным
    // значениям alpha, beta, gamma, delta
    // -----------------------------------
    double v = gamma / 2.0;
    if (abs(mA - exp(C_I * (alpha - beta / 2.0 - delta / 2.0)) * cos(v)) > EPS)
        alpha = alpha + M_PI;
}

void QUOperator::toZXdec()
// ZX-разложение
// Автор: Швецкий М.В.
// -------------------
{
    if (abs(mA) && abs(mB)) {
        alpha = arg(mA * mD - mB * mC) / 2.0;
        beta = (arg(mD / mA) + arg(mC / mB)) / 2.0;

        comp_d z = -2.0 * mA * mB * exp(-C_I * (M_PI / 2.0 + 2.0 * alpha - beta));
        gamma = real(std::asin(z));

        if (real(exp(-C_I * 2.0 * alpha) * (mA * mD + mB * mC)) <= 0)
            gamma = M_PI - gamma;

        delta = (arg(mD / mA) - arg(mC / mB)) / 2.0;
    } else if (abs(mB) < EPS && abs(mC) < EPS) {
        alpha = arg(mA * mD) / 2.0;
        gamma = 0;
        delta = 0; // delta - любое вещественное число
        beta = arg(mD / mA) - delta;
    } else if (abs(mA) < EPS && abs(mD) < EPS) {
        if (abs(1.0 - mB * mC) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(-mB * mC) / 2.0;
        beta = 0; // beta - любое вещественное число
        gamma = M_PI;
        delta = beta + arg(mB / mC);
    }

    // Корректировка значения alpha при изменении знака
    // элемента a матрицы U, вычисленного  по найденным
    // значениям alpha, beta, gamma, delta
    // -----------------------------------
    double v = gamma / 2.0;
    if (abs(exp(C_I * (alpha - beta / 2.0 - delta / 2.0)) * cos(v) - mA) > EPS ||
        abs(-C_I * exp(C_I * (alpha - beta / 2.0 + delta / 2.0)) * sin(v) - mB) > EPS ||
        abs(-C_I * exp(C_I * (alpha + beta / 2.0 - delta / 2.0)) * sin(v) - mC) > EPS ||
        abs(exp(C_I * (alpha + beta / 2.0 + delta / 2.0)) * cos(v) - mD) > EPS)
        alpha = alpha + M_PI;
}

void QUOperator::toXYdec()
// ZX-разложение
// Автор: Швецкий М.В.
// -------------------
{
    if (abs(mD) > EPS)
        if (abs(1.0 + mA / conj(mD)) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(mA / conj(mD)) / 2.0;
    else if (abs(mC) > EPS)
        if (abs(1.0 - mB / conj(mC)) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(-mB / conj(mC)) / 2.0;
    // -----------------------------------
    comp_d A, B;
    A = (exp(-C_I * alpha) * (mA + mB) + exp(C_I * alpha) * (conj(mC) + conj(mD))) / 2.0;
    B = (exp(-C_I * alpha) * (mA + mB) - exp(C_I * alpha) * (conj(mC) + conj(mD))) / 2.0;
    double a1, a2, b1, b2;
    a1 = real(A);
    a2 = imag(A);
    b1 = real(B);
    b2 = imag(B);
    // --------------------------------------
    if (fabs(a1) < EPS && fabs(b2) < EPS)
        if (fabs(b1) > EPS) {
            beta = 2.0 * atan(a2 / b1);
            gamma = M_PI;
            delta = 0; // delta - любое вещественное число
        } else if (fabs(a2) > EPS) {
            beta = M_PI - 2.0 * atan(b1 / a2);
            gamma = M_PI;
            delta = 0; // delta - любое вещественное число
        }
    // ----------------------------
    if (fabs(a2) < EPS && fabs(b1) < EPS)
        if (abs(a1) > EPS) {
            beta = 2.0 * atan(-b2 / a1);
            gamma = 0;
            delta = 0; // delta - любое вещественное число
        } else if (fabs(b2) > EPS) {
            beta = M_PI - 2.0 * atan(-a1 / b2);
            gamma = 0;
            delta = 0; // delta - любое вещественное число
        }
    // --------------------------------------------------------
    if (fabs(a1) < EPS && fabs(b1) < EPS && fabs(a2) > EPS && fabs(b2) > EPS) {
        beta = M_PI;
        gamma = -2.0 * asin(a2);
        delta = 0;
    }
    // --------------------------------------------------------
    if (fabs(a1) > EPS && fabs(b1) > EPS && fabs(a2) < EPS && fabs(b2) < EPS) {
        beta = 0;
        gamma = -2.0 * asin(b1);
        delta = 0;
    }
    // --------------------------------------------------------
    if (fabs(a1) > EPS && fabs(a2) > EPS && fabs(b1) < EPS && fabs(b2) < EPS) {
        beta = M_PI / 2.0;
        gamma = 2.0 * asin(-a2);
        delta = -M_PI / 2.0;
    }
    // --------------------------------------------------------
    if (fabs(a1) > EPS && fabs(a2) > EPS && fabs(b1) > EPS && fabs(b2) > EPS) {
        beta = atan(-b2 / a1) + atan(a2 / b1);
        delta = atan(-b2 / a1) - atan(a2 / b1);
        gamma = 2.0 * asin(-a2 / sin(beta / 2.0 - delta / 2.0));
    }
}

bool QUOperator::isUnitary()
// Проверка на унитарность
// -----------------------
{
    comp_d cA = std::conj(mA), cB = std::conj(mC), cC = std::conj(mB), cD = std::conj(mD);
    return eq((mA * cA + mB * cC), comp_d(1, 0)) && eq((mA * cB + mB * cD), comp_d(0, 0)) &&
           eq((mC * cA + mD * cC), comp_d(0, 0)) && eq((mC * cB + mD * cD), comp_d(1, 0));
}

QUOperator QUOperator::operator+(QUOperator &op) const {
    return QUOperator(mA + op._a(), mB + op._b(), mC + op._c(), mD + op._d());
}

QUOperator QUOperator::operator-(QUOperator &op) const {
    return QUOperator(mA - op._a(), mB - op._b(), mC - op._c(), mD - op._d());
}

QUOperator QUOperator::operator*(double n) const {
    return QUOperator(mA * n, mB * n, mC * n, mD * n);
}

QUOperator operator*(const double n, const QUOperator &op) {
    return op * n;
}

QUOperator QUOperator::operator*(comp_d n) const {
    return QUOperator(mA * n, mB * n, mC * n, mD * n);
}

QUOperator operator*(const comp_d n, const QUOperator &op) {
    return op * n;
}

QVector<double> QUOperator::findNVec() {
    QVector<double> qv;
    double th = 2.0 * acos(cos(gamma / 2.0) * cos(beta / 2.0 + delta / 2.0)), sint = sin(th / 2.0),
           nX = sin(gamma / 2.0) / sint * sin(delta / 2.0 - beta / 2.0),
           nY = sin(gamma / 2.0) / sint * cos(beta / 2.0 - delta / 2.0),
           nZ = cos(gamma / 2.0) / sint * sin(beta / 2.0 + delta / 2.0);
    double ln = nX * nX + nY * nY + nZ * nZ;
    qv.push_back(nX);
    qv.push_back(nY);
    qv.push_back(nZ);
    qv.push_back(th);
    return (qv);
}

// -------------------------------------
//       Вспомогательгые функции
// -------------------------------------

bool eq(comp_d a, comp_d b)
// Проверка равенства комплексных чисел
// ------------------------------------
{
    return abs(abs(std::imag(a)) - abs(std::imag(b))) < EPS &&
           abs(abs(std::real(a)) - abs(std::real(b))) < EPS;
}

double okr(double a, double s)
// округление
// ----------
{
    a *= s;
    if (a - floor(a) >= 0.5)
        return (floor(a) + 1) / s;
    else
        return floor(a) / s;
}

QUOperator RXop(double the)
// Оператор поворота вокруг OX на угол the
// ---------------------------------------
{
    the /= 2.0;
    return QUOperator(cos(the), -C_I * sin(the), -C_I * sin(the), cos(the));
}

QUOperator RYop(double the)
// Оператор поворота вокруг OY на угол the
// ---------------------------------------
{
    the /= 2.0;
    return QUOperator(cos(the), -sin(the), sin(the), cos(the));
}

QUOperator RZop(double the)
// Оператор поворота вокруг OZ на угол the
// ---------------------------------------
{
    the /= 2.0;
    return QUOperator(exp(-C_I * the), 0.0, 0.0, exp(C_I * the));
}

QUOperator PHIop(double gam)
// Оператор Phi(gamma)
//--------------------
{
    comp_d mA = pow(C_E, C_I * gam);
    return QUOperator(mA, 0.0, 0.0, 1.0);
}
