// Реализация методов класс Bloch.
// -------------------------------
// Кадочников Д. 4 курс (06.10.2014)

#include "bloch.hpp"
#include <QEvent>
#include <QWheelEvent>
#include <gl/gl.h>

// -------------------------------------
//         Реализация QBVector
// -------------------------------------

QBVector::QBVector(float a_, comp_d b_)
    : Qubit(a_, b_)
// конструктор
// -----------
{
    fillQBPV();
    emit alpBetChanged(a, b);
    emit xyzChanged(x, y, z);
}

void QBVector::fillQBPV()
// заполнение вектора вершин стрелки
// ---------------------------------
{
    qbpVector.push_back(QBPoint(0.02, 0.0, 0.9));
    qbpVector.push_back(QBPoint(-0.02, 0.0, 0.9));
    qbpVector.push_back(QBPoint(0.0, 0.02, 0.9));
    qbpVector.push_back(QBPoint(0.0, -0.02, 0.9));
}

void QBVector::setPsi(float a_, comp_d b_)
// переопределение метода set
// --------------------------
{
    Qubit::setPsi(a_, b_);
    qbpVector.clear();
    fillQBPV();
    for (int j = 0; j < qbpVector.size(); j++) {
        qbpVector[j].rotateY(this->the);
        qbpVector[j].rotateZ(this->phi);
    }
    emit phiTheChanged(phi, the);
    emit xyzChanged(x, y, z);
}

void QBVector::randomize()
// случайно
// --------
{
    Qubit::randomize();
    qbpVector.clear();
    fillQBPV();
    for (int j = 0; j < qbpVector.size(); j++) {
        qbpVector[j].rotateY(this->the);
        qbpVector[j].rotateZ(this->phi);
    }
    report();
}

void QBVector::set(float p_, float t_)
// переопределение метода set
// --------------------------
{
    Qubit::set(p_, t_);
    qbpVector.clear();
    fillQBPV();
    for (int j = 0; j < qbpVector.size(); j++) {
        qbpVector[j].rotateY(this->the);
        qbpVector[j].rotateZ(this->phi);
    }
    emit alpBetChanged(a, b);
    emit xyzChanged(x, y, z);
}

void QBVector::draw()
// метод рисования
// ---------------
{
    glColor3f(1, 0, 0);
    glLineWidth(2.5f);

    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(x, y, z);
    glEnd();

    glBegin(GL_LINES);
    for (int i = 0; i < qbpVector.size(); i++) {
        glVertex3f(x, y, z);
        glVertex3f(qbpVector[i]._x(), qbpVector[i]._y(), qbpVector[i]._z());
    }
    glEnd();
}

void QBVector::rotateX(float ng)
// переопределение поворотов
// -------------------------
{
    Qubit::rotateX(ng);
    for (int i = 0; i < qbpVector.size(); i++)
        qbpVector[i].rotateX(ng);
}
void QBVector::rotateY(float ng)
// -----------------------------
{
    Qubit::rotateY(ng);
    for (int i = 0; i < qbpVector.size(); i++)
        qbpVector[i].rotateY(ng);
}

void QBVector::rotateZ(float ng)
// -----------------------------
{
    Qubit::rotateZ(ng);
    for (int i = 0; i < qbpVector.size(); i++)
        qbpVector[i].rotateZ(ng);
}
void QBVector::applyQUOp(QUOperator &op) {
    Qubit::applyQUOp(op);
    for (int i = 0; i < qbpVector.size(); i++) {
        qbpVector[i].rotateZ(op._del());
        qbpVector[i].rotateY(op._gam());
        //!!!
        qbpVector[i].rotateZ(op._bet());
    }
}

void QBVector::rotation(float ng, rotType rt, float nX, float nY, float nZ)
// -----------------------------------------
{
    switch (rt) {
    case rX:
        rotateX(ng);
        break;
    case rY:
        rotateY(ng);
        break;
    case rZ:
        rotateZ(ng);
        break;
    case rN:
        //        applyQUOp(cos(ng / 2.0) * Iop - C_I * sin(ng / 2.0) * (nY * Yop + nX * Xop + nZ *
        //        Zop));
        break;
    }
    report();
}

void QBVector::report() {
    emit phiTheChanged(phi, the);
    emit alpBetChanged(a, b);
    emit xyzChanged(x, y, z);
}

void QBVector::reportOp(float nX, float nY, float nZ, float ng) {
    // emit newOp(cos(ng/2.0)*Iop-C_I*sin(ng/2.0)*(nY*Yop+nX*Xop+nZ*Zop));
}

// -------------------------------------
//        Реализация Bloch
// -------------------------------------

enum { TR_SPR = 10, SPR = 20, TR_CLR = 30, CLR = 50 };
const int NCols = 12;
QColor    Cols[NCols] = {Qt::red,      Qt::green,   Qt::blue,        Qt::gray,
                      Qt::yellow,   Qt::darkRed, Qt::darkGreen,   Qt::darkBlue,
                      Qt::darkCyan, Qt::cyan,    Qt::darkMagenta, Qt::magenta};

Bloch::Bloch(QWidget *parent)
    // Конструктор
    // -----------
    : QGLWidget(parent) {
    xRot = -60;
    yRot = 0;
    zRot = -135;
    nSca = 1.2;
    sRadius = 1;
    iter = 0;
    naX = 0;
    naY = 0;
    naZ = 0;
    trCol = 0;
    oZCol = Qt::black;
    oYCol = Qt::black;
    oXCol = Qt::black;
    isTimerActive = false;
    showNewAx = false;
    traceFlag = true;
    font = QFont("System", 11);
}

void Bloch::initializeGL()
// Инициализация OpenGL
// --------------------
{
    qglClearColor(Qt::white);
}

void Bloch::resizeGL(int nWidth, int nHeight)
// Метод изменения размера окна
// ----------------------------
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = (GLfloat)nHeight / (GLfloat)nWidth;

    if (nWidth >= nHeight)
        glOrtho(-2.0 / ratio, 2.0 / ratio, -2.0, 2.0, -10.0, 10.0);
    else
        glOrtho(-2.0, 2.0, -2.0 * ratio, 2.0 * ratio, -10.0, 10.0);
    glViewport(0, 0, (GLfloat)nWidth, (GLfloat)nHeight);
}

void Bloch::wheelEvent(QWheelEvent *pe)
// Обработка события вращение колеса мыши
// --------------------------------------
{
    if (pe->angleDelta().y() > 0)
        scale_plus();
    else if (pe->angleDelta().y() < 0)
        scale_minus();

    updateGL();
}

void Bloch::keyPressEvent(QKeyEvent *pe)
// Обработка события нажатия клавиши
// ---------------------------------
{
    switch (pe->key()) {
    case Qt::Key_Plus:
        scale_plus();
        break;
    case Qt::Key_Minus:
        scale_minus();
        break;
    case Qt::Key_Up:
        rotate_up();
        break;
    case Qt::Key_Down:
        rotate_down();
        break;
    case Qt::Key_Left:
        rotate_left();
        break;
    case Qt::Key_Right:
        rotate_right();
        break;
    case Qt::Key_Space:
        defaultScene();
        break;
    case Qt::Key_Escape:
        defaultScene();
    }

    updateGL();
}

void Bloch::scale_plus()
// Увеличение масштаба
// -------------------
{
    nSca = nSca * 1.1;
}

void Bloch::scale_minus()
// Уменьшение масштаба
// -------------------
{
    nSca = nSca / 1.1;
}

void Bloch::rotate_up()
// Вращение сцены вверх
// --------------------
{
    xRot += 1.0;
}

void Bloch::rotate_down()
// Вращение сцены вниз
// -------------------
{
    xRot -= 1.0;
}

void Bloch::rotate_left()
// Вращение сцены влево
// --------------------
{
    zRot += 1.0;
}

void Bloch::rotate_right()
// Вращение вправо
// ---------------
{
    zRot -= 1.0;
}

void Bloch::defaultScene()
// Сцена по умолчанию
// ------------------
{
    xRot = -60;
    yRot = 0;
    zRot = -135;
    nSca = 1.2;
}

void Bloch::mousePressEvent(QMouseEvent *pe)
// Обработка события нажатия мыши
// ------------------------------
{
    this->setFocus();
    ptrMousePosition = pe->pos();
}

void Bloch::mouseMoveEvent(QMouseEvent *pe)
// Обработка события движения мыши
// -------------------------------
{
    xRot += 180 / nSca * (GLfloat)(pe->y() - ptrMousePosition.y()) / height();
    zRot += 180 / nSca * (GLfloat)(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL();
}

void Bloch::drawSphere(double r, int lats, int longs) {
    int i, j;
    for (i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

void Bloch::paintGL()
// Метод рисования
// ---------------
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // if(isTimerActive)
    //    drawRotInfo();

    // Масштаб
    glScalef(nSca, nSca, nSca);

    // Вращение системы координат
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);

    // Отрисовка сферы
    glColor4f(0.85f, 0.85f, 0.85f, 0.5f);
    drawSphere(1.0, 50, 50);
    drawCircle();

    glEnable(GL_DEPTH_TEST);

    drawAxis(); // Отрисовка осей
    drawNewAx();

    if (traceFlag)
        drawTrace(); // Траектория

    bool cond = (abs(qubitVector._x()) > 0.01f || abs(qubitVector._y()) > 0.01f) &&
                (abs(qubitVector._x()) > 0.01f || abs(qubitVector._z()) > 0.01f) &&
                (abs(qubitVector._y()) > 0.01f || abs(qubitVector._z()) > 0.01f);

    if (cond) {
        glDisable(GL_DEPTH_TEST);
        drawProj(); // Проекции на оси
        glEnable(GL_DEPTH_TEST);
    }

    if (!cond)
        glDisable(GL_DEPTH_TEST);
    qubitVector.draw(); // Рисование вектора
    if (!cond)
        glEnable(GL_DEPTH_TEST);

    glDisable(GL_DEPTH_TEST);
}

void Bloch::showDecC() {
    glColor3f(0.0f, 0.0f, 0.0f);
    QString st = QString("(%1;%2;%3)")
                     .arg(okr(qubitVector._x(), 100)) //%4
                     .arg(okr(qubitVector._y(), 100))
                     .arg(okr(qubitVector._z(), 100));
    renderText(-1.9, 1.8, 0.0, st, font);
}

void Bloch::drawAxis()
// Рисовать осеи координат
// -----------------------
{
    float axSize = 1.7f;

    glLineWidth(2.0f);
    // OX
    qglColor(oXCol);
    glBegin(GL_LINES);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(-axSize, 0.f, 0.f);

    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1, 0.f, 0.025f);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1, 0.f, -0.025f);

    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1f, 0.025f, 0.f);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1f, -0.025f, 0.f);
    glEnd();
    renderText(axSize + 0.1, 0.0, 0.0, "x", font);
    // OY
    qglColor(oYCol);
    glBegin(GL_LINES);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, -axSize, 0.f);

    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, axSize - 0.1f, 0.025f);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, axSize - 0.1f, -0.025f);

    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.025f, axSize - 0.1f, 0.f);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(-0.025f, axSize - 0.1f, 0.f);
    glEnd();
    renderText(0, axSize + 0.1f, 0.f, "y", font);
    // OZ
    qglColor(oZCol);
    glBegin(GL_LINES);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, 0.f, -axSize);

    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.025f, 0.f, axSize - 0.1f);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(-0.025f, 0.f, axSize - 0.1f);

    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, 0.025f, axSize - 0.1f);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, -0.025f, axSize - 0.1f);
    glEnd();
    renderText(0, 0, axSize + 0.1f, "z", font);
}

void Bloch::drawNewAx() {
    if (!showNewAx)
        return;
    bool cond;
    glColor3f(0.0f, 0.9f, 0.0f);
    glLineWidth(2.5f);

    float ax, ay, az;

    if (isTimerActive)
        ax = rZYCur.naX, ay = rZYCur.naY, az = rZYCur.naZ;
    else
        ax = naX, ay = naY, az = naZ;

    cond = !((abs(ax) > 0.001f || abs(ay) > 0.001f) && (abs(ax) > 0.001f || abs(az) > 0.001f) &&
             (abs(ay) > 0.001f || abs(az) > 0.001f));

    if (cond)
        glDisable(GL_DEPTH_TEST);

    glBegin(GL_LINES);
    glVertex3f(-ax, -ay, -az);
    glVertex3f(ax, ay, az);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(ax * 1.15, ay * 1.15, az * 1.3, "n", font);

    if (cond)
        glEnable(GL_DEPTH_TEST);
}

void Bloch::drawCircle()
// Рисовать круг на XOY
// --------------------
{
    glColor4f(0.7f, 0.8f, 0.8f, 0.5f);
    glBegin(GL_POLYGON);
    for (float i = 0.f; i < 2.f * 3.14f; i += 3.14f / 18.f)
        glVertex2f(sRadius * sin(i), sRadius * cos(i));
    glEnd();
    glLineWidth(1.5f);
    glColor3f(0.6f, 0.7f, 0.7f);
    glBegin(GL_LINE_LOOP);
    for (float i = 0.f; i < 2.f * 3.14f; i += 3.14f / 18.f)
        glVertex2f(sRadius * sin(i), sRadius * cos(i));
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(0.0, 0.05, -1.2, "|1>", font);
    renderText(0.0, 0.05, 1.2, "|0>", font);
}

void Bloch::drawTrace()
// Рисовать след
// -------------
{
    if (tracePts.isEmpty())
        return;

    qglColor(Cols[trCol]);
    glLineWidth(1.5f);

    glBegin(GL_LINES);
    for (int i = tracePts.size() - 1; i > 5; i -= 3) {
        if (tracePts[i] > TR_SPR) {
            if (tracePts[i] > TR_CLR) {
                qglColor(Cols[int(tracePts[i]) - CLR]);
                i--;
            }
            if (tracePts[i] > TR_SPR) {
                glEnd();
                glBegin(GL_LINES);
                i--;
            }
            if (tracePts[i] > TR_CLR) {
                qglColor(Cols[int(tracePts[i]) - CLR]);
                i--;
            }
        } else {

            if (tracePts[i - 3] < TR_SPR) {
                glVertex3f(tracePts[i - 2], tracePts[i - 1], tracePts[i]);
                glVertex3f(tracePts[i - 5], tracePts[i - 4], tracePts[i - 3]);
            }
        }
    }
    glEnd();
}

void Bloch::drawProj()
// Рисовать проекции
// -----------------
{
    glColor3f(0.f, 0.5f, 1.f);
    glLineWidth(2.5f);

    glBegin(GL_LINES);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(qubitVector._x(), 0, 0);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, qubitVector._y(), 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, qubitVector._z());
    glEnd();
}

void Bloch::drawRotInfo() {
    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(-1.9f, 1.8f, 0.f, rZYCur.msg, font);
}

void Bloch::addTracePoint() {
    if (!traceFlag)
        return;

    if (tracePts.isEmpty())
        setTraceColor(trCol);

    tracePts.push_back(qubitVector._x());
    tracePts.push_back(qubitVector._y());
    tracePts.push_back(qubitVector._z());

    if (tracePts.size() > 1000000) {
        while (tracePts[1] > TR_SPR) {
            if (tracePts[1] > TR_CLR)
                tracePts.remove(0, 1);
            else
                tracePts.remove(1, 1);
        }
        tracePts.remove(1, 3);
    }
    updateGL();
}

void Bloch::addTraceSeparator() {
    if (!traceFlag || tracePts.empty())
        return;

    if ((tracePts.size() > 1 && tracePts[tracePts.size() - 2] > TR_SPR) ||
        (tracePts.last() > TR_SPR && tracePts.last() < TR_CLR))
        return;

    tracePts.push_back(SPR);
}

void Bloch::setTraceColor(int n) {

    if (traceFlag) {
        if ((!tracePts.isEmpty()) && tracePts.last() > TR_CLR)
            tracePts.pop_back();

        tracePts.push_back(trCol + CLR);
    }

    trCol = n;
}

void Bloch::setDrawTrace(bool st) {
    if (!(traceFlag = st)) // да, =
        tracePts.clear();
    updateGL();
}

void Bloch::clearTrace() {
    tracePts.clear();
    updateGL();
}

void Bloch::setTP(float t, float p)
// Установить положение вектора
// аргументы - углы тэта и фи
// --------------------------
{
    if (isTimerActive)
        return;
    qubitVector.set(p * RAD, t * RAD);
    addTraceSeparator();

    updateGL();
}

void Bloch::setAB(float a, comp_d b)
// Установить положение вектора
// аргументы - альфа и бета
// ------------------------
{
    if (isTimerActive)
        return;
    qubitVector.setPsi(a, b);
    addTraceSeparator();

    updateGL();
}

void Bloch::setRandomPsi()
// Установить положение вектора
// аргументы - альфа и бета
// ------------------------
{
    if (isTimerActive)
        return;
    qubitVector.randomize();
    addTraceSeparator();

    updateGL();
}

void Bloch::startMotion()
// Запуск анимации
// ---------------
{
    if (rZYQue.empty())
        return;
    rZYCur = rZYQue.dequeue();
    timerID = startTimer(10);
    isTimerActive = true;
    emit motionBegin(rZYCur.msg);
    if (rZYCur.typ == rZ) {
        if (1.f - abs(qubitVector._z()) < 0.0001f) {
            stopMotion();
            return;
        }
        oXCol = Qt::black;
        oYCol = Qt::black;
        oZCol = Qt::gray;
    } else if (rZYCur.typ == rY) {
        if (1.f - abs(qubitVector._y()) < 0.0001f) {
            stopMotion();
            return;
        }
        oXCol = Qt::black;
        oZCol = Qt::black;
        oYCol = Qt::gray;
    } else if (rZYCur.typ == rX) {
        if (1.f - abs(qubitVector._x()) < 0.0001f) {
            stopMotion();
            return;
        }
        oZCol = Qt::black;
        oYCol = Qt::black;
        oXCol = Qt::gray;
    }
}

void Bloch::stopMotion()
// Остановка/продолжение анимации
// ------------------------------
{
    killTimer(timerID);
    emit motionEnd();
    if (rZYQue.empty()) {
        oXCol = Qt::black;
        oZCol = Qt::black;
        oYCol = Qt::black;
        isTimerActive = false;
    } else
        startMotion();
}

void Bloch::rotateZY(float zb, float yg, float zd, QString msg)
// ZY-вращение
// Rz(zb), Ry(yg), Rz(zd)
// ----------------------
{
    // Rz(delta)
    if (zb)
        rZYQue.push_back(rZYStruct(zb, rZ, msg));
    // Ry(gamma)
    if (yg)
        rZYQue.push_back(rZYStruct(yg, rY, msg));
    // Rz(beta)
    if (zd)
        rZYQue.push_back(rZYStruct(zd, rZ, msg));

    if (!isTimerActive)
        startMotion();
}

void Bloch::rotateZX(float zb, float xg, float zd, QString msg)
// ZX-вращение
// Rz(zb), Rx(xg), Rz(zd)
// ----------------------
{
    // Rz(delta)
    if (zb)
        rZYQue.push_back(rZYStruct(zb, rZ, msg));
    // Rx(gamma)
    if (xg)
        rZYQue.push_back(rZYStruct(xg, rX, msg));
    // Rz(beta)
    if (zd)
        rZYQue.push_back(rZYStruct(zd, rZ, msg));

    if (!isTimerActive)
        startMotion();
}

void Bloch::rotateXY(float xb, float yg, float xd, QString msg)
// XY-вращение
// Rx(xb), Ry(yg), Rx(xd)
// ----------------------
{
    // Rx(delta)
    if (xb)
        rZYQue.push_back(rZYStruct(xb, rX, msg));
    // Ry(gamma)
    if (yg)
        rZYQue.push_back(rZYStruct(yg, rY, msg));
    // Rx(beta)
    if (xd)
        rZYQue.push_back(rZYStruct(xd, rX, msg));

    if (!isTimerActive)
        startMotion();
}

void Bloch::setNewAxis(float ax, float ay, float az) {
    showNewAx = true;
    naX = ax;
    naY = ay;
    naZ = az;
    updateGL();
}

void Bloch::rotateN(float nt, QString msg) {
    if (!showNewAx)
        return;

    if (nt)
        rZYQue.push_back(rZYStruct(nt, rN, msg, naX, naY, naZ));

    if (!isTimerActive)
        startMotion();
}

void Bloch::timerEvent(QTimerEvent *pe)
// Анимация вращения
// -----------------
{
    iter;

    if (abs(iter) < abs(rZYCur.ang)) {
        addTracePoint();
        if (rZYCur.ang > 0) {

            if (rZYCur.typ == rN)
                qubitVector.rotation(RAD, rZYCur.typ, rZYCur.naX, rZYCur.naY, rZYCur.naZ);
            else
                qubitVector.rotation(RAD, rZYCur.typ);
            iter++;
        } else {

            if (rZYCur.typ == rN)
                qubitVector.rotation(-RAD, rZYCur.typ, rZYCur.naX, rZYCur.naY, rZYCur.naZ);
            else
                qubitVector.rotation(-RAD, rZYCur.typ);
            iter--;
        }
        updateGL();
    } else {
        if (rZYCur.typ == rN)
            qubitVector.rotation(RAD * (rZYCur.ang - iter), rZYCur.typ, rZYCur.naX, rZYCur.naY,
                                 rZYCur.naZ);
        else
            qubitVector.rotation(RAD * (rZYCur.ang - iter), rZYCur.typ);

        addTracePoint();

        iter = 0;

        stopMotion();
        updateGL();
    }
}

void Bloch::reset() {
    showNewAx = false;
    rZYQue.clear();
    stopMotion();
    iter = 0;
    tracePts.clear();
    defaultScene();
    setAB(1, comp_d(0.0, 0.0));
    qubitVector.report();
    updateGL();
}
