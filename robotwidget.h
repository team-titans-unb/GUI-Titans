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
    RobotData getCurrentData() const;

signals:
    void idChanged(const RobotData &data);
    void roleChanged(const RobotData& data);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupUi();

    QLabel *m_nameLabel;
    QComboBox *m_roleComboBox;
    StatusIndicator *m_statusIndicator;
    QLabel *m_robotImageLabel;
    RobotData m_currentData;
    void onRoleSelectionChanged(int index);
};

#endif