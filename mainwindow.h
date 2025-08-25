#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ButtonCategoria_clicked();
    void on_ButtonPlayPause_clicked();
    void on_ButtonCor_clicked();

    void on_ButtonLado_clicked();

    void startTimer();
    void updateTimer();

private:
    Ui::MainWindow *ui;
    bool playing = false;
    bool isButtonColorBlue = true;

    void setButtonToPlay();
    void setButtonToPause();

    QTimer *timer;
    QTime screenTime;
};

#endif // MAINWINDOW_H
