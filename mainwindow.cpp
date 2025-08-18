#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "configmanager.h"
#include "robotwidget.h"
#include <QGridLayout>
#include <QSettings>
#include <QApplication>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupInitialState();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::setupInitialState()
{
    m_robotsLayout = new QGridLayout(ui->robotsContainerWidget); 
    m_configManager.reset(new ConfigManager("mockdata.json"));

    QSettings settings("Titans", "UIState");
    QString lastCategory = settings.value("lastCategory", "VSSS").toString();
    m_isButtonColorBlue = settings.value("isButtonColorBlue", true).toBool();

    ui->ButtonCategoria->setText(lastCategory);
    if (m_isButtonColorBlue) {
        ui->ButtonCor->setStyleSheet("background-color: #00bbff;");
        ui->ButtonCor->setText("Azul");
    } else {
        ui->ButtonCor->setStyleSheet("background-color: yellow;");
        ui->ButtonCor->setText("Amarelo");
    }

    setButtonToPlay();
    loadCategory(lastCategory);
}

void MainWindow::on_ButtonCategoria_clicked()
{
    QString novoTexto = (ui->ButtonCategoria->text() == "VSSS") ? "SSL" : "VSSS";
    ui->ButtonCategoria->setText(novoTexto);

    QSettings settings("Titans", "UIState");
    settings.setValue("lastCategory", novoTexto);
    
    loadCategory(novoTexto);
}

void MainWindow::on_ButtonPlayPause_clicked()
{
    if (!m_playing) {
        setButtonToPause();
        // startCommunication();
        m_playing = true;
    } else {
        setButtonToPlay();
        // stopCommunication();
        m_playing = false;
    }
}

void MainWindow::setButtonToPlay()
{
    ui->ButtonPlayPause->setText("");
    ui->ButtonPlayPause->setIcon(QIcon(":/icons/play.svg"));
    ui->ButtonPlayPause->setIconSize(QSize(32, 32));
    ui->ButtonPlayPause->setStyleSheet(
        "QPushButton { background:#4CAF50; border-radius:6px; padding:10px; }"
        "QPushButton:pressed { background:#45A049; }"
        );
}

void MainWindow::setButtonToPause()
{
    ui->ButtonPlayPause->setText("");
    ui->ButtonPlayPause->setIcon(QIcon(":/icons/pause.svg"));
    ui->ButtonPlayPause->setIconSize(QSize(32, 32));
    ui->ButtonPlayPause->setStyleSheet(
        "QPushButton { background:#F44336; border-radius:6px; padding:10px; }"
        "QPushButton:pressed { background:#D32F2F; }"
        );
}

void MainWindow::on_ButtonCor_clicked()
{
    QString newColorName;
    
    if (m_isButtonColorBlue) {        
        ui->ButtonCor->setStyleSheet("background-color: yellow;");
        ui->ButtonCor->setText("Amarelo");
        newColorName = "yellow";
        m_isButtonColorBlue = false;
    } else {
        ui->ButtonCor->setStyleSheet("background-color: #00bbff;");
        ui->ButtonCor->setText("Azul");
        newColorName = "blue";
        m_isButtonColorBlue = true;
    }

    QList<RobotData>& robots = m_configManager->getRobots();
    for (RobotData& robot : robots) {
        robot.teamColor = newColorName;
    }
    
    QString currentCategory = ui->ButtonCategoria->text();
    m_configManager->save(currentCategory);

    QSettings settings("Titans", "UIState");
    settings.setValue("isTeamColorBlue", m_isButtonColorBlue);
    
    for(int i = 0; i < robots.size(); ++i) {
        m_robotWidgets[i]->updateData(robots[i]);
    }
}

void MainWindow::loadCategory(const QString& category)
{
    clearRobotLayout(); 

    if (m_configManager->load(category)) {
        const QList<RobotData>& robots = m_configManager->getRobots();

        int row = 0, col = 0;
        for (const RobotData &robotData : robots) {
            RobotWidget *widget = new RobotWidget();
            widget->updateData(robotData);
            
            m_robotWidgets.append(widget);
            m_robotsLayout->addWidget(widget, row, col++);
            if (col >= 2) {
                col = 0;
                row++;
            }
        }
    }
}

void MainWindow::clearRobotLayout()
{
    for (RobotWidget* widget : m_robotWidgets) {
        m_robotsLayout->removeWidget(widget);
        widget->deleteLater();
    }
    m_robotWidgets.clear();
}