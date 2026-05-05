#pragma once

#include <QMainWindow>

class QAction;
class QActionGroup;
class QGraphicsView;
class PolygonScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void loadImage();
    void onImageLoaded(bool loaded);
    void onModeChanged(QAction *action);

private:
    QGraphicsView *view {nullptr};
    PolygonScene *scene {nullptr};

    QAction *loadAction {nullptr};
    QActionGroup *modeGroup {nullptr};
    QAction *drawAction {nullptr};
    QAction *selectAction {nullptr};
    QAction *editAction {nullptr};
};
