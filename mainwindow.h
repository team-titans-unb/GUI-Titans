#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    bool playing = false;

    void setButtonToPlay();
    void setButtonToPause();
};

#endif // MAINWINDOW_H
