#include "MainWindow.h"
#include "PolygonScene.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Polygon Editor"));
    setMinimumSize(800, 600);

    view = new QGraphicsView(this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setCentralWidget(view);

    scene = new PolygonScene(this);
    view->setScene(scene);

    connect(scene, &PolygonScene::imageLoaded, this, &MainWindow::onImageLoaded);

    auto *toolBar = addToolBar(tr("Tools"));

    loadAction = new QAction(tr("Load Image"), this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadImage);
    toolBar->addAction(loadAction);

    toolBar->addSeparator();

    modeGroup = new QActionGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->setEnabled(false);

    drawAction = new QAction(tr("Draw Polygon"), this);
    drawAction->setCheckable(true);
    modeGroup->addAction(drawAction);
    toolBar->addAction(drawAction);

    selectAction = new QAction(tr("Select/Move Polygon"), this);
    selectAction->setCheckable(true);
    modeGroup->addAction(selectAction);
    toolBar->addAction(selectAction);

    editAction = new QAction(tr("Edit Vertices"), this);
    editAction->setCheckable(true);
    modeGroup->addAction(editAction);
    toolBar->addAction(editAction);

    connect(modeGroup, &QActionGroup::triggered, this, &MainWindow::onModeChanged);

    auto *fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(loadAction);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), this, &QWidget::close);

    auto *modeMenu = menuBar()->addMenu(tr("Mode"));
    modeMenu->addAction(drawAction);
    modeMenu->addAction(selectAction);
    modeMenu->addAction(editAction);
}

void MainWindow::loadImage()
{
    auto picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    if (picturesPath.isEmpty()) {
        picturesPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    const auto fileName = QFileDialog::getOpenFileName(this,
                                                       tr("Open Image"),
                                                       picturesPath,
                                                       tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    if (!fileName.isEmpty() && !scene->loadImage(fileName)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to load image!"));
    }
}

void MainWindow::onImageLoaded(bool loaded)
{
    modeGroup->setEnabled(loaded);

    if (loaded) {
        drawAction->setChecked(true);
        scene->setMode(PolygonMode::Draw);
    } else {
        scene->setMode(PolygonMode::None);
    }
}

void MainWindow::onModeChanged(QAction *action)
{
    if (action == drawAction) {
        scene->setMode(PolygonMode::Draw);
    } else if (action == selectAction) {
        scene->setMode(PolygonMode::Select);
    } else if (action == editAction) {
        scene->setMode(PolygonMode::Edit);
    }
}
