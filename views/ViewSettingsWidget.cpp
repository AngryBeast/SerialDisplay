// -------------------------------------------------------------------------------------------------------------------
//
//  File: ViewSettingsWidget.cpp
//
//  Copyright 2016 (c) Decawave Ltd, Dublin, Ireland.
//
//  All rights reserved.
//
//  Author:
//
// -------------------------------------------------------------------------------------------------------------------

#include "ViewSettingsWidget.h"
#include "ui_ViewSettingsWidget.h"

#include "RTLSDisplayApplication.h"
#include "QPropertyModel.h"
#include "ViewSettings.h"
#include "OriginTool.h"
#include "ScaleTool.h"
#include "GraphicsView.h"
#include "GraphicsWidget.h"
#include "RTLSClient.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>

ViewSettingsWidget::ViewSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewSettingsWidget),
    _floorplanOpen(false)
{
    ui->setupUi(this);

    //ui->tabWidget->setCurrentIndex(0);
    //ui->tabWidget->removeTab(2);

    //QObject::connect(ui->floorplanOpen_pb, SIGNAL(clicked()), this, SLOT(floorplanOpenClicked()));

    //QObject::connect(ui->scaleX_pb, SIGNAL(clicked()), this, SLOT(scaleClicked()));
    //QObject::connect(ui->scaleY_pb, SIGNAL(clicked()), this, SLOT(scaleClicked()));
    //QObject::connect(ui->origin_pb, SIGNAL(clicked()), this, SLOT(originClicked()));
    socket = new QTcpSocket();
    server = new QTcpServer();

    //QObject::connect(ui->saveFP, SIGNAL(clicked()), this, SLOT(saveFPClicked()));
    QObject::connect(ui->gridShow, SIGNAL(clicked()), this, SLOT(gridShowClicked()));
    //QObject::connect(ui->showOrigin, SIGNAL(clicked()), this, SLOT(originShowClicked()));
    QObject::connect(ui->showTagHistory, SIGNAL(clicked()), this, SLOT(tagHistoryShowClicked()));
    //QObject::connect(ui->showGeoFencingMode, SIGNAL(clicked()), this, SLOT(showGeoFencingModeClicked()));
    QObject::connect(ui->showNavigationMode, SIGNAL(clicked()), this, SLOT(showNavigationModeClicked()));

    QObject::connect(ui->useAutoPos, SIGNAL(clicked()), this, SLOT(useAutoPosClicked()));
    QObject::connect(ui->showTagTable, SIGNAL(clicked()), this, SLOT(tagAncTableShowClicked()));
    QObject::connect(ui->showAnchorTable, SIGNAL(clicked()), this, SLOT(tagAncTableShowClicked()));
    QObject::connect(ui->showAnchorTagCorrectionTable, SIGNAL(clicked()), this, SLOT(tagAncTableShowClicked()));

    //QObject::connect(ui->zone1, SIGNAL(editingFinished()), this, SLOT(zone1EditFinished()));
    //QObject::connect(ui->zone2, SIGNAL(editingFinished()), this, SLOT(zone2EditFinished()));

    //QObject::connect(ui->zone1, SIGNAL(valueChanged(double)), this, SLOT(zone1ValueChanged(double)));
    //QObject::connect(ui->zone2, SIGNAL(valueChanged(double)), this, SLOT(zone2ValueChanged(double)));
    //QObject::connect(ui->inAlarm, SIGNAL(clicked()), this, SLOT(alarmSetClicked()));
    //QObject::connect(ui->outAlarm, SIGNAL(clicked()), this, SLOT(alarmSetClicked()));

    QObject::connect(ui->tagHistoryN, SIGNAL(valueChanged(int)), this, SLOT(tagHistoryNumberValueChanged(int)));

    QObject::connect(RTLSDisplayApplication::viewSettings(), SIGNAL(showSave(bool)), this, SLOT(showSave(bool)));
    QObject::connect(RTLSDisplayApplication::viewSettings(), SIGNAL(showGO(bool, bool)), this, SLOT(showOriginGrid(bool, bool)));
    QObject::connect(RTLSDisplayApplication::viewSettings(), SIGNAL(setFloorPlanPic()), this, SLOT(getFloorPlanPic()));
    QObject::connect(RTLSDisplayApplication::client(), SIGNAL(enableFiltering()), this, SLOT(enableFiltering()));

    QObject::connect(ui->logging_pb, SIGNAL(clicked()), this, SLOT(loggingClicked()));


    //TCP
    QObject::connect(socket, &QTcpSocket::readyRead, this, &ViewSettingsWidget::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &ViewSettingsWidget::socket_Disconnected);
    QObject::connect(server,&QTcpServer::newConnection,this,&ViewSettingsWidget::server_New_Connect);
    QObject::connect(ui->pushButton_send,&QPushButton::clicked,this,&ViewSettingsWidget::on_pushButton_Send_clicked);
    QObject::connect(ui->pushButton_connect,&QPushButton::clicked,this,&ViewSettingsWidget::on_pushButton_Connect_clicked);



    //get data
    //QObject::connect(RTLSDisplayApplication::_client,&QPushButton::clicked,this,&ViewSettingsWidget::on_pushButton_Connect_clicked);

    ui->pushButton_send->setEnabled(false);
    ui->lineEdit_port->setText("8888");
    _logging = false ;


    ui->label_logfile->setText("");
    if(_logging)
    {
        ui->logging_pb->setText("Stop");
        ui->label_logingstatus->setText("Logging enabled.");
    }
    else
    {
        ui->logging_pb->setText("Start");
        ui->label_logingstatus->setText("Logging disabled.");
    }

    RTLSDisplayApplication::connectReady(this, "onReady()");
}

void ViewSettingsWidget::onReady()
{
    QPropertyDataWidgetMapper *mapper = QPropertyModel::newMapper(RTLSDisplayApplication::viewSettings(), this);
    mapper->addMapping(ui->gridWidth_sb, "gridWidth");
    mapper->addMapping(ui->gridHeight_sb, "gridHeight");

    //mapper->addMapping(ui->floorplanFlipX_cb, "floorplanFlipX", "checked");
    //mapper->addMapping(ui->floorplanFlipY_cb, "floorplanFlipY", "checked");
    mapper->addMapping(ui->gridShow, "showGrid", "checked");
    //mapper->addMapping(ui->showOrigin, "showOrigin", "checked");

    //mapper->addMapping(ui->floorplanXOff_sb, "floorplanXOffset");
    //mapper->addMapping(ui->floorplanYOff_sb, "floorplanYOffset");

    //mapper->addMapping(ui->floorplanXScale_sb, "floorplanXScale");
    //mapper->addMapping(ui->floorplanYScale_sb, "floorplanYScale");
    mapper->toFirst();

    //QObject::connect(ui->floorplanFlipX_cb, SIGNAL(clicked()), mapper, SLOT(submit())); // Bug with QDataWidgetMapper (QTBUG-1818)
    //QObject::connect(ui->floorplanFlipY_cb, SIGNAL(clicked()), mapper, SLOT(submit()));
    QObject::connect(ui->gridShow, SIGNAL(clicked()), mapper, SLOT(submit())); // Bug with QDataWidgetMapper (QTBUG-1818)
    //QObject::connect(ui->showOrigin, SIGNAL(clicked()), mapper, SLOT(submit()));

    //by default the Geo-Fencing is OFF

    ui->showTagHistory->setChecked(true);

    //ui->zone1->setDisabled(true);
   // ui->zone2->setDisabled(true);
    //ui->label_z1->setDisabled(true);
    //ui->label_z2->setDisabled(true);
    //ui->outAlarm->setDisabled(true);
    //ui->inAlarm->setDisabled(true);

    ui->tabWidget->setCurrentIndex(0);

    //RTLSDisplayApplication::graphicsWidget()->zone1Value(ui->zone1->value());
    //RTLSDisplayApplication::graphicsWidget()->zone2Value(ui->zone2->value());
    //RTLSDisplayApplication::graphicsWidget()->setAlarm(ui->inAlarm->isChecked(), ui->outAlarm->isChecked());

    ui->filtering->setEnabled(false);
    ui->filtering->addItems(RTLSDisplayApplication::client()->getLocationFilters());
    QObject::connect(ui->filtering, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLocationFilter(int)));
}

ViewSettingsWidget::~ViewSettingsWidget()
{
    delete ui;
}


void ViewSettingsWidget::enableFiltering(void)
{
     ui->filtering->setEnabled(true);
}

void ViewSettingsWidget::updateLocationFilter(int index)
{
     RTLSDisplayApplication::client()->setLocationFilter(index);
}

int ViewSettingsWidget::applyFloorPlanPic(const QString &path)
{
    QPixmap pm(path);

    if (pm.isNull())
    {
        //QMessageBox::critical(this, "Could not load floor plan", QString("Failed to load image : %1").arg(path));
        return -1;
    }

    //ui->floorplanPath_lb->setText(QFileInfo(path).fileName());
    RTLSDisplayApplication::viewSettings()->setFloorplanPixmap(pm);
    _floorplanOpen = true;
    //ui->floorplanOpen_pb->setText("Clear");

    return 0;
}

void ViewSettingsWidget::getFloorPlanPic()
{
    applyFloorPlanPic(RTLSDisplayApplication::viewSettings()->getFloorplanPath());
}

//void ViewSettingsWidget::floorplanOpenClicked()
//{
//    if(_floorplanOpen == false)
//    {
//        QString path = QFileDialog::getOpenFileName(this, "Open Bitmap", QString(), "Image (*.png *.jpg *.jpeg *.bmp)");
//        if (path.isNull()) return;

//        if(applyFloorPlanPic(path) == 0) //if OK set/save the path string
//        {
//            RTLSDisplayApplication::viewSettings()->floorplanShow(true);
//            RTLSDisplayApplication::viewSettings()->setFloorplanPath(path);
//        }
//        _floorplanOpen = true;
//        ui->floorplanOpen_pb->setText("Clear");
//    }
//    else
//    {
//       RTLSDisplayApplication::viewSettings()->floorplanShow(false);
//       RTLSDisplayApplication::viewSettings()->clearSettings();
//       _floorplanOpen = false;
//       ui->floorplanOpen_pb->setText("Open");
//       ui->floorplanFlipX_cb->setChecked(false);
//       ui->floorplanFlipY_cb->setChecked(false);
//       ui->floorplanXScale_sb->setValue(0.0);
//       ui->floorplanYScale_sb->setValue(0.0);
//       ui->floorplanXOff_sb->setValue(0.0);
//       ui->floorplanYOff_sb->setValue(0.0);
//       ui->floorplanPath_lb->setText("");
//    }
//}

void ViewSettingsWidget::showOriginGrid(bool orig, bool grid)
{
    Q_UNUSED(orig)

    ui->gridShow->setChecked(grid);
    //ui->showOrigin->setChecked(orig);
}

void ViewSettingsWidget::gridShowClicked()
{
    RTLSDisplayApplication::viewSettings()->setShowGrid(ui->gridShow->isChecked());
}

//void ViewSettingsWidget::originShowClicked()
//{
//    RTLSDisplayApplication::viewSettings()->setShowOrigin(ui->showOrigin->isChecked());
//}

void ViewSettingsWidget::showNavigationModeClicked()
{
    if(ui->showNavigationMode->isChecked())
    {
        //ui->showGeoFencingMode->setChecked(false);
        //showGeoFencingModeClicked();
        ui->showNavigationMode->setChecked(true);
    }
    else
    {
        ui->showNavigationMode->setChecked(true);
        //ui->showGeoFencingMode->setChecked(true);
        //showGeoFencingModeClicked();
    }
}




void ViewSettingsWidget::tagHistoryNumberValueChanged(int value)
{
    RTLSDisplayApplication::graphicsWidget()->tagHistoryNumber(value);
}


void ViewSettingsWidget::useAutoPosClicked()
{
    RTLSDisplayApplication::client()->setUseAutoPos(ui->useAutoPos->isChecked());

    RTLSDisplayApplication::graphicsWidget()->anchTableEditing(!ui->useAutoPos->isChecked());
}

void ViewSettingsWidget::tagAncTableShowClicked()
{
    RTLSDisplayApplication::graphicsWidget()->setShowTagAncTable(ui->showAnchorTable->isChecked(),
                                                                 ui->showTagTable->isChecked(),
                                                                 ui->showAnchorTagCorrectionTable->isChecked());
}

void ViewSettingsWidget::setTagHistory(int h)
{
    ui->tagHistoryN->setValue(h);
}


void ViewSettingsWidget::tagHistoryShowClicked()
{
    RTLSDisplayApplication::graphicsWidget()->setShowTagHistory(ui->showTagHistory->isChecked());
}


void ViewSettingsWidget::loggingClicked(void)
{
    if(_logging == false)
    {
        _logging = true ;
        RTLSDisplayApplication::client()->openLogFile("");
        ui->logging_pb->setText("Stop");
        ui->label_logingstatus->setText("Logging enabled.");
        ui->label_logfile->setText(RTLSDisplayApplication::client()->getLogFilePath());
    }
    else
    {
        RTLSDisplayApplication::client()->closeLogFile();
        ui->logging_pb->setText("Start");
        ui->label_logingstatus->setText("Logging disabled.");
        ui->label_logfile->setText("");
        //ui->saveFP->setChecked(false);
        _logging = false ;
    }
}

void ViewSettingsWidget::socket_Read_Data()
{
    QByteArray buffer;
        //读取缓冲区数据
        buffer = socket->readAll();
        if(!buffer.isEmpty())
        {

        }
}

void ViewSettingsWidget::socket_Disconnected()
{
    ui->pushButton_send->setEnabled(false);
    ui->pushButton_connect->setText("connect");
}

void ViewSettingsWidget::server_New_Connect()
{

    //获取客户端连接
    socket = server->nextPendingConnection();
    //连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(socket, &QTcpSocket::readyRead, this, &ViewSettingsWidget::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &ViewSettingsWidget::socket_Disconnected);
    //发送按键使能
    ui->pushButton_send->setEnabled(true);

    qDebug() << "A Client connect!";
}

void ViewSettingsWidget::on_pushButton_Send_clicked()
{
    QString msg = "sss";
    socket->write(msg.toUtf8());
    socket->flush();
}

void ViewSettingsWidget::on_pushButton_Connect_clicked()
{
    if(ui->pushButton_connect->text() == tr("connect"))
    {
        //从输入框获取端口号
        int port = ui->lineEdit_port->text().toInt();

        //监听指定的端口
        if(!server->listen(QHostAddress::Any, port))
        {
            //若出错，则输出错误信息
            qDebug()<<server->errorString();
            return;
        }
        //修改按键文字
        ui->pushButton_connect->setText("disconnect");
        ui->pushButton_send->setEnabled(true);

        //qDebug()<< "Listen succeessfully!";
    }
    else
    {
        socket->close();
        ui->pushButton_connect->setText("connect");
        ui->pushButton_send->setEnabled(false);
    }

}

void ViewSettingsWidget::TCPSendtagPos(quint64 tagId, double x, double y, double z)
{
    qDebug()<< "AB:get slot!!!!!!" << endl;

    if (ui->checkBox_send->isChecked())
    {
        if (ui->pushButton_connect->text() == tr("disconnect"))
        {

            QString msg = QString::number(x, 'f', 2);
            msg.append(",");
            msg.append(QString::number(y, 'f', 2));
            msg.append(",");
            msg.append(QString::number(z, 'f', 2));

            qDebug()<< msg << endl;

            socket->write(msg.toUtf8());
            socket->flush();
        }
    }
}



void ViewSettingsWidget::originClicked()
{
    OriginTool *tool = new OriginTool(this);
    QObject::connect(tool, SIGNAL(done()), tool, SLOT(deleteLater()));
    RTLSDisplayApplication::graphicsView()->setTool(tool);
}

void ViewSettingsWidget::scaleClicked()
{
    ScaleTool *tool = NULL;

    if (QObject::sender() == 0)
        tool = new ScaleTool(ScaleTool::XAxis, this);
    else if (QObject::sender() == 0)
        tool = new ScaleTool(ScaleTool::YAxis, this);

    QObject::connect(tool, SIGNAL(done()), tool, SLOT(deleteLater()));
    RTLSDisplayApplication::graphicsView()->setTool(tool);
}



