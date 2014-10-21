#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    usb.setPortName("COM3");
    usb.setBaudRate(QSerialPort::Baud9600);
    usb.setDataBits(QSerialPort::Data8);

    connect(&usb,SIGNAL(readyRead()),this,SLOT(onReceivedData()));

    usb.open(QIODevice::ReadOnly);
}

MainWindow::~MainWindow()
{
    usb.close();
    delete ui;
}

void MainWindow::onReceivedData()
{
    data += usb.readAll();
    QByteArray dataTmp;

    if(data.length() >= 7)
    {
        dataTmp = data.right(7);
        dataPartExtract(dataTmp);
    }
}

bool MainWindow::dataPartExtract(QByteArray dataIn)
{
    double temp;
    QString str = QString(dataIn.left(5));
    temp = str.toDouble();
    ui->lcdNumber->display(QString::number(temp,'f',2));
    //

    return 0;
}
