// Kaelin Hoang, u0984462 | Tyrus Draper, u1038756 | Megan Schnedar, u0924126 | Khanhly Nguyen, u0822847 | Hannah Bruns, u0629077
/*
 * Gui class for a sprite editor
 *
 * Class follows the following outline:
 * -Setup Window
 * -User input button signals
 *
 * Pixelator (MainWindow) implements a multi-featured sprite editor application capable of saving/loading past projects, displaying current projects
 * and exporting projects to a gif.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablelabel.h"
#include "gif.h"
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    FRAMEDISPLAYYVALUE(530),
    FRAMESIZE(128),
    frameDisplayXValue(8),
    modified(false)
{
    ui->setupUi(this);

    currentColor = Qt::black;
    currentFrame = Frame();
    pixelWidth = 64;
    pixelHeight = 64;
    CANVASWIDTH = ui->frameLabel->width();
    CANVASHEIGHT = ui->frameLabel->height();
    currentFile = "untitled";
    fileLoaded = false;

    layout = new QHBoxLayout(ui->frameDisplayScrollArea);
    ui->deleteFrame->setEnabled(false);
    ui->clearFrame->setEnabled(false);

    ui->frameDisplayScrollAreaWidget->setLayout(layout);

    // Range for frame rate is [0, 64]
    frameRate = 32;
    previewIndex = 0;
    initialSize = 0;

    // Set colorDisplay label
    ui->colorDisplay->setStyleSheet("QLabel{background-color: black}");

    addConnections();

    // Set up event filers for hovering
    ui->green->installEventFilter(this);
    ui->black->installEventFilter(this);
    ui->white->installEventFilter(this);
    ui->yellow->installEventFilter(this);
    ui->blue->installEventFilter(this);
    ui->magenta->installEventFilter(this);
    ui->red->installEventFilter(this);
    ui->gray->installEventFilter(this);
    ui->square->installEventFilter(this);
    ui->circle->installEventFilter(this);
    ui->paintBucket->installEventFilter(this);
    ui->eraser->installEventFilter(this);
    ui->pencil->installEventFilter(this);
    ui->pxSize1->installEventFilter(this);
    ui->pxSize2->installEventFilter(this);
    ui->pxSize3->installEventFilter(this);
    ui->pxSize4->installEventFilter(this);
    ui->addFrame->installEventFilter(this);
    ui->deleteFrame->installEventFilter(this);
    ui->clearFrame->installEventFilter(this);
    ui->setSize->installEventFilter(this);
    ui->actualSize->installEventFilter(this);

    // Set UI buttons to disabled
    setAllUIButtonsEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addConnections()
{
    QObject::connect(ui->white, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->black, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->gray, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->red, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->green, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->blue, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->yellow, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);
    QObject::connect(ui->magenta, &QPushButton::clicked,
                     this, &MainWindow::setCurrentColor);

    // Menubar connections
    QObject::connect(ui->saveFile, &QAction::triggered,
                     this, &MainWindow::saveDialog);
    QObject::connect(ui->openFile, &QAction::triggered,
                     this, &MainWindow::loadFromFile);
    QObject::connect(ui->createNewFile, &QAction::triggered,
                     this, &MainWindow::createNew);
    QObject::connect(ui->exportGif, &QAction::triggered,
                     this, &MainWindow::exportDialog);
}


/*
 *  Method to set current pencil color
 */
void MainWindow::setCurrentColor()
{
    // Get the button that sent the signal, cast to string
    QObject* sender = this->sender();
    QString buttonString = (QString)sender->objectName();
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());

    if(buttonString == "white")
    {
        currentColor = Qt::white;
        currentFrame.setPenColor(Qt::white);
        ui->colorDisplay->setStyleSheet("QLabel{background-color : white; border: 1px solid black;}");
    }
    else if(buttonString == "black")
    {
        currentColor = Qt::black;
        currentFrame.setPenColor(Qt::black);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "gray")
    {
        currentColor = Qt::gray;
        currentFrame.setPenColor(Qt::gray);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "red")
    {
        currentColor = Qt::red;
        currentFrame.setPenColor(Qt::red);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "green")
    {
        currentColor = Qt::green;
        currentFrame.setPenColor(Qt::green);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "blue")
    {
        currentColor = Qt::blue;
        currentFrame.setPenColor(Qt::blue);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "yellow")
    {
        currentColor = Qt::yellow;
        currentFrame.setPenColor(Qt::yellow);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }
    else if(buttonString == "magenta")
    {
        currentColor = Qt::magenta;
        currentFrame.setPenColor(Qt::magenta);
        ui->colorDisplay->setStyleSheet("QLabel{border : none;}");
    }

    pixmap.fill(currentColor);
    ui->colorDisplay->setPixmap(pixmap);
}

/*
 * Sets up the drawing pen
 */
void MainWindow::on_pencil_clicked()
{
    currentFrame.setPenColor(currentColor);
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
    pixmap.fill(currentColor);
    ui->colorDisplay->setPixmap(pixmap);
    repaint();

    isFreeDrawing = true;
    isDrawingCircle = false;
    isDrawingRect = false;
    isFilling = false;

    selectedButton->setDown(false);
    ui->pencil->setDown(true);
    selectedButton = ui->pencil;
    selectedPen->setDown(true);
}

/*
 *  "Erases" color by setting currentColor to match white background
 */
void MainWindow::on_eraser_clicked()
{
    currentFrame.setPenColor(Qt::white);
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
    pixmap.fill(Qt::white);
    ui->colorDisplay->setPixmap(pixmap);
    repaint();

    isFreeDrawing = true;
    isDrawingCircle = false;
    isDrawingRect = false;
    isFilling = false;

    selectedButton->setDown(false);
    ui->eraser->setDown(true);
    selectedButton = ui->eraser;
    selectedPen->setDown(true);
}

/*
 *  Method to update internally held "frameWidth" and "frameHeight"
 *  null and 0 values will be default set to 128
 */
void MainWindow::on_setSize_clicked()
{
    setAllUIButtonsEnabled(true);
    pixelWidth = ui->sizeDropBox->currentText().toInt();
    pixelHeight = pixelWidth;
    initialSize = pixelHeight;
    createFrame();
    addFrameDisplay(currentFrame);

    // Default number of pixels is 64, scale the pixel size to match the shrinking number of pixels
    currentFrame.penSize = 1;
    currentFrame.penColor = currentColor;

    isFreeDrawing = true;
    isDrawingCircle = false;
    isDrawingRect = false;
    isFilling = false;

    ui->pencil->setDown(true);
    selectedButton = ui->pencil;
    ui->pxSize1->setDown(true);
    selectedPen = ui->pxSize1;

    ui->sizeDropBox->setEnabled(false);
    ui->setSize->setVisible(false);
}

/*
 * Slider value changed slot, updates frame rate display
 * Modifies default frame rate (32) to int within [0, 64]
 * 0 fps displays current frame, 64 fps displays gif at predefined full speed.
 */
void MainWindow::on_frameRateSlider_valueChanged(int value)
{
    frameRate = value;

    QString frameR = "Frame Rate: " + QString::number(value);
    ui->frameRate->setText(frameR);
    ui->frameRate->setAlignment(Qt::AlignLeft);
    ui->frameRate->setFrameStyle(QFrame::NoFrame);
    ui->frameRate->show();
}

/*
 * Sets up a new frame and adds the frame to the display of frame icons
 */
void MainWindow::on_addFrame_clicked()
{
    ui->deleteFrame->setEnabled(true);
    createFrame();
    addFrameDisplay(currentFrame);
}

/*
 *  Reassign current frame by indexing into frames vector with ID parameter, updates frame's border in preview frame layout
 */
void MainWindow::onFrameThumbnailClicked(int ID)
{
    for(int i = 0; i < displayFrames.size(); i++)
    {
        displayFrames[i]->setStyleSheet("border: none");
    }
    currentFrame = frames[ID];
    displayFrames[ID]->setStyleSheet("border: 2px outset blue");
    currentFrame.setPenColor(currentColor);
    ui->clearFrame->setEnabled(true);
    updateFrameDisplay();
}

/*
 * Methods for setting up different drawing functionality
 */
void MainWindow::on_square_clicked()
{
    currentFrame.setPenColor(currentColor);
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
    pixmap.fill(currentColor);
    ui->colorDisplay->setPixmap(pixmap);
    repaint();

    isFreeDrawing = false;
    isDrawingCircle = false;
    isDrawingRect = true;
    isFilling = false;

    selectedButton->setDown(false);
    ui->square->setDown(true);
    selectedButton = ui->square;
    selectedPen->setDown(true);
}

void MainWindow::on_circle_clicked()
{
    currentFrame.setPenColor(currentColor);
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
    pixmap.fill(currentColor);
    ui->colorDisplay->setPixmap(pixmap);
    repaint();

    isFreeDrawing = false;
    isDrawingCircle = true;
    isDrawingRect = false;
    isFilling = false;

    selectedButton->setDown(false);
    ui->circle->setDown(true);
    selectedButton = ui->circle;
    selectedPen->setDown(true);
}

void MainWindow::on_paintBucket_clicked()
{
    currentFrame.setPenColor(currentColor);
    QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
    pixmap.fill(currentColor);
    ui->colorDisplay->setPixmap(pixmap);
    repaint();

    isFreeDrawing = false;
    isDrawingCircle = false;
    isDrawingRect = false;
    isFilling = true;

    selectedButton->setDown(false);
    ui->paintBucket->setDown(true);
    selectedButton = ui->paintBucket;
    selectedPen->setDown(true);
}

/*
 * Methods for setting internally held current frame pen width to new pen width upon user button press
 * Four varying sizes: 1px, 2px, 4px, 8px
*/
void MainWindow::on_pxSize1_clicked()
{
    currentFrame.penSize = 1;
    selectedPen->setDown(false);
    ui->pxSize1->setDown(true);
    selectedPen = ui->pxSize1;
}
void MainWindow::on_pxSize2_clicked()
{
    currentFrame.penSize = 2;
    selectedPen->setDown(false);
    ui->pxSize2->setDown(true);
    selectedPen = ui->pxSize2;
}
void MainWindow::on_pxSize3_clicked()
{
    currentFrame.penSize = 4;
    selectedPen->setDown(false);
    ui->pxSize3->setDown(true);
    selectedPen = ui->pxSize3;
}
void MainWindow::on_pxSize4_clicked()
{
    currentFrame.penSize = 8;
    selectedPen->setDown(false);
    ui->pxSize4->setDown(true);
    selectedPen = ui->pxSize4;
}

/*
 * Delete current frame, updates displays, and calls removeFrames to remove current frame from frames and displayframes.
 * If current frame is at the beginning or the middle of frames vector, swaps frames w/ std::swap til we get to the end, then deletes the last frame
 * If current frame is at the end of frames vector, deletes last frame and current frame moves to previous frame
 * If current frame is the only frame, clear the frame, new file allows for resetting size
*/
void MainWindow::on_deleteFrame_clicked()
{
    if(frames.size() > 1)
    {
        ui->clearFrame->setEnabled(true);

        // If current frame is at the start or the middle, current frame is set to its following frame
        // QLayout doesn't have a nice swap function to recursively copy the pixel maps on the QLabels so that's done with std::swap
        if(currentFrame.ID < frames.size()-1)
        {
            for(int i = currentFrame.ID; i < frames.size()-1; i++)
            {
                if(i == currentFrame.ID)
                {
                    currentFrame = frames[i+1];
                    currentFrame.ID--;
                    ui->frameLabel->setPixmap(currentFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
                }
                // Swap frames and adjust IDs to reflect new frame index
                std::swap(frames[i], frames[i+1]);
                frames[i].ID--;
                std::swap(displayFrames[i], displayFrames[i+1]);
                displayFrames[i]->ID--;

                layout->insertWidget(i,displayFrames[i]);
            }
            displayFrames[currentFrame.ID]->setStyleSheet("border: 2px outset blue");

            // remove deleted frame from each vector
            // QLayout documentation states we must handle deletion
            removeFrame(frames.size()-1, displayFrames.size()-1, false);
        }
        else if (currentFrame.ID == frames.size()-1)
        {
            currentFrame = frames[frames.size()-2];

            ui->frameLabel->setPixmap(currentFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));

            removeFrame(frames.size()-1, displayFrames.size()-1, false);
            displayFrames[displayFrames.size()-1]->setStyleSheet("border: 2px outset blue");
            layout->setContentsMargins(10, 25, 10, 10);
        }
    }
    else if (frames.size() == 1)
    {
        ui->deleteFrame->setEnabled(false);
        ui->clearFrame->setEnabled(false);

        QColor color = Qt::white;
        Frame newFrame = Frame(pixelWidth, pixelHeight, &color);
        frames[0] = newFrame;
        currentFrame = newFrame;

        ui->frameLabel->setPixmap(newFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
        displayFrames[0]->setPixmap(newFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
    }
}

/*
 *  Helper method to remove a frame from frames, displayFrames, and layout
 */
void MainWindow::removeFrame(int framesIndex, int displayFramesIndex, bool clearVector)
{
    if(displayFrames[displayFramesIndex]){
        layout->removeWidget(displayFrames[displayFramesIndex]);
        delete displayFrames[displayFramesIndex];
        layout->setContentsMargins(10, 25, 10, 10);
    }

    if(clearVector)
    {
        frames.clear();
        displayFrames.clear();
    }
    else
    {
        frames.remove(framesIndex);
        displayFrames.remove(displayFramesIndex);
    }
}

/*
 * Clears current frame's pixmap
 * Disables delete frame if there is only one frame remaining
*/
void MainWindow::on_clearFrame_clicked()
{
    if(frames.size() == 1)
    {
        ui->deleteFrame->setEnabled(false);
    }
    ui->clearFrame->setEnabled(false);

    QColor color = Qt::white;
    Frame newFrame(pixelWidth, pixelHeight, &color);

    currentFrame.display = newFrame.display;
    currentFrame.pixels = newFrame.pixels;
    frames[currentFrame.ID].display = currentFrame.display;
    frames[currentFrame.ID].pixels = newFrame.pixels;

    ui->frameLabel->setPixmap(frames[currentFrame.ID].display.scaled(CANVASWIDTH, CANVASHEIGHT));
    displayFrames[currentFrame.ID]->setPixmap(newFrame.display.scaled(FRAMESIZE, FRAMESIZE));
    layout->setContentsMargins(10, 25, 10, 10);
}

/*
 * Sends coordinates to drawing functions
 * Records the starting point
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    shapeStartPoint = QPoint(event->pos().x(), event->pos().y() - ui->centralWidget->pos().y());
    if(frames.size() >= 1)
    {
        ui->clearFrame->setEnabled(true);
        ui->deleteFrame->setEnabled(true);
    }
    int refZeroX = ui->frameLabel->pos().x();
    int refZeroY = ui->frameLabel->pos().y();

    int mouseX = event->pos().x();
    int mouseY = event->pos().y() - ui->centralWidget->pos().y();
    previouslyDrawnPixel = QPoint(mouseX - refZeroX, mouseY - refZeroY);
    drawPixel(event->pos());
}

/*
 * Sends coordinates to drawing functions
 * Only occurs is mouse is being pressed down (i.e. user is drawing)
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    drawPixel(event->pos());

}

/*
 * Sends coordinates to drawing functions
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(frames.size() == 0)
    {
        return;
    }
    if(isDrawingRect || isDrawingCircle)
    {
        drawPixelsAfterRelease(event->pos());
    }
}

/*
 * Warning to display if sprite has not been saved
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(modified)
    {
        QMessageBox::StandardButton save;
        save = QMessageBox::warning(this, tr("Pixelator"),
                                    tr("Sprite has been modified. Do you want to save your changes?"),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if(save == QMessageBox::Save)
        {
            saveFile("ssp");
        }
        else if (save == QMessageBox::Cancel)
        {
            event->ignore();
        }
        else if (save == QMessageBox::Discard){
            event->accept();
        }
    }
    else
    {
        event->accept();
    }
}

/*
 * This will either draw a line between two points or record the starting point for a rectangle or ellipse
 */
void MainWindow::drawPixel(QPoint point)
{
    if(frames.size() == 0)
    {
        return;
    }
    int refZeroX = ui->frameLabel->pos().x();
    int refZeroY = ui->frameLabel->pos().y();

    int mouseX = point.x();
    int mouseY = point.y() - ui->centralWidget->pos().y();

    if(mouseX >= refZeroX && mouseX <= refZeroX  + CANVASWIDTH && mouseY >= refZeroY && mouseY <= refZeroY + CANVASHEIGHT)
    {

        if(isFreeDrawing)
        {
            int refX = (float)(mouseX - refZeroX) / (float)CANVASWIDTH * (float)pixelWidth;
            int refY = (float)(mouseY - refZeroY) / (float)CANVASHEIGHT * (float)pixelHeight;
            QPoint refPoint(refX, refY);

            int prevX = (float)previouslyDrawnPixel.x() / (float)CANVASWIDTH * (float)pixelWidth;
            int prevY = (float)previouslyDrawnPixel.y() / (float)CANVASHEIGHT * (float)pixelHeight;
            QPoint previousPoint(prevX,  prevY);

            currentFrame.drawLine(previousPoint, refPoint);
            this->updateFrameDisplay();
            modified = true;
            previouslyDrawnPixel = QPoint(mouseX - refZeroX, mouseY - refZeroY);
        }
        else if(isFilling)
        {
            int refX = (float)(mouseX - refZeroX) / (float)CANVASWIDTH * (float)pixelWidth;
            int refY = (float)(mouseY - refZeroY) / (float)CANVASHEIGHT * (float)pixelHeight;
            QPoint refPoint(refX, refY);
            this->currentFrame.penColor = currentColor;
            this->currentFrame.penSize = 1;
            currentFrame.fillColor(refPoint);
            this->updateFrameDisplay();
            modified = true;
        }
    }
}

/*
 * Used for drawing rectangles and circles, image will display after mouse release
 */
void MainWindow::drawPixelsAfterRelease(QPoint end)
{
    int refZeroX = ui->frameLabel->pos().x();
    int refZeroY = ui->frameLabel->pos().y();

    int mouseX = end.x();
    int mouseY = end.y() - ui->centralWidget->pos().y();

    if((mouseX >= refZeroX && mouseX <= refZeroX  + CANVASWIDTH && mouseY >= refZeroY && mouseY <= refZeroY + CANVASHEIGHT)
        && (shapeStartPoint.x() >= refZeroX && shapeStartPoint.x() <= refZeroX  + CANVASWIDTH && shapeStartPoint.y() >= refZeroY && shapeStartPoint.y() <= refZeroY + CANVASHEIGHT))
    {
        int refStartX = (float)(shapeStartPoint.x() - refZeroX) / (float)CANVASWIDTH * (float)pixelWidth;
        int refStartY = (float)(shapeStartPoint.y() - refZeroY) / (float)CANVASHEIGHT * (float)pixelHeight;
        QPoint refStartPoint(refStartX, refStartY);

        if(isDrawingRect)
        {
            int refEndX = (float)(mouseX - refZeroX) / (float)CANVASWIDTH * (float)pixelWidth;
            int refEndY = (float)(mouseY - refZeroY) / (float)CANVASHEIGHT * (float)pixelHeight;
            QPoint refEndPoint(refEndX, refEndY);

            currentFrame.drawRect(refStartPoint, refEndPoint);

            this->updateFrameDisplay();
            modified = true;
        }
        else if(isDrawingCircle)
        {
            int refEndX = (float)(mouseX - refZeroX) / (float)CANVASWIDTH * (float)pixelWidth;
            int refEndY = (float)(mouseY - refZeroY) / (float)CANVASHEIGHT * (float)pixelHeight;
            QPoint refEndPoint(refEndX, refEndY);
            currentFrame.drawCircle(refStartPoint, refEndPoint);

            this->updateFrameDisplay();
            modified = true;
        }

    }
}

/*
 *  Creates a new frame w/ white background, adds new frame to preview sequence, reassigns current frame
 */
void MainWindow::createFrame()
{
    QColor color = Qt::white;
    Frame newFrame(pixelWidth, pixelHeight, &color);
    newFrame.setPenColor(currentColor);
    newFrame.ID = frames.size();
    frames.push_back(newFrame);
    currentFrame = newFrame;

    ui->frameLabel->setPixmap(currentFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
    ui->frameLabel->update();

    if(frames.size() == 1)
    {
        playLivePreview();
    }
}

/*
 * Adds a new clickable label widget representing Frame parameter
 * Adds new widget to layout w/ blue border
 */
void MainWindow::addFrameDisplay(Frame& frame)
{
    ClickableLabel* label = new ClickableLabel;
    label->ID = frames.size() - 1;

    label->setFixedHeight(FRAMESIZE);
    label->setFixedWidth(FRAMESIZE);

    label->setPixmap(frame.display.scaled(FRAMESIZE, FRAMESIZE));

    QObject::connect(label, &ClickableLabel::clicked,
                     this, &MainWindow::onFrameThumbnailClicked);
    displayFrames.push_back(label);
    layout->addWidget(label);
    layout->setContentsMargins(10, 25, 10, 10);
    frameDisplayXValue += FRAMESIZE + 20;

    for(int i = 0; i < displayFrames.size(); i++)
    {
        displayFrames[i]->setStyleSheet("border: none");
    }
    displayFrames[label->ID]->setStyleSheet("border: 2px outset blue");
}

/*
 * Updates the frame icons at the bottom of the screen and the large drawing canvas
 */
void MainWindow::updateFrameDisplay()
{
    ui->frameLabel->setPixmap(currentFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
    frames[currentFrame.ID].display = currentFrame.display;
    displayFrames[currentFrame.ID]->setPixmap(currentFrame.display.scaled(FRAMESIZE, FRAMESIZE));
    frames[currentFrame.ID].pixels = currentFrame.pixels;
    repaint();
}

bool MainWindow::saveDialog()
{
    return saveFile("ssp");
}

/*
 * Sets up the text file that will be saved
 */
bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString path;
    if (fileLoaded)
    {
        path = currentFile;
    }
    else
    {
        path = QDir::currentPath() + "/" + currentFile + "." + fileFormat;
    }

    QString file = QFileDialog::getSaveFileName(this, tr("save"), path,
                               tr("%1 Files (*.%2);; All Files(*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if(file.isEmpty())
    {
        return false;
    }
    else
    {
        return saveSprite(file);
    }
}

/*
 * Sets up the initial text file header for saving
 */
bool MainWindow::saveSprite(const QString& fileName)
{
    QString name = fileName;
    QFile file(name);
    if(file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << initialSize << " " << initialSize << "\n"
               << frames.size() << "\n"
               << rgbaSave() << endl;
        modified = false;
        return true;
    }
    return false;
}

/*
 * Turns the backing vector of pixels into the correct format in the file to be saved
 */
QString MainWindow::rgbaSave()
{
    QString result;
    Frame frame;

    for(int i = 0; i < frames.size(); i++)
    {
        frame = frames[i];
        QImage image = frame.display.toImage();
        for(int x = 0; x < image.width(); x++)
        {
            for(int y = 0; y < image.height(); y++)
            {
                int pixIdx = x + pixelHeight * y;
                QColor color = image.pixelColor(x, y);
                int r,g,b,a;
                color.getRgb(&r, &g, &b, &a);
                frame.pixels[pixIdx] = std::tuple<int, int, int, int>(r, g, b, a);
            }
        }
        for (int y = 0; y < initialSize; y++)
        {
            for (int x = 0; x < initialSize; x++)
            {
                QString line;
                line = QString::number(std::get<0>(frame.pixels[x*initialSize+y])) + " " + QString::number(std::get<1>(frame.pixels[x*initialSize+y])) + " "
                     + QString::number(std::get<2>(frame.pixels[x*initialSize+y])) + " " + QString::number(std::get<3>(frame.pixels[x*initialSize+y])) + " ";
                result += line;
            }
            result += "\n";
        }
    }
    return result;
}

bool MainWindow::exportDialog()
{
    if(modified)
    {
        QMessageBox::StandardButton save;
        save = QMessageBox::warning(this, tr("Pixelator"),
                                    tr("Are you sure you want to export your Sprite to a .gif?"),
                                    QMessageBox::Yes | QMessageBox::Cancel);

        if(save == QMessageBox::Yes)
        {
            return exportFile("gif");
        }
        else if (save == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

/*
 * Exports the current project to a gif
 */
bool MainWindow::exportFile(const QByteArray &fileFormat)
{
    QString path;
    if (fileLoaded)
    {

        if(currentFile[currentFile.size()-4] == '.')
        {
            currentFile.chop(4);
            currentFile.append(".gif");
        }

        path = currentFile;
    }
    else
    {
        path = QDir::currentPath() + "/" + currentFile + "." + fileFormat;
    }

    QString file = QFileDialog::getSaveFileName(this, tr("save"), path,
                               tr("%1 Files (*.%2);; All Files(*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if(file.isEmpty())
    {
        return false;
    }
    else
    {
        return exportSprite(file);
    }
}

/*
 * Exports the drawn sprites using helper library
 */
bool MainWindow::exportSprite(const QString& fileName)
{

    //convert string to a byte array
    QString name = fileName;
    QByteArray data = name.toLocal8Bit();
    const char *file = data.data();

    uint32_t delay = 7;


    GifWriter writer;
    GifBegin(&writer, file, pixelWidth, pixelHeight, delay);

    //for every frame add all rgb values to array and send it into the magic gif frame writer
    Frame frame;
    for(int j = 0; j < frames.size(); j++)
    {
        frame = frames[j];
        QImage image = frame.display.toImage().rgbSwapped();

        image.convertToFormat(QImage::Format_RGBA8888);
        GifWriteFrame(&writer, image.bits(), image.width(), image.height(), delay);
    }

    GifEnd(&writer);

    QMessageBox::information(NULL, "Done!", QString("GIF Image has been made!"));
    return true;
}

/*
 * Loads ssp file into client
 * File format:
 * height width \n
 * frame # \n
 * rgba, rgba, ... \n
 */
void MainWindow::loadFromFile()
{
    // Add dummy frame to be overwritten, also enables buttons
    if(frames.size() == 0)
    {
        on_setSize_clicked();
    }
    else
    {
        // Delete frames until there is 1 remaining to overwrite
        while(frames.size() > 1)
        {
            removeFrame(1, 1, false);
        }
    }

    currentFile = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Image Files (*.ssp);;All Files (*)"));
    if(currentFile.isEmpty())
        return;
    else
    {
        fileLoaded = true;
        QFile file(currentFile);

        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file."), file.errorString());
            return;
        }

        QTextStream in(&file);
        QVector<QString> fileData;
        QVector <QString> individualData;
        while(!in.atEnd())
        {
            QString line = in.readLine();
            fileData.push_back(line);
            QString data;
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] == " ")
                {
                    individualData.push_back(data);
                    data.clear();
                }
                else if (i == line.length()-1)
                {
                    data += line[i];
                    individualData.push_back(data);
                    data.clear();
                }
                else
                {
                    data += line[i];
                }
            }
        }
        // Sprite dimensions must be square
        if(individualData[0] != individualData[1])
        {
            QMessageBox invalidSize;
            invalidSize.setText("The size of this image in incompatible with the program. \n"
                                "Please select a file with a square dimension.");
            invalidSize.exec();
        }
        else
        {
            pixelHeight = individualData[0].toInt();
            pixelWidth = individualData[1].toInt();
            ui->sizeDropBox->setCurrentText((QString) pixelHeight);
            initialSize = pixelWidth;
            int numFrames = individualData[2].toInt();
            individualData.remove(0,3);
            int numPixelsPerFrame = individualData.size()/numFrames;
            QColor color = Qt::white;
            Frame newFrame(pixelHeight, pixelWidth, &color);

            for(int size = 0; size < numFrames; size++)
            {
                newFrame.ID = size;

                std::tuple<int, int, int, int> pixels;
                QVector<std::tuple<int, int, int, int>> pixVec;
                for(int framePixels = size*numPixelsPerFrame; framePixels < individualData.size(); framePixels+=4)
                {
                    pixels = std::tuple<int, int, int, int>(individualData[framePixels].toInt(), individualData[(framePixels + 1)].toInt(), individualData[(framePixels + 2)].toInt(), individualData[(framePixels + 3)].toInt());
                    pixVec.push_back(pixels);
                }
                if(size == 0)
                {
                    frames[0] = newFrame;
                    frames[0].drawFromVector(pixVec);
                    currentFrame = frames[0];
                    updateFrameDisplay();
                }
                else
                {
                    frames.push_back(newFrame);
                    frames[size].drawFromVector(pixVec);
                    addFrameDisplay(frames[size]);
                }
            }
            displayFrames[displayFrames.size()-1]->setStyleSheet("border: none");
            displayFrames[0]->setStyleSheet("border: 2px outset blue");
        }
    }
}

/*
 * Create a new project
 * Deletes all frames and resets ui variables to default values
 * Allows user to reset size and begin editing brand new sprite
 */
void MainWindow::createNew()
{
    if(frames.size() >= 1)
    {
        modified = false;
        currentColor = Qt::black;
        currentFrame.penColor = Qt::black;
        ui->colorDisplay->setStyleSheet("QLabel{background-color: black}");
        frameRate = 32;
        ui->frameRateSlider->setValue(32);
        previewIndex = 0;

        while(frames.size() > 1)
        {
            onFrameThumbnailClicked(0);
            on_deleteFrame_clicked();
        }
        currentFrame = Frame();
        ui->frameLabel->setPixmap(currentFrame.display.scaled(CANVASWIDTH, CANVASHEIGHT));
        ui->previewLabel->setPixmap(currentFrame.display.scaled(120, 120));

        QPixmap pixmap(ui->colorDisplay->width(), ui->colorDisplay->height());
        pixmap.fill(Qt::black);
        ui->colorDisplay->setPixmap(pixmap);
        currentFile = "untitled.ssp";
        repaint();

        removeFrame(0,0,true);

        // Set label default displays
        setAllUIButtonsEnabled(false);
        ui->sizeDropBox->setEnabled(true);
        ui->setSize->setEnabled(true);
        ui->setSize->setVisible(true);
    }
}

/*
 *  Plays a live preview in the bottom right corner of the GUI given QVector<Frame> frames
 */
void MainWindow::playLivePreview()
{
    if(actualSize)
    {
        if(frames.size() >= 1)
        {
            int height = currentFrame.display.height();
            int width = currentFrame.display.width();

            int x = ui->previewView->width()/2 - width/2 + ui->previewView->pos().x();
            int y = ui->previewView->height()/2 - height/2 + ui->previewView->pos().y()-10;

            ui->previewLabel->setFixedSize(currentFrame.display.size());
            ui->previewLabel->setPixmap(frames[previewIndex%frames.size()].display.scaled(height, width));
            ui->previewLabel->setGeometry(x, y, width, height);
            previewIndex++;
        }
        QTimer::singleShot((64-frameRate)*10, this, &MainWindow::playLivePreview);
    }
    else
    {
        if(frames.size() >= 1)
        {
            ui->previewLabel->setFixedSize(120, 120);
            ui->previewLabel->setGeometry(510, 530, 120, 120);
            ui->previewLabel->setPixmap(frames[previewIndex%frames.size()].display.scaled(120, 120));

            previewIndex++;
        }
        QTimer::singleShot((64-frameRate)*10, this, &MainWindow::playLivePreview);

    }
}

/*
 *  Toggles between actual size of gif and relative size
 */
void MainWindow::on_actualSize_clicked()
{
    actualSize = !actualSize;
    ui->actualSize->setDown(actualSize);
}


/*
 *  Enable/disable all buttons for user input
 */
void MainWindow::setAllUIButtonsEnabled(bool enabled)
{
    ui->addFrame->setEnabled(enabled);
    ui->deleteFrame->setEnabled(enabled);
    ui->clearFrame->setEnabled(enabled);
    ui->pxSize1->setEnabled(enabled);
    ui->pxSize2->setEnabled(enabled);
    ui->pxSize3->setEnabled(enabled);
    ui->pxSize4->setEnabled(enabled);
    ui->frameRateSlider->setEnabled(enabled);
    ui->pencil->setEnabled(enabled);
    ui->paintBucket->setEnabled(enabled);
    ui->eraser->setEnabled(enabled);
    ui->square->setEnabled(enabled);
    ui->circle->setEnabled(enabled);
    ui->colorDisplay->setEnabled(enabled);
    ui->black->setEnabled(enabled);
    ui->white->setEnabled(enabled);
    ui->gray->setEnabled(enabled);
    ui->blue->setEnabled(enabled);
    ui->yellow->setEnabled(enabled);
    ui->green->setEnabled(enabled);
    ui->magenta->setEnabled(enabled);
    ui->red->setEnabled(enabled);
    ui->actualSize->setEnabled(enabled);
}

/*
 * This function sets up the help label when buttons are hovered over
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // This function repeatedly call for those QObjects
    // which have installed eventFilter (Step 2)
    if (event->type() == QEvent::Leave)
    {
        ui->helpLabel->setText("");
        return QWidget::eventFilter(obj, event);
    }
    else if (event->type() == QEvent::Enter)
    {
        if (obj == (QObject*)ui->green)
        {
            ui->helpLabel->setText("Set drawing color to green.");
        }
        else if (obj == (QObject*)ui->black)
        {
            ui->helpLabel->setText("Set drawing color to black.");
        }
        else if (obj == (QObject*)ui->red)
        {
            ui->helpLabel->setText("Set drawing color to red.");
        }
        else if (obj == (QObject*)ui->gray)
        {
            ui->helpLabel->setText("Set drawing color to gray.");
        }
        else if (obj == (QObject*)ui->yellow)
        {
            ui->helpLabel->setText("Set drawing color to yellow.");
        }
        else if (obj == (QObject*)ui->magenta)
        {
            ui->helpLabel->setText("Set drawing color to magenta.");
        }
        else if (obj == (QObject*)ui->white)
        {
            ui->helpLabel->setText("Set drawing color to white.");
        }
        else if (obj == (QObject*)ui->blue)
        {
            ui->helpLabel->setText("Set drawing color to blue.");
        }
        else if (obj == (QObject*)ui->square)
        {
            ui->helpLabel->setText("Click and drag on the canvas to draw a rectangle.");
        }
        else if (obj == (QObject*)ui->circle)
        {
            ui->helpLabel->setText("Click and drag on the canvas to draw an ellipse.");
        }
        else if (obj == (QObject*)ui->paintBucket)
        {
            ui->helpLabel->setText("Click on a pixel to change all pixels of that color to the selected color of the paint bucket.");
        }
        else if (obj == (QObject*)ui->eraser)
        {
            ui->helpLabel->setText("Click and drag to clear pixels using the selected pen width.");
        }
        else if (obj == (QObject*)ui->pencil)
        {
            ui->helpLabel->setText("Click and drag to draw pixels using the selected pen width and color.");
        }
        else if (obj == (QObject*)ui->pxSize1)
        {
            ui->helpLabel->setText("Sets the drawing size to one pixel.");
        }
        else if (obj == (QObject*)ui->pxSize2)
        {
            ui->helpLabel->setText("Sets the drawing size to two pixels.");
        }
        else if (obj == (QObject*)ui->pxSize3)
        {
            ui->helpLabel->setText("Sets the drawing size to four pixels.");
        }
        else if (obj == (QObject*)ui->pxSize4)
        {
            ui->helpLabel->setText("Sets the drawing size to eight pixels.");
        }
        else if (obj == (QObject*)ui->addFrame)
        {
            ui->helpLabel->setText("Add a frame to the sprite. This will set up a fresh canvas and set that as the current frame.");
        }
        else if (obj == (QObject*)ui->deleteFrame)
        {
            ui->helpLabel->setText("Delete the current frame from the sprite. The current frame will be adjusted to one next to it.");
        }
        else if (obj == (QObject*)ui->clearFrame)
        {
            ui->helpLabel->setText("Clears all pixels on the currently selected frame for an empty canvas.");
        }
        else if (obj == (QObject*)ui->setSize)
        {
            ui->helpLabel->setText("Sets the dimensions of the sprite in number of pixels with what is displayed in the dropdown.");
        }
        else if (obj == (QObject*)ui->actualSize)
        {
            ui->helpLabel->setText("Toggles preview between relative size and actual size.");
        }
        return QWidget::eventFilter(obj, event);
    }
    else
    {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}
