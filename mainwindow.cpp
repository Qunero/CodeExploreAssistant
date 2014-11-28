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
    isInModifyCfgFileGroupInfoState = false;

    ui->setupUi(this);
    QRegExp reDec("0*[0-9]{0,19}\\s*"); //max is 9223372036854775807
    ui->lineEdit_dec->setValidator(new QRegExpValidator(reDec));
    QRegExp reHex("(0[Xx])?0*[A-Fa-f0-9]{0,15}\\s*"); // max is 0x7fffffffffffffff
    ui->lineEdit_hex->setValidator(new QRegExpValidator(reHex));
    ui->checkBox_autoConvert->toggled(true);
    ui->checkBox_autoConvert->setChecked(true);
    ui->pushButtonConvertDec2Hex->setDefault(true);
    ui->lineEdit_dec->setFocus();

    // configure page
    QRegExp reGroupName("[_a-zA-Z0-9]{1,16}");
    ui->lineEdit_groupName->setValidator(new QRegExpValidator(reGroupName));

    // for MVC
    QStringList headerLabels;
    headerLabels.insert(EM_CFGFILE_LIST_GROUP_NAME, "Group Name");
    headerLabels.insert(EM_CFGFILE_LIST_AUTOLOAD, "Autoload");
    headerLabels.insert(EM_CFGFILE_LIST_CFG_FILE_PATH, "File Path");
    cfgFileListModel.setColumnCount(EM_CFGFILE_LIST_COUNT);
    cfgFileListModel.setHorizontalHeaderLabels(headerLabels);
    ui->tableView_configuredFileList->setModel(&cfgFileListModel);
    // set read only and select whole row at once
    ui->tableView_configuredFileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_configuredFileList->setSelectionBehavior(QAbstractItemView::SelectRows);

    // app init and autoload some cfg file
    appInit();
    codeExplainingHeader << "Explaining" << "Code in Hex" << "Detail" << "Group";
    groupProxyModel.setSourceModel(&sourceModel);
    groupProxyModel.setFilterKeyColumn(3);
    codeProxyModel.setSourceModel(&groupProxyModel);
    codeProxyModel.setFilterKeyColumn(1);
    codeProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableView_codeDetail->setModel(&codeProxyModel);
    ui->tableView_codeDetail->sortByColumn(1, Qt::AscendingOrder);
    ui->tableView_codeDetail->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if (loadCfgFiles())
    {
        isCfgFileLoaded = true;
        // after successfully loaded cfg file, enable the 'Show Detail' function
        ui->checkBox_showDetail->setChecked(true);
    }

    QObject::connect(this->ui->tableView_configuredFileList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(modifyCfgFileGroupInfo()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::modifyCfgFileGroupInfo()
{
    QModelIndexList selectedList = ui->tableView_configuredFileList->selectionModel()->selectedIndexes();
    QStringList rowInfo;
    foreach (QModelIndex index, selectedList)
    {
        qDebug() << "selected row info : index.row=" <<index.row()<<"index.column=" <<index.column();
        rowInfo.append(cfgFileListModel.data(index).toString());
    }
    isInModifyCfgFileGroupInfoState = true;
    currentRowOfCfgFileList = ui->tableView_configuredFileList->currentIndex().row();
    updateChoosenFileInfo(rowInfo);
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

void MainWindow::clearChoosenFileInfo()
{
    ui->lineEdit_cfgFilePath->clear();
    ui->lineEdit_groupName->clear();
    ui->checkBox_autoloadCfgFile->setChecked(false);
    isInModifyCfgFileGroupInfoState = false;
    currentRowOfCfgFileList = 0;
}

void MainWindow::updateChoosenFileInfo(QStringList rowInfo)
{
    ui->lineEdit_groupName->setText(rowInfo[EM_CFGFILE_LIST_GROUP_NAME]);
    ui->checkBox_autoloadCfgFile->setChecked(rowInfo[EM_CFGFILE_LIST_AUTOLOAD].compare("Yes", Qt::CaseInsensitive)==0?true:false);
    ui->lineEdit_cfgFilePath->setText(rowInfo[EM_CFGFILE_LIST_CFG_FILE_PATH]);
}

bool MainWindow::hasValidSelectedCfgFile()
{
    // check weather has a valid choosen file
    if (! ui->tableView_configuredFileList->currentIndex().isValid())
    {
        QMessageBox::warning(this,
                             "Selection Needed",
                             "You need to select a configured file firstly.",
                             QMessageBox::Ok);
        return false;
    }
    return true;
}

bool MainWindow::isCurrentFileExists()
{
    return QFile(currentFileName).exists();
}

void MainWindow::saveRowToCfgFileListModel(QStringList rowInfo)
{
    // save choosen configure file to list
    QList<QStandardItem *> items;
    QStandardItem * groupNameItem = new QStandardItem(rowInfo[EM_CFGFILE_LIST_GROUP_NAME]);
    QStandardItem * autoloadItem = new QStandardItem(rowInfo[EM_CFGFILE_LIST_AUTOLOAD]);
    QStandardItem * filePathItem = new QStandardItem(rowInfo[EM_CFGFILE_LIST_CFG_FILE_PATH]);

    if (isInModifyCfgFileGroupInfoState)
    {
        cfgFileListModel.setItem(currentRowOfCfgFileList, EM_CFGFILE_LIST_GROUP_NAME, groupNameItem);
        cfgFileListModel.setItem(currentRowOfCfgFileList, EM_CFGFILE_LIST_AUTOLOAD, autoloadItem);
        cfgFileListModel.setItem(currentRowOfCfgFileList, EM_CFGFILE_LIST_CFG_FILE_PATH, filePathItem);
        qDebug() << "Save Cfg File to list: " << rowInfo;
    }
    else
    {
        items.append(groupNameItem);
        items.append(autoloadItem);
        items.append(filePathItem);
        cfgFileListModel.appendRow(items);
        qDebug() << "Add Cfg File to list: " << rowInfo;
    }
}

bool MainWindow::appInit()
{
    // check init file
    QFile iniFile(APP_INIT_FILE_PATH);
    if (! iniFile.exists())
    {
        qDebug() << APP_INIT_FILE_PATH << " is not exists!!!";
        return false;
    }

    if (! iniFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&iniFile);
    while (! in.atEnd())
    {
        QString line = in.readLine();
        QStringList row = line.split(CfgFileColumnSep);
        //qDebug() << row;
        if (row.count() < 2)
        {
            iniFile.close();
            return false;
        }
        saveRowToCfgFileListModel(row);
    }
    return true;
}

bool MainWindow::loadCfgFiles()
{
    QString group;
    QString filePath;
    int loadedFileCount = 0;
    sourceModel.setColumnCount(4);
    sourceModel.setHorizontalHeaderLabels(codeExplainingHeader);

    // update groups in explore page
    QStringList groups("ALL");
    for (int row=0; row < cfgFileListModel.rowCount(); row++) {
        groups.append(cfgFileListModel.item(row,0)->data(Qt::DisplayRole).toString());
    }
    ui->comboBox_chooseGroup->clear();
    ui->comboBox_chooseGroup->addItems(groups);

    for (int row=0; row<cfgFileListModel.rowCount(); row++)
    {
        QModelIndex index = cfgFileListModel.index(row, EM_CFGFILE_LIST_AUTOLOAD);
        if (cfgFileListModel.data(index).toString().compare("Yes") != 0)
        {
            continue;
        }
        index = cfgFileListModel.index(row, EM_CFGFILE_LIST_GROUP_NAME);
        group = cfgFileListModel.data(index).toString();
        index = cfgFileListModel.index(row, EM_CFGFILE_LIST_CFG_FILE_PATH);
        filePath = cfgFileListModel.data(index).toString();
        if (loadOneCfgFile(group, filePath))
        {
            loadedFileCount++;
        }
    }

    if (loadedFileCount > 0)
    {
        ui->statusBar->showMessage(QString("Successfully loaded %1 files.").arg(loadedFileCount), 5000);
        return true;
    }
    return false;
}


bool MainWindow::loadOneCfgFile(QString &group, QString & filePath)
{
    QFile f(filePath);

    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&f);
    QString headerMagic = in.readLine();
    if (headerMagic.compare(APP_CFG_FILE_MAGIC_HEADER) != 0)
    {
        ui->statusBar->showMessage(QString("File<%1> is not a valid Configure file!!! please check the content").arg(filePath), 3000);
        return false;
    }
    while (! in.atEnd())
    {
        QString line = in.readLine();
        QStringList row = line.split(CfgFileColumnSep);
        //qDebug() << row;
        if (row.count() < CFGFILE_CONTENT_COLUMN_COUNT)
        {
            // EM_SHOW_DETAIL_DETAIL is not a must, ignore the other invalid lines
            continue;
        }
        row.append(group);
        saveRowToSourceModel(row);
    }
    ui->statusBar->showMessage(QString("Successfully loaded file: %1").arg(filePath), 3000);
    return true;
}

void MainWindow::saveRowToSourceModel(QStringList rowInfo)
{
    QList<QStandardItem *> items;
    QStandardItem * explaining = new QStandardItem(rowInfo[EM_SHOW_DETAIL_EXPLAINING]);
    QStandardItem * code = new QStandardItem(rowInfo[EM_SHOW_DETAIL_CODE]);
    QStandardItem * detail = new QStandardItem(rowInfo[EM_SHOW_DETAIL_DETAIL]);
    QStandardItem * group = new QStandardItem(rowInfo[EM_SHOW_DETAIL_GROUP]);

    items.append(explaining);
    items.append(code);
    items.append(detail);
    items.append(group);
    sourceModel.appendRow(items);
    //qDebug() << "Add row to table: " << rowInfo;
}

void MainWindow::applyCodeFilter(bool clear)
{
    if (! ui->checkBox_showDetail->isChecked())
    {
        return;
    }

    if (clear)
    {
        codeProxyModel.setFilterRegExp(".*");
        return;
    }
    QString hex = QString::number(sharedNumberValue, 16);
    codeProxyModel.setFilterFixedString("0X"+hex);
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
        if (arg1.isEmpty())
        {
            ui->lineEdit_hex->text().clear();
            applyCodeFilter(true);
        }
        else
        {
            applyCodeFilter(false);
        }
    }
}

void MainWindow::on_lineEdit_hex_textChanged(const QString &arg1)
{
    if (ui->lineEdit_hex->hasFocus()
            && ui->checkBox_autoConvert->isChecked())
    {
        sharedNumberValue = arg1.toLongLong(0, 16);
        updateUiFromHex2Dec();
        if (arg1.isEmpty())
        {
            ui->lineEdit_dec->text().clear();
            applyCodeFilter(true);
        }
        else
        {
            applyCodeFilter(false);
        }
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
    QMessageBox::StandardButton btn;
    if (checked &&
            (! ui->checkBox_autoConvert->isChecked()))
    {
        // too keep the consistant 'Auto Convert' should be enable
        btn = QMessageBox::warning(this,
                                   "Enable Auto Convert",
                                   "    To keep consistant of your input number, please enable 'Auto Convert'."
                                   "Do you want to enable it now?",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
        if (btn != QMessageBox::Yes)
        {
            ui->checkBox_showDetail->setChecked(false);
            return;
        }
        ui->checkBox_autoConvert->setChecked(true);
        sharedNumberValue = ui->lineEdit_hex->text().toLongLong(0, 16);
        updateUiFromHex2Dec();
    }
    if (checked && !isCfgFileLoaded)
    {       
        btn = QMessageBox::warning(this,
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

void MainWindow::on_pushButton_saveToList_clicked()
{
    if (ui->lineEdit_cfgFilePath->text().isEmpty())
    {
        QMessageBox::warning(this,
                             "Choose Configure File",
                             "The <File Path> can not be empty, please choose a configure file.",
                             QMessageBox::Ok);
        ui->pushButton_chooseCfgFile->setFocus();
        return;
    }

    if (ui->lineEdit_groupName->text().isEmpty())
    {
        QMessageBox::warning(this,
                             "Setting Group Name",
                             "    The <Group Name> can not be empty, please set a proper \ngroup name for this configure file.",
                             QMessageBox::Ok);
        ui->lineEdit_groupName->setFocus();
        return;
    }

    if (! isCurrentFileExists())
    {
        QMessageBox::critical(this,
                              "File Not Exists",
                              QString("The configure file <%1> is not exists, please check file path, or you may try choose another one").arg(currentFileName),
                              QMessageBox::Ok);
        return;
    }

    // todo: ignore same file
    QStringList row;
    row.append(ui->lineEdit_groupName->text());
    row.append(ui->checkBox_autoloadCfgFile->isChecked()?"Yes":"No");
    row.append(ui->lineEdit_cfgFilePath->text());

    saveRowToCfgFileListModel(row);

    clearChoosenFileInfo();
}

void MainWindow::on_pushButton_modifyCfgFile_clicked()
{
    if (! hasValidSelectedCfgFile())
    {
        return;
    }

    modifyCfgFileGroupInfo();
}

void MainWindow::on_pushButton_openDetail_clicked()
{
    if (! hasValidSelectedCfgFile())
    {
        return;
    }

}

void MainWindow::on_pushButton_deleteCfgFile_clicked()
{
    if (! hasValidSelectedCfgFile())
    {
        return;
    }
    QString cfmMsg =  QString("    You have choosen a configure file at row %1,\n%2")
            .arg(ui->tableView_configuredFileList->currentIndex().row() + 1)
            .arg("Do you really want to delete this configure file?");
    if (QMessageBox::No == QMessageBox::warning(this,
                                                "Confirm Deletion",
                                                cfmMsg,
                                                QMessageBox::Yes | QMessageBox::No,
                                                QMessageBox::No))
    {
        return;
    }

    cfgFileListModel.removeRow(ui->tableView_configuredFileList->currentIndex().row());
}

void MainWindow::on_pushButton_clear_clicked()
{
    clearChoosenFileInfo();
}

void MainWindow::on_pushButton_chooseCfgFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a configure file"),
                                                    ".",
                                                    tr("Configure File (*.cfg *.txt);;All(*.*)"));
    if (fileName.isNull())
    {
        return;
    }
    ui->lineEdit_cfgFilePath->setText(fileName);
}

void MainWindow::on_lineEdit_cfgFilePath_textChanged(const QString &arg1)
{
    currentFileName = arg1;
}

void MainWindow::on_pushButton_loadAllCfgFile_clicked()
{
    if (loadCfgFiles())
    {
        isCfgFileLoaded = true;
    }
}

void MainWindow::on_pushButton_unloadAllCfgFile_clicked()
{
    sourceModel.clear();
    isCfgFileLoaded = false;
}

void MainWindow::on_comboBox_chooseGroup_currentIndexChanged(const QString &arg1)
{
    if (ui->comboBox_chooseGroup->currentIndex() == 0
            && arg1.compare("ALL") == 0)
    {
        groupProxyModel.setFilterRegExp(".*");
        return;
    }
    groupProxyModel.setFilterFixedString(arg1);
}

void MainWindow::on_pushButton_loadCfgFile_clicked()
{
    if (! hasValidSelectedCfgFile())
    {
        return;
    }
    QString group;
    QString filePath;

    // update groups in explore page, delte after added model for 'Choose Group'
    QStringList groups("ALL");
    for (int row=0; row < cfgFileListModel.rowCount(); row++) {
        groups.append(cfgFileListModel.item(row,0)->data(Qt::DisplayRole).toString());
    }
    ui->comboBox_chooseGroup->clear();
    ui->comboBox_chooseGroup->addItems(groups);

    int row = ui->tableView_configuredFileList->currentIndex().row();
    QModelIndex index = cfgFileListModel.index(row, EM_CFGFILE_LIST_GROUP_NAME);
    group = cfgFileListModel.data(index).toString();
    index = cfgFileListModel.index(row, EM_CFGFILE_LIST_CFG_FILE_PATH);
    filePath = cfgFileListModel.data(index).toString();
    loadOneCfgFile(group, filePath);
}

void MainWindow::on_pushButton_saveCfgFileList_clicked()
{
    // TODO
}
