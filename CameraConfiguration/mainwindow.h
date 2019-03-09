#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QCheckBox *cbMirror;
    QCheckBox *cbTrace;
    QCheckBox *cbBlur;
    QCheckBox *cbDebug;
    QCheckBox *cbTransparent;

    QLineEdit *leOpacity;
    QLineEdit *leDelay;
    QLineEdit *leLeft;
    QLineEdit *leTop;
    QLineEdit *leWidth;
    QLineEdit *leHeight;
    QPushButton *pbYes;
    QPushButton *pbReload;
};

#endif // MAINWINDOW_H
