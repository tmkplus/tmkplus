#ifndef QT5PAINT_H
#define QT5PAINT_H

#include <QList>
#include <QMenu>
#ifdef PAINTER_IS_A_MAINWINDOW
#include <QMainWindow>
#else
#include <QDialog>
#endif

class ScribbleArea;

//! [0]
#ifdef PAINTER_IS_A_MAINWINDOW
class Qt5Paint : public QMainWindow
#else
class Qt5Paint : public QDialog
#endif
{
    Q_OBJECT

public:
    Qt5Paint(QString openPath, QString savePath, bool showSaveChanges);
    QString ftopen;
    QString ftsave;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void open();
    void onDrawTextClicked();
    void onSizeTextClicked();
    void save();
    void penColor();
    void penWidth();
    void about();

private:
    bool stillStarting;
    void WaitSecs(int seconds, bool useMilliseconds);
    void createActions();
    void createMenus(bool showSaveChanges);
    bool maybeSave();
    bool doShowSaveChanges;
    bool saveFile(const QByteArray &fileFormat);

    ScribbleArea *scribbleArea;

    QMenu *saveAsMenu;
    QMenu *fileMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;

    QAction *openAct;
    QList<QAction *> saveAsActs;
    QAction *exitAct;
    QAction *sizeTextAct;
    QAction *penColorAct;
    QAction *penWidthAct;
    QAction *printAct;
    QAction *clearScreenAct;
    QAction *drawTextAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};
//! [0]

#endif
