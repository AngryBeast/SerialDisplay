// -------------------------------------------------------------------------------------------------------------------
//
//  File: ViewSettingsWidget.h
//
//  Copyright 2016 (c) Decawave Ltd, Dublin, Ireland.
//
//  All rights reserved.
//
//  Author:
//
// -------------------------------------------------------------------------------------------------------------------

#ifndef VIEWSETTINGSWIDGET_H
#define VIEWSETTINGSWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class ViewSettingsWidget;
}

class ViewSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewSettingsWidget(QWidget *parent = 0);
    ~ViewSettingsWidget();

    int applyFloorPlanPic(const QString &path);

signals:
    void saveViewSettings(void);

protected slots:
    void onReady();

    //void floorplanOpenClicked();
    void updateLocationFilter(int index);
    void enableFiltering(void);
    void originClicked();
    void scaleClicked();

    void gridShowClicked();
    //void originShowClicked();
    void tagHistoryShowClicked();

    //void saveFPClicked();
    void tagAncTableShowClicked();
    void useAutoPosClicked();
    //void showGeoFencingModeClicked();
    void showNavigationModeClicked();
    //void alarmSetClicked();

    //void zone1ValueChanged(double);
    //void zone2ValueChanged(double);
    //void zone1EditFinished(void);
    //void zone2EditFinished(void);
    void tagHistoryNumberValueChanged(int);

    void showOriginGrid(bool orig, bool grid);
    void getFloorPlanPic(void);
    //void showSave(bool);

    void setTagHistory(int h);
    void loggingClicked(void);

    //TCP
    void socket_Read_Data();
    void socket_Disconnected();
    void server_New_Connect();
    void on_pushButton_Send_clicked();
    void on_pushButton_Connect_clicked();

//public slots:
    void TCPSendtagPos(quint64 tagId, double x, double y, double z);

private:
    Ui::ViewSettingsWidget *ui;

    bool _logging ;
    bool _floorplanOpen ;
    QTcpServer* server;
    QTcpSocket *socket;

};

#endif // VIEWSETTINGSWIDGET_H
