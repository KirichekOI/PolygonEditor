#pragma once

#include <QGraphicsScene>
#include <QImage>
#include <QVector>
#include <QPolygonF>

class QGraphicsPixmapItem;
class QGraphicsPolygonItem;
class QGraphicsEllipseItem;

enum class PolygonMode {
    None,
    Draw,
    Select,
    Edit
};

class PolygonScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit PolygonScene(QObject *parent = nullptr);
    ~PolygonScene() = default;

    bool loadImage(const QString &fileName);
    void setMode(PolygonMode mode);

signals:
    void imageLoaded(bool loaded);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void createVertexHandles(QGraphicsPolygonItem *polygon);
    void clearVertexHandles();
    void clearDrawHandles();
    void updatePolygonFromHandles(QGraphicsPolygonItem *polygon);
    void updateHandlesFromPolygon(QGraphicsPolygonItem *polygon);

    QGraphicsPixmapItem *backgroundItem{nullptr};
    PolygonMode currentMode{PolygonMode::None};

    QGraphicsPolygonItem *currentPolygon{nullptr};
    QPolygonF currentPoints;
    QVector<QGraphicsEllipseItem*> drawHandles;
    bool isDrawing{false};

    QGraphicsPolygonItem *editingPolygon{nullptr};
    QVector<QGraphicsEllipseItem*> vertexHandles;
    bool isEditingVertex{false};
    QGraphicsEllipseItem *selectedHandle{nullptr};
    QPointF dragStartPos;
};
