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

#include <QDebug>
#include <QGuiApplication>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <Qt3DInput/QInputAspect>

#include <QPropertyAnimation>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QRenderAspect>

#include "qt3dwindow.h"

void drawLine(const QVector3D &start, const QVector3D &end, const QColor &color,
              Qt3DCore::QEntity *_rootEntity) {
    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float *>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    auto *buf = new Qt3DRender::QBuffer(geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int *>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(_rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

Qt3DCore::QEntity *createScene() {
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongAlphaMaterial(rootEntity);

    //    // Torus
    //    Qt3DCore::QEntity *     torusEntity = new Qt3DCore::QEntity(rootEntity);
    //    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    //    torusMesh->setRadius(5);
    //    torusMesh->setMinorRadius(1);
    //    torusMesh->setRings(100);
    //    torusMesh->setSlices(20);

    //    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    //    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    //    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    //    torusEntity->addComponent(torusMesh);
    //    torusEntity->addComponent(torusTransform);
    //    torusEntity->addComponent(material);

    // Sphere
    Qt3DCore::QEntity *      sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(5);
    sphereMesh->setSlices(100);
    sphereMesh->setRings(100);
    qDebug() << sphereMesh->rings();
    qDebug() << sphereMesh->slices();
    sphereMesh->setGenerateTangents(true);

    //    Qt3DCore::QTransform *    sphereTransform = new Qt3DCore::QTransform;
    //    OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
    //    controller->setTarget(sphereTransform);
    //    controller->setRadius(20.0f);

    //    QPropertyAnimation *sphereRotateTransformAnimation = new
    //    QPropertyAnimation(sphereTransform);
    //    sphereRotateTransformAnimation->setTargetObject(controller);
    //    sphereRotateTransformAnimation->setPropertyName("angle");
    //    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    //    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    //    sphereRotateTransformAnimation->setDuration(10000);
    //    sphereRotateTransformAnimation->setLoopCount(-1);
    //    sphereRotateTransformAnimation->start();

    sphereEntity->addComponent(sphereMesh);
    //    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);

    drawLine({-10, 0, 0}, {10, 0, 0}, Qt::red, rootEntity);   // X
    drawLine({0, -10, 0}, {0, 10, 0}, Qt::green, rootEntity); // Y
    drawLine({0, 0, -10}, {0, 0, 10}, Qt::blue, rootEntity);  // Z
    return rootEntity;
}

int main(int argc, char *argv[]) {
    QGuiApplication        app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *scene = createScene();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    //    camera->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController =
        new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed(0.0f);
    camController->setLookSpeed(180.0f);
    camController->setCamera(camera);

    view.setRootEntity(scene);
    view.show();
    return app.exec();
}
