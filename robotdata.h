#ifndef ROBOTDATA_H
#include <QMetaType>
#define ROBOTDATA_H

#include <QString>

enum class RobotRole {
    Attacker,
    Goalkeeper,
    Defender,
    Unknown
};

struct RobotData {
    int id;
    QString name;
    RobotRole role;
    bool isConnected;
    
    QString category;
    QString teamColor;
};

Q_DECLARE_METATYPE(RobotData)

#endif