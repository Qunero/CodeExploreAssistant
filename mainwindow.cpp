#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString msgFmt =
            "          CodeExploreAssistant V" APP_REVISION " Build@%1      \n"
            "    This software is for quickly convert decimal numbers\n"
            "to hex numbers or in verse convert from hex to decimal.\n"
            "--------------------------------------------------------\n"
            "Author: QuNengrong(Neo Nengrong Qu)\n"
            "Email : Quner612#qq.com  \n"
            "--------------------------------------------------------\n"
            "Copyright (c) 2014, Copyleft follow LGPL 3.0.\n"
            "--------------------------------------------------------\n"
            "NOTE: If your number is greater than 9223372036854775807,\n"
            "i.e. Hex number 0x7fffffffffffffff, you will get 0 after convertion.";
    QString date = QDate::currentDate().toString(Qt::ISODate);
    _strMsgAboutSoftware = QString(msgFmt).arg(date);
    _sharedNumberValue = 0;

    ui->setupUi(this);
    QRegExp reDec("[0-9]{0,19}\\s*"); //max is 9223372036854775807
    ui->lineEdit_dec->setValidator(new QRegExpValidator(reDec));
    QRegExp reHex("(0[Xx])?0*[A-Fa-f0-9]{0,15}\\s*"); // max is 0x7fffffffffffffff
    ui->lineEdit_hex->setValidator(new QRegExpValidator(reHex));
    ui->checkBox_autoConvert->toggled(true);
    ui->pushButtonConvertDec2Hex->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUiFromDec2Hex()
{
    QString hex = QString::number(_sharedNumberValue, 16);
    ui->lineEdit_hex->setText("0X" + hex.toUpper());
}

void MainWindow::updateUiFromHex2Dec()
{
    QString dec = QString::number(_sharedNumberValue, 10);
    ui->lineEdit_dec->setText(dec.toUpper());
}

void MainWindow::on_checkBox_autoConvert_toggled(bool checked)
{
    ui->pushButtonConvertDec2Hex->setVisible(!checked);
    ui->pushButtonConvertHex2Dec->setVisible(!checked);
}

void MainWindow::on_pushButtonConvertDec2Hex_clicked()
{
    _sharedNumberValue = ui->lineEdit_dec->text().toLongLong(NULL, 10);
    updateUiFromDec2Hex();
}

void MainWindow::on_pushButtonConvertHex2Dec_clicked()
{
    _sharedNumberValue = ui->lineEdit_hex->text().toLongLong(NULL, 16);
    updateUiFromHex2Dec();

}

void MainWindow::on_lineEdit_dec_textChanged(const QString &arg1)
{
    if (ui->lineEdit_dec->hasFocus()
            && ui->checkBox_autoConvert->isChecked())
    {
        _sharedNumberValue = arg1.toLongLong(0, 10);
        updateUiFromDec2Hex();
    }
}

void MainWindow::on_lineEdit_hex_textChanged(const QString &arg1)
{
    if (ui->lineEdit_hex->hasFocus()
            && ui->checkBox_autoConvert->isChecked())
    {
        _sharedNumberValue = arg1.toLongLong(0, 16);
        updateUiFromHex2Dec();
    }
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,
                       "About "APP_NAME,
                       _strMsgAboutSoftware);
}
