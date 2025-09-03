#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QList>
#include "robotdata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ConfigManager;
class RobotWidget;
class QGridLayout;

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
    void onRobotWidgetClicked(const RobotData& currentRobotData);
    void onRobotRoleChanged(const RobotData& updatedData);

    void on_ButtonLado_clicked();

    void startTimer();
    void updateTimer();

private:
    void setupInitialState();
    void loadCategory(const QString& category);
    void clearRobotLayout();

    QScopedPointer<Ui::MainWindow> ui;
    QScopedPointer<ConfigManager> m_configManager;

    bool m_playing = false;
    bool m_isButtonColorBlue = true;

    QGridLayout* m_robotsLayout = nullptr;
    QList<RobotWidget*> m_robotWidgets;

    void setButtonToPlay();
    void setButtonToPause();

    QTimer *timer;
    QTime screenTime;
};

#endif // MAINWINDOW_H
