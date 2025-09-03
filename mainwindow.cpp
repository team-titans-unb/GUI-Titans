#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "robotselectiondialog.h"
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

    //cronometro
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

}

MainWindow::~MainWindow()
{
    
}

void MainWindow::setupInitialState()
{
    m_robotsLayout = new QGridLayout();
    QVBoxLayout* containerRobotsLayout = new QVBoxLayout(ui->robotsContainerWidget);
    QLabel* robotsTitleLabel = new QLabel("ROBÔS:");
    robotsTitleLabel->setStyleSheet(
        "font-weight: bold;"
        "font-size: 14px;"
        "margin-bottom: 10px;"
    );
    robotsTitleLabel->setAlignment(Qt::AlignLeft);

    containerRobotsLayout->addWidget(robotsTitleLabel);
    containerRobotsLayout->addLayout(m_robotsLayout);
    containerRobotsLayout->addStretch();

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
        startTimer();
        QApplication::processEvents();

        // startCommunication();
        m_playing = true;
    } else {
        setButtonToPlay();
        timer->stop();
        QApplication::processEvents();


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

void MainWindow::startTimer()
{
    screenTime.setHMS(0,0,0);
    QString stringScreenTime = screenTime.toString("mm:ss");
    ui->LabelCronometro->setText(stringScreenTime);

    timer->start(1000);
}

void MainWindow::updateTimer(){

    screenTime = screenTime.addSecs(1);

    QString stringScreenTime = screenTime.toString("mm:ss");
    ui->LabelCronometro->setText(stringScreenTime);

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
        RobotWidget* widget = m_robotWidgets[i];
        const RobotData& updateRobotData = robots[i];

        RobotData dataForDisplay = updateRobotData;
        dataForDisplay.name = widget->getCurrentData().name;

        widget->updateData(dataForDisplay);
    }
}

void MainWindow::loadCategory(const QString& category)
{
    clearRobotLayout(); 

    if (m_configManager->load(category)) {
        QList<RobotData>& robots = m_configManager->getRobots();
        const QString startupColor = "blue";
        for (RobotData &robotData : robots) {
            robotData.teamColor = startupColor;
        }

        int row = 0, col = 0;
        for (const RobotData &robotData : robots) {
            RobotWidget *widget = new RobotWidget();
            widget->updateData(robotData);

            connect(widget, &RobotWidget::idChanged, this, &MainWindow::onRobotWidgetClicked);
            connect(widget, &RobotWidget::roleChanged, this, &MainWindow::onRobotRoleChanged);

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
void MainWindow::on_ButtonLado_clicked()
{
    if (ui->ButtonLado->text() == "ESQUERDO") {
        ui->ButtonLado->setText("DIREITO");
    } else {
        ui->ButtonLado->setText("ESQUERDO");
    }
}



void MainWindow::onRobotWidgetClicked(const RobotData& currentRobotData)
{
    QSet<int> displayedRobotIDs;
    for (const RobotWidget* widget : m_robotWidgets) {
        displayedRobotIDs.insert(widget->getCurrentData().id);
    }

    const QList<RobotData>& allRobots = m_configManager->getAllRobots(currentRobotData.category, currentRobotData.teamColor);
    QList<RobotData> availableRobots;
    for (const RobotData& robot : allRobots) {
        if (!displayedRobotIDs.contains(robot.id)) {
            availableRobots.append(robot);
        }
    }

    if (availableRobots.isEmpty()) {
        qDebug() << "Nenhum robô disponível para troca.";
        return;
    }

    RobotSelectionDialog dialog(availableRobots, this);
    if (dialog.exec() == QDialog::Accepted) {
        RobotData newRobotData = dialog.getSelectedRobot();

        RobotWidget* widgetToUpdate = nullptr;
        for (RobotWidget* widget : m_robotWidgets) {
            if (widget->getCurrentData().id == currentRobotData.id) {
                widgetToUpdate = widget;
                break;
            }
        }
        if (!widgetToUpdate) {
            qWarning() << "Widget do robô atual não encontrado!";
            return;
        }

        RobotData dataForDisplay = newRobotData;
        dataForDisplay.name = widgetToUpdate->getCurrentData().name;

        QList<RobotData>& robotsInConfig = m_configManager->getRobots();
        int indexOfRobotToChange = -1;
        for(int i = 0; i < robotsInConfig.size(); ++i) {
            if (robotsInConfig[i].id == currentRobotData.id) {
                indexOfRobotToChange = i;
                break;
            }
        }

        if (indexOfRobotToChange != -1) {
            RobotData& slotToModify = robotsInConfig[indexOfRobotToChange];
            slotToModify.id = newRobotData.id;
        }

        widgetToUpdate->updateData(dataForDisplay);
        
        m_configManager->save(currentRobotData.category);
    }
}

void MainWindow::onRobotRoleChanged(const RobotData& updatedData)
{
    QList<RobotData>& robots = m_configManager->getRobots();

    for (RobotData& robot : robots) {
        if (robot.id == updatedData.id) {
            robot.role = updatedData.role;
            break;
        }
    }

    QString currentCategory = ui->ButtonCategoria->text();
    m_configManager->save(currentCategory);
}