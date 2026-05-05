#include "PolygonScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QImage>
#include <QPixmap>

PolygonScene::PolygonScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 800, 600);
}

bool PolygonScene::loadImage(const QString &fileName)
{
    QImage image;
    if (!image.load(fileName)) {
        emit imageLoaded(false);
        return false;
    }

    clear();

    backgroundItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    backgroundItem->setZValue(-1);
    addItem(backgroundItem);

    setSceneRect(backgroundItem->boundingRect());

    currentPolygon = nullptr;
    currentPoints.clear();
    isDrawing = false;
    clearDrawHandles();
    clearVertexHandles();
    editingPolygon = nullptr;

    emit imageLoaded(true);
    return true;
}

void PolygonScene::setMode(PolygonMode mode)
{
    if (backgroundItem == nullptr && mode != PolygonMode::None) {
        return;
    }

    currentMode = mode;

    if (currentPolygon) {
        removeItem(currentPolygon);
        delete currentPolygon;
        currentPolygon = nullptr;
    }
    currentPoints.clear();
    isDrawing = false;
    clearDrawHandles();

    clearVertexHandles();
    editingPolygon = nullptr;
    isEditingVertex = false;
    selectedHandle = nullptr;

    if (mode == PolygonMode::Select) {
        const auto itemsList = items();
        for (auto *item : itemsList) {
            if (auto *polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
                polygon->setFlag(QGraphicsItem::ItemIsMovable, true);
                polygon->setFlag(QGraphicsItem::ItemIsSelectable, true);
            }
        }
    } else {
        const auto itemsList = items();
        for (auto *item : itemsList) {
            if (auto *polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
                polygon->setFlag(QGraphicsItem::ItemIsMovable, false);
                polygon->setFlag(QGraphicsItem::ItemIsSelectable, false);
            }
        }
    }
}

void PolygonScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentMode == PolygonMode::None || backgroundItem == nullptr) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    const auto pos = event->scenePos();

    switch (currentMode) {
    case PolygonMode::Draw:
        if (event->button() == Qt::LeftButton) {
            if (!isDrawing) {
                isDrawing = true;
                currentPoints.clear();
                currentPoints << pos;

                currentPolygon = new QGraphicsPolygonItem();
                currentPolygon->setPen(QPen(Qt::green, 2));
                currentPolygon->setBrush(QBrush(Qt::green, Qt::Dense4Pattern));
                addItem(currentPolygon);

                auto *handle = new QGraphicsEllipseItem(-4, -4, 8, 8);
                handle->setPos(pos);
                handle->setPen(QPen(Qt::blue, 2));
                handle->setBrush(QBrush(Qt::blue));
                addItem(handle);
                drawHandles.append(handle);
            } else {
                currentPoints << pos;
                currentPolygon->setPolygon(currentPoints);

                auto *handle = new QGraphicsEllipseItem(-4, -4, 8, 8);
                handle->setPos(pos);
                handle->setPen(QPen(Qt::blue, 2));
                handle->setBrush(QBrush(Qt::blue));
                addItem(handle);
                drawHandles.append(handle);
            }
            update();
        } else if (event->button() == Qt::RightButton && isDrawing) {
            if (currentPoints.size() >= 3) {
                auto *polygonItem = new QGraphicsPolygonItem(currentPoints);
                polygonItem->setPen(QPen(Qt::red, 2));
                polygonItem->setBrush(QBrush(QColor::fromHsv((items().size() * 50) % 360, 200, 200), Qt::Dense4Pattern));
                addItem(polygonItem);
            }

            clearDrawHandles();

            removeItem(currentPolygon);
            delete currentPolygon;
            currentPolygon = nullptr;
            currentPoints.clear();
            isDrawing = false;
            update();
        }
        break;

    case PolygonMode::Select:
        QGraphicsScene::mousePressEvent(event);
        break;

    case PolygonMode::Edit:
        if (event->button() == Qt::LeftButton) {
            selectedHandle = nullptr;
            for (auto *handle : vertexHandles) {
                if (handle->isUnderMouse()) {
                    selectedHandle = handle;
                    isEditingVertex = true;
                    dragStartPos = pos;
                    break;
                }
            }

            if (!selectedHandle) {
                const auto itemsList = items(pos);
                for (auto *item : itemsList) {
                    if (auto *polygon = dynamic_cast<QGraphicsPolygonItem*>(item)) {
                        if (editingPolygon != polygon) {
                            clearVertexHandles();
                            editingPolygon = polygon;
                            createVertexHandles(editingPolygon);
                        }
                        break;
                    }
                }
            }
        }
        break;

    default:
        QGraphicsScene::mousePressEvent(event);
        break;
    }
}

void PolygonScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentMode == PolygonMode::None || backgroundItem == nullptr) {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }

    const auto pos = event->scenePos();

    switch (currentMode) {
    case PolygonMode::Draw:
        if (isDrawing && currentPolygon) {
            auto tempPoints = currentPoints;
            tempPoints << pos;
            currentPolygon->setPolygon(tempPoints);
        }
        break;

    case PolygonMode::Select:
        QGraphicsScene::mouseMoveEvent(event);

        if (editingPolygon) {
            updateHandlesFromPolygon(editingPolygon);
        }
        break;

    case PolygonMode::Edit:
        if (isEditingVertex && selectedHandle) {
            const auto delta = pos - dragStartPos;
            selectedHandle->moveBy(delta.x(), delta.y());
            dragStartPos = pos;
            updatePolygonFromHandles(editingPolygon);
        }
        break;

    default:
        QGraphicsScene::mouseMoveEvent(event);
        break;
    }
}

void PolygonScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentMode == PolygonMode::None || backgroundItem == nullptr) {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    switch (currentMode) {
    case PolygonMode::Select:
        QGraphicsScene::mouseReleaseEvent(event);
        break;

    case PolygonMode::Edit:
        isEditingVertex = false;
        selectedHandle = nullptr;
        break;

    default:
        QGraphicsScene::mouseReleaseEvent(event);
        break;
    }
}

void PolygonScene::createVertexHandles(QGraphicsPolygonItem *polygon)
{
    if (!polygon)
    {
        return;
    }

    clearVertexHandles();

    const auto poly = polygon->polygon();
    const auto polygonPos = polygon->pos();

    for (const auto &point : poly) {
        auto *handle = new QGraphicsEllipseItem(-5, -5, 10, 10);
        handle->setPos(polygonPos + point);
        handle->setPen(QPen(Qt::blue, 2));
        handle->setBrush(QBrush(Qt::blue));
        handle->setFlag(QGraphicsItem::ItemIsMovable, false);
        handle->setZValue(1);
        addItem(handle);
        vertexHandles.append(handle);
    }
}

void PolygonScene::clearVertexHandles()
{
    for (auto *handle : vertexHandles) {
        removeItem(handle);
        delete handle;
    }
    vertexHandles.clear();
}

void PolygonScene::clearDrawHandles()
{
    for (auto *handle : drawHandles) {
        removeItem(handle);
        delete handle;
    }
    drawHandles.clear();
}

void PolygonScene::updatePolygonFromHandles(QGraphicsPolygonItem *polygon)
{
    if (!polygon)
    {
        return;
    }

    QPolygonF newPolygon;
    for (auto *handle : vertexHandles) {
        newPolygon << (handle->pos() - polygon->pos());
    }
    polygon->setPolygon(newPolygon);
}

void PolygonScene::updateHandlesFromPolygon(QGraphicsPolygonItem *polygon)
{
    if (!polygon || editingPolygon != polygon)
    {
        return;
    }

    const auto poly = polygon->polygon();
    const auto polygonPos = polygon->pos();

    for (int i = 0; i < vertexHandles.size() && i < poly.size(); ++i) {
        vertexHandles[i]->setPos(polygonPos + poly[i]);
    }
}
