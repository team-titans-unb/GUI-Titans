#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSettings>
#include <QApplication>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Restaura o texto da categoria
    QSettings settings("Titans", "Categoria");
    QString ultimoTexto = settings.value("ultimoTextoCategoria", "VSSS").toString();
    ui->ButtonCategoria->setText(ultimoTexto);

    // Inicializa botão como PLAY
    setButtonToPlay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ButtonCategoria_clicked()
{
    QString novoTexto;
    if (ui->ButtonCategoria->text() == "VSSS") {
        novoTexto = "SSL";
    } else {
        novoTexto = "VSSS";
    }
    ui->ButtonCategoria->setText(novoTexto);

    QSettings settings("Titans", "Categoria");
    settings.setValue("ultimoTextoCategoria", novoTexto);
}

void MainWindow::on_ButtonPlayPause_clicked()
{
    if (!playing) {

        setButtonToPause();
        QApplication::processEvents();

        // startCommunication();

        playing = true;
    } else {

        setButtonToPlay();
        QApplication::processEvents();


        // stopCommunication();

        playing = false;
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
    if (isButtonColorBlue) {        
        ui->ButtonCor->setStyleSheet("background-color: yellow;");
        ui->ButtonCor->setText("Amarelo");
        isButtonColorBlue = false;
    } 
    else {
        ui->ButtonCor->setStyleSheet("background-color: #00bbff;");
        ui->ButtonCor->setText("Azul");
        isButtonColorBlue = true;
    }
}