#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("Titans", "Categoria");

    QString ultimoTexto = settings.value("ultimoTextoCategoria", "VSSS").toString();

    ui->ButtonCategoria->setText(ultimoTexto);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ButtonCategoria_clicked()
{
    QString novoTexto;


    if(ui->ButtonCategoria->text()=="VSSS"){
        novoTexto = "SSL";
    }else
        novoTexto = "VSSS";

    ui->ButtonCategoria->setText(novoTexto);

    QSettings settings("Titans", "Categoria");

    settings.setValue("ultimoTextoCategoria", novoTexto);
}


void MainWindow::on_ButtonLado_clicked()
{
    if(ui->ButtonLado->text()=="ESQUERDO"){
        ui->ButtonLado->setText("DIRETO");
    }
    else{
        ui->ButtonLado->setText("ESQUERDO!");
    }
}

