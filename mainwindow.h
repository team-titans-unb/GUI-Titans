#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QList>
#include "robotdata.h"

#include <QUdpSocket>
#include <QPainter>

#include "proto/wrapper.pb.h"
#include "proto/messages_robocup_ssl_detection.pb.h"
#include "proto/messages_robocup_ssl_geometry.pb.h"

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
    void appendLogMessage(const QString &message);
    void appendVisaoMessage(const QString &message);
    void appendControleMessage(const QString &message);

private slots:
    void on_ButtonCategoria_clicked();
    void on_ButtonPlayPause_clicked();
    void on_ButtonCor_clicked();
    void onRobotWidgetClicked(const RobotData& currentRobotData);
    void onRobotRoleChanged(const RobotData& updatedData);

    void on_ButtonLado_clicked();

    void startTimer();
    void updateTimer();
    void on_ButtonLimpar_clicked();

    void readPendingDatagrams(); // Função que será chamada quando chegarem dados

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

    void initVisionListener();   // Função para configurar o "ouvinte" de rede

    void drawFieldState(const SSL_DetectionFrame& detection, const SSL_GeometryData& geometry);
    // variável para o socket UDP (o "ouvido" da rede)
    QUdpSocket *m_udpSocket = nullptr;

};

#endif
