#ifndef ROBOTWIDGET_H
#define ROBOTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include "robotdata.h"

class StatusIndicator;

class RobotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RobotWidget(QWidget *parent = nullptr);

    void updateData(const RobotData &data);

private:
    void setupUi();

    QLabel *m_nameLabel;
    QComboBox *m_roleComboBox;
    StatusIndicator *m_statusIndicator;
    QLabel *m_robotImageLabel;
};

#endif