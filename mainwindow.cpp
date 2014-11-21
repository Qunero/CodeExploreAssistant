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
            "    When 'Show Detail' checkbox is checked, you will quickly\n"
            "get the Explaining Code with its value match the input number.\n"
            "--------------------------------------------------------\n"
            "Author: QuNengrong(Neo Nengrong Qu)\n"
            "Email : Quner612#qq.com  \n"
            "--------------------------------------------------------\n"
            "Copyright (c) 2014, Copyleft follow LGPL 3.0.\n"
            "--------------------------------------------------------\n"
            "NOTE: If your number is greater than 9223372036854775807,\n"
            "i.e. Hex number 0x7fffffffffffffff, you will get 0 after convertion.";
    QString date = QDate::currentDate().toString(Qt::ISODate);
    aboutSoftwareMsg = QString(msgFmt).arg(date);
    sharedNumberValue = 0;
    isCfgFileLoaded = false;

    ui->setupUi(this);
    QRegExp reDec("0*[0-9]{0,19}\\s*"); //max is 9223372036854775807
    ui->lineEdit_dec->setValidator(new QRegExpValidator(reDec));
    QRegExp reHex("(0[Xx])?0*[A-Fa-f0-9]{0,15}\\s*"); // max is 0x7fffffffffffffff
    ui->lineEdit_hex->setValidator(new QRegExpValidator(reHex));
    ui->checkBox_autoConvert->toggled(true);
    ui->pushButtonConvertDec2Hex->setDefault(true);
    ui->lineEdit_dec->setFocus();

    // for MVC
    cfgFileListModel.setColumnCount(3);
    cfgFileListModel.setHorizontalHeaderLabels(QStringList()<<"Group Name"<<"Autoload"<<"File path");
    ui->tableView_configuredFileList->setModel(&cfgFileListModel);
    // set read only and select whole row at once
    ui->tableView_configuredFileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_configuredFileList->setSelectionBehavior(QAbstractItemView::SelectRows);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUiFromDec2Hex()
{
    QString hex = QString::number(sharedNumberValue, 16);
    ui->lineEdit_hex->setText("0X" + hex.toUpper());
}

void MainWindow::updateUiFromHex2Dec()
{
    QString dec = QString::number(sharedNumberValue, 10);
    ui->lineEdit_dec->setText(dec.toUpper());
}

void MainWindow::clearChooseFileInfo()
{
    ui->lineEdit_cfgFilePath->clear();
    ui->lineEdit_groupName->clear();
    ui->checkBox_autoloadCfgFile->setChecked(false);
}

void MainWindow::on_checkBox_autoConvert_toggled(bool checked)
{
    ui->pushButtonConvertDec2Hex->setEnabled(!checked);
    ui->pushButtonConvertHex2Dec->setEnabled(!checked);
}

void MainWindow::on_pushButtonConvertDec2Hex_clicked()
{
    sharedNumberValue = ui->lineEdit_dec->text().toLongLong(NULL, 10);
    updateUiFromDec2Hex();
}

void MainWindow::on_pushButtonConvertHex2Dec_clicked()
{
    sharedNumberValue = ui->lineEdit_hex->text().toLongLong(NULL, 16);
    updateUiFromHex2Dec();

}

void MainWindow::on_lineEdit_dec_textChanged(const QString &arg1)
{
    if (ui->lineEdit_dec->hasFocus()
            && ui->checkBox_autoConvert->isChecked())
    {
        sharedNumberValue = arg1.toLongLong(0, 10);
        updateUiFromDec2Hex();
    }
}

void MainWindow::on_lineEdit_hex_textChanged(const QString &arg1)
{
    if (ui->lineEdit_hex->hasFocus()
            && ui->checkBox_autoConvert->isChecked())
    {
        sharedNumberValue = arg1.toLongLong(0, 16);
        updateUiFromHex2Dec();
    }
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,
                       "About "APP_NAME,
                       aboutSoftwareMsg);
}

void MainWindow::on_checkBox_showDetail_toggled(bool checked)
{
    if (checked && !isCfgFileLoaded)
    {
        QMessageBox::StandardButton btn
                =
                QMessageBox::warning(this,
                                     "Configure Needed",
                                     "    No Explaining File is loaded, you need configure it.\n"
                                     "Do you want to configure it now?",
                                     QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            ui->tabWidget->setCurrentWidget(ui->tab_configure);
        }

        ui->checkBox_showDetail->setChecked(false);
    }
}

void MainWindow::on_pushButton_addToList_clicked()
{
    if (ui->lineEdit_groupName->text().isEmpty()
            || ui->lineEdit_cfgFilePath->text().isEmpty())
    {
        return;
    }

    // todo: ignore same file
    // save choosen configure file to list
    QList<QStandardItem *> items;
    items.append(new QStandardItem(ui->lineEdit_groupName->text()));
    items.append(new QStandardItem(ui->checkBox_autoloadCfgFile->isChecked()?"Yes":"No"));
    items.append(new QStandardItem(ui->lineEdit_cfgFilePath->text()));
    cfgFileListModel.insertRow(cfgFileListModel.rowCount(), items);

    clearChooseFileInfo();
}

void MainWindow::on_pushButton_modifyCfgFiel_clicked()
{
    /*QItemSelectionModel selection = ui->tableView_configuredFileList->selectionModel();
    selection.selection();
    QList<QStandardItem *> items = cfgFileListModel.data(ui->tableView_configuredFileList->currentIndex());
    ui->lineEdit_groupName->setText(iteams[0]);
    ui->checkBox_autoloadCfgFile->setChecked(items[1]->text().compare("Yes")==0?true:false);
    ui->lineEdit_cfgFilePath->setText(items[2]);*/
}
