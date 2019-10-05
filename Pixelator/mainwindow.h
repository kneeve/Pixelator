// Written by: Hannah Bruns, Tyrus Draper, Kaelin Hoang, Khanhly Nguyen, Megan Schnedar
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "frame.h"
#include "clickablelabel.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QByteArray>
#include <QPoint>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool eventFilter(QObject *obj, QEvent *event);
    ~MainWindow();

private slots:
    void addConnections();
    void setCurrentColor();
    void on_pencil_clicked();
    void on_pxSize1_clicked();
    void on_pxSize2_clicked();
    void on_pxSize3_clicked();
    void on_pxSize4_clicked();
    void on_eraser_clicked();
    void on_circle_clicked();
    void on_square_clicked();
    void on_paintBucket_clicked();
    void on_setSize_clicked();
    void on_frameRateSlider_valueChanged(int value);
    void on_addFrame_clicked();
    void onFrameThumbnailClicked(int);
    void on_deleteFrame_clicked();
    void removeFrame(int, int, bool clearVector);
    void on_clearFrame_clicked();
    void on_actualSize_clicked();

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    void createFrame();
    void addFrameDisplay(Frame&);
    void updateFrameDisplay();
    void addFrameDisplay();
    bool saveDialog();
    bool saveFile(const QByteArray&);
    bool saveSprite(const QString&);
    void loadFromFile();
    void createNew();
    void drawPixel(QPoint);
    void drawPixelsAfterRelease(QPoint);
    void playLivePreview();
    void setAllUIButtonsEnabled(bool);
    void exportSprite();
    bool exportDialog();
    bool exportFile(const QByteArray&);
    bool exportSprite(const QString&);
    QString rgbaSave();

    Ui::MainWindow *ui;
    QColor currentColor;
    QHBoxLayout* layout;
    Frame currentFrame;
    QVector<Frame> frames;
    QVector<ClickableLabel*> displayFrames;
    QPoint shapeStartPoint;
    QPoint previouslyDrawnPixel;
    QPushButton* selectedButton;
    QPushButton* selectedPen;
    const int FRAMEDISPLAYYVALUE;
    const int FRAMESIZE;
    int CANVASWIDTH;
    int CANVASHEIGHT;
    int frameDisplayXValue;
    int pixelWidth;
    int pixelHeight;
    int frameRate;
    bool isFreeDrawing;
    bool isDrawingRect = false;
    bool isDrawingCircle = false;
    bool isFilling;
    bool modified;
    int previewIndex;
    int initialSize;
    QString currentFile;
    bool fileLoaded;
    bool actualSize = false;
};

#endif // MAINWINDOW_H
