#ifndef ROBOTSELECTIONDIALOG_H
#define ROBOTSELECTIONDIALOG_H

#include <QDialog>
#include <QList>
#include "robotdata.h"

class QListWidget;

class RobotSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RobotSelectionDialog(const QList<RobotData>& availableRobots, QWidget *parent = nullptr);

    RobotData getSelectedRobot() const;

private slots:
    void onAccepted();

private:
    void setupUi(const QList<RobotData>& availableRobots);

    QListWidget* m_listWidget;
    RobotData m_selectedRobot;
};

#endif