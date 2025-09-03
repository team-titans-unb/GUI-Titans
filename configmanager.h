#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QList>
#include "robotdata.h"

class ConfigManager
{
public:
    explicit ConfigManager(const QString &filePath);
    bool load(const QString &category); 
    bool save(const QString &category); 
    QList<RobotData>& getRobots();
    QList<RobotData> getAllRobots(QString category, QString teamColor) const;

private:
    QString m_filePath;
    QList<RobotData> m_robots;

    RobotRole stringToRole(const QString &roleStr);
    QString roleToString(RobotRole role);
};

#endif