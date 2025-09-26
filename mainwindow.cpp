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
#include <cmath>

#include <QNetworkDatagram>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupInitialState();

    initVisionListener();

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


// função que configura a conexão
void MainWindow::initVisionListener() {
    m_udpSocket = new QUdpSocket(this);

    const QHostAddress multicastAddress = QHostAddress("224.5.23.2");
    const quint16 port = 10020;

    if (!m_udpSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)) {
        qWarning() << "Falha ao fazer bind na porta UDP:" << port;
        return;
    }

    if (!m_udpSocket->joinMulticastGroup(multicastAddress)) {
        qWarning() << "Falha ao entrar no grupo multicast:" << multicastAddress.toString();
        return;
    }

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

    qDebug() << "Ouvindo pacotes de visão em" << multicastAddress.toString() << ":" << port;
    ui->fieldLabel->setText("Aguardando pacotes do vss-vision...");
}


// função que é chamada quando os dados chegam
void MainWindow::readPendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());

        SSL_WrapperPacket packet;
        if (packet.ParseFromArray(datagram.data(), datagram.size())) {
            if (packet.has_detection() && packet.has_geometry()) {
                // Chame a função passando os dois argumentos
                drawFieldState(packet.detection(), packet.geometry());
            }
        }
    }
}


//  função que desenha o campo na tela
void MainWindow::drawFieldState(const SSL_DetectionFrame& detection, const SSL_GeometryData& geometry) {
    QPixmap fieldPixmap(ui->fieldLabel->size());
    fieldPixmap.fill(QColor("#006400"));
    QPainter painter(&fieldPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const SSL_GeometryFieldSize& field = geometry.field();
    const float fieldLength = field.field_length();
    const float fieldWidth = field.field_width();
    const float boundaryWidth = field.boundary_width();
    const float goalWidth = field.goal_width();
    const float goalDepth = field.goal_depth();
    const float penaltyAreaWidth = field.penalty_area_width();
    const float penaltyAreaDepth = field.penalty_area_depth();
    const float centerCircleRadius = 200.0;

    // cálculo da escala invertida para um campo vertical
    const float scaleX = (float)ui->fieldLabel->width() / (fieldWidth + boundaryWidth * 2);
    const float scaleY = (float)ui->fieldLabel->height() / (fieldLength + boundaryWidth * 2);
    const float scale = qMin(scaleX, scaleY) * 0.95;

    painter.translate(ui->fieldLabel->width() / 2.0, ui->fieldLabel->height() / 2.0);

    painter.setPen(QPen(Qt::white, 2));

    // coordenadas X e Y invertida em todos os desenhos
    // 1. Linhas de Contorno
    painter.drawRect(QRectF(-fieldWidth / 2 * scale, -fieldLength / 2 * scale, fieldWidth * scale, fieldLength * scale));

    // Linha do Meio de Campo
    painter.drawLine(QPointF(-fieldWidth / 2 * scale, 0), QPointF(fieldWidth / 2 * scale, 0));

    // Círculo Central
    painter.drawEllipse(QPointF(0, 0), centerCircleRadius * scale, centerCircleRadius * scale);

    // Área de Pênalti (Cima, "Esquerda" no campo horizontal)
    painter.drawRect(QRectF(-penaltyAreaWidth / 2 * scale, (-fieldLength / 2) * scale, penaltyAreaWidth * scale, penaltyAreaDepth * scale));

    // Área de Pênalti (Baixo, "Direita" no campo horizontal)
    painter.drawRect(QRectF(-penaltyAreaWidth / 2 * scale, (fieldLength / 2 - penaltyAreaDepth) * scale, penaltyAreaWidth * scale, penaltyAreaDepth * scale));

    painter.setPen(QPen(Qt::white, 4));
    // Gol (Cima)
    painter.drawRect(QRectF(-goalWidth / 2 * scale, (-fieldLength / 2 - goalDepth) * scale, goalWidth * scale, goalDepth * scale));

    // Gol (Baixo)
    painter.drawRect(QRectF(-goalWidth / 2 * scale, fieldLength / 2 * scale, goalWidth * scale, goalDepth * scale));

    // Bola
    if (detection.balls_size() > 0) {
        const SSL_DetectionBall& ball = detection.balls(0);
        painter.setBrush(QColor("orange"));
        painter.setPen(Qt::NoPen);
        // Coordenadas invertidas: (y, x)
        painter.drawEllipse(QPointF(ball.y() * scale, ball.x() * scale), 22 * scale, 22 * scale);
    }

    // Robôs Azuis
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::black);
    for (const SSL_DetectionRobot& robot : detection.robots_blue()) {
        painter.save();
        // Coordenadas invertidas: (y, x)
        painter.translate(robot.y() * scale, robot.x() * scale);
        // MUDANÇA 3: Ajustamos a orientação em -90 graus
        painter.rotate(qRadiansToDegrees(robot.orientation() - M_PI / 2.0));

        float robotSize = 150 * scale;
        painter.drawRect(QRectF(-robotSize / 2, -robotSize / 2, robotSize, robotSize));
        painter.setBrush(Qt::white);
        painter.drawRect(QRectF(robotSize / 6, -robotSize / 4, robotSize / 3, robotSize / 2));
        painter.setBrush(Qt::blue);
        painter.restore();
    }

    // Robôs Amarelos
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::black);
    for (const SSL_DetectionRobot& robot : detection.robots_yellow()) {
        painter.save();
        // Coordenadas invertidas: (y, x)
        painter.translate(robot.y() * scale, robot.x() * scale);
        // MUDANÇA 3: Ajustamos a orientação em -90 graus
        painter.rotate(qRadiansToDegrees(robot.orientation() - M_PI / 2.0));
        float robotSize = 150 * scale;
        painter.drawRect(QRectF(-robotSize / 2, -robotSize / 2, robotSize, robotSize));
        painter.setBrush(Qt::black);
        painter.drawRect(QRectF(robotSize / 6, -robotSize / 4, robotSize / 3, robotSize / 2));
        painter.setBrush(Qt::yellow);
        painter.restore();
    }

    painter.end();
    ui->fieldLabel->setPixmap(fieldPixmap);
}
