#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "robotselectiondialog.h"
#include "configmanager.h"
#include "robotwidget.h"
#include <QGridLayout>
#include <QSettings>
#include <QApplication>
#include <QIcon>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDebug>
#include <QDir>

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

        // ===========================================
        // Atualização dos robôs e variáveis necessárias
        // ===========================================

        // 1. Coletar os dados atuais
        QString category = ui->ButtonCategoria->text();
        QString color = m_isButtonColorBlue ? "blue" : "yellow";
        QString side = (ui->ButtonLado->text() == "ESQUERDO") ? "left" : "right";
        QList<RobotData>& robots = m_configManager->getRobots();

        // 2. Transformar os dados dos robôs em JSON
        QJsonArray robotsArray;
        for (const RobotData& robot : robots) {
            QJsonObject robotObj;
            robotObj["name"] = robot.name;
            robotObj["id"] = robot.id;
            robotObj["role"] = robot.role == RobotRole::Attacker ? "attacker" :
                               robot.role == RobotRole::Goalkeeper ? "goalkeeper" :
                               robot.role == RobotRole::Defender ? "defender" : "unknown";
            robotsArray.append(robotObj);
        }

        // 3. Converte JSON para uma string compacta
        QJsonDocument doc(robotsArray);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        //qDebug() << "Dados dos robôs em JSON:" << jsonString;
        
        // 4. Montar e executar o comando QProcess
        QProcess *process = new QProcess(this);
        QString program = "/bin/python3";
        QString appDir = QCoreApplication::applicationDirPath();
        QDir dir(appDir);

        if (dir.cd("../..")) {
            QString projectRootPath = dir.path();
            // qDebug() << "Caminho raiz do projeto:" << projectRootPath;
        } else {
            qDebug() << "Erro: Não foi possível voltar 2 diretórios.";
        }
        QString scriptPath = dir.path() + "/scripts/update_config.py";
        QStringList arguments;
        arguments << scriptPath 
                  << "--category" << category
                  << "--color" << color
                  << "--side" << side
                  << "--robot-data" << jsonString;
        
        qDebug() << "Executando comando de atualização:" << program << arguments;
        process->start(program, arguments);
        process->waitForFinished(-1);

        // ===========================================
        // Captura e exibe a saída do script
        // ===========================================
        // QByteArray stdOut = process->readAllStandardOutput();
        // QByteArray stdErr = process->readAllStandardError();

        // if (!stdOut.isEmpty()) {
        //     qDebug() << "Saida do Script (stdout):" << QString::fromUtf8(stdOut);
        //     appendLogMessage("Script: " + QString::fromUtf8(stdOut));
        // }
        // if (!stdErr.isEmpty()) {
        //     qDebug() << "Erros do Script (stderr):" << QString::fromUtf8(stdErr);
        //     appendLogMessage("Script ERRO: " + QString::fromUtf8(stdErr));
        // }
        // ===================================================================

        qDebug() << "Script finalizado com o exit code:" << process->exitCode();

        process->deleteLater();

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
    ui->ButtonCategoria->setEnabled(true);
    ui->ButtonCor->setEnabled(true);
    ui->ButtonLado->setEnabled(true);
    ui->ButtonLimpar->setEnabled(true);
    for (RobotWidget* widget : m_robotWidgets) {
        widget->setEnabled(true);
    }
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
    ui->ButtonCategoria->setEnabled(false);
    ui->ButtonCor->setEnabled(false);
    ui->ButtonLado->setEnabled(false);
    ui->ButtonLimpar->setEnabled(false);
    for (RobotWidget* widget : m_robotWidgets) {
        widget->setEnabled(false);
    }

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

void MainWindow::on_ButtonLimpar_clicked()
{
    int index = ui->tabWidget->currentIndex(); // atual

    switch (index) {
    case 0: // logs
        ui->logsTextEdit->clear();
        break;
    case 1: // visao
        ui->visaoTextEdit->clear();
        break;
    case 2: // controle
        ui->controleTextEdit->clear();
        break;
    default:
        break;
    }
}

void MainWindow::appendLogMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logsTextEdit->appendPlainText("[" + timestamp + "] " + message);
}

void MainWindow::appendVisaoMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->visaoTextEdit->appendPlainText("[" + timestamp + "] " + message);
}

void MainWindow::appendControleMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->controleTextEdit->appendPlainText("[" + timestamp + "] " + message);
}
