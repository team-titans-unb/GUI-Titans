#include "configmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <algorithm>

ConfigManager::ConfigManager(const QString &filePath) : m_filePath(filePath) {}

bool ConfigManager::load(const QString &category)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Não foi possível abrir o arquivo de configuração:" << m_filePath;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject rootObj = doc.object();
    
    QString categoryKey = "ROBOT_" + category.toUpper();

    if (!rootObj.contains(categoryKey)) {
        qWarning() << "Categoria" << categoryKey << "não encontrada no arquivo de configuração.";
        return false;
    }

    m_robots.clear();
    QJsonObject robotsObj = rootObj[categoryKey].toObject();

    for (const QString &robotIdStr : robotsObj.keys()) {
        QJsonObject robotDetails = robotsObj[robotIdStr].toObject();
        RobotData data;

        data.id = robotDetails["id"].toInt();
        data.name = robotDetails["name"].toString();
        data.role = stringToRole(robotDetails["role"].toString());
        data.teamColor = robotDetails["teamColor"].toString();
        data.category = category;
        data.isConnected = false;

        m_robots.append(data);
    }
    
    std::sort(m_robots.begin(), m_robots.end(), [](const RobotData& a, const RobotData& b){
        return a.id < b.id;
    });

    return true;
}

bool ConfigManager::save(const QString &category)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Não foi possível abrir o arquivo para ler antes de salvar:" << m_filePath;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    QJsonObject rootObj = doc.object();

    QString categoryKey = "ROBOT_" + category.toUpper();
    QJsonObject updatedRobotsObj;

    for (const RobotData &robotData : m_robots) {
        QJsonObject robotDetails;
        robotDetails["id"] = robotData.id;
        robotDetails["name"] = robotData.name;
        robotDetails["teamColor"] = robotData.teamColor;
        robotDetails["role"] = roleToString(robotData.role);
        
        updatedRobotsObj[QString::number(robotData.id)] = robotDetails;
    }

    rootObj[categoryKey] = updatedRobotsObj;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Não foi possível abrir o arquivo para salvar as alterações:" << m_filePath;
        return false;
    }

    doc.setObject(rootObj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

QList<RobotData>& ConfigManager::getRobots() 
{ 
    return m_robots; 
}

QList<RobotData> ConfigManager::getAllRobots(QString category, QString teamColor) const
{
    QList<RobotData> allPossibleRobots;

    QMap<QString, int> categoryIdRanges;
    categoryIdRanges["vsss"] = 9;
    categoryIdRanges["ssl"] = 7;

    QStringList colors = {"blue", "yellow"};

    if (!categoryIdRanges.contains(category.toLower()) || !colors.contains(teamColor.toLower())) {
        qWarning() << "Categoria ou cor de time inválida para obter todos os robôs.";
        return allPossibleRobots;
    }

    for (int id = 0; id <= categoryIdRanges[category.toLower()]; ++id) {
        RobotData robot;
        robot.id = id;
        robot.name = QString("%1_%2_%3").arg(category, teamColor, QString::number(id));
        robot.role = RobotRole::Unknown;
        robot.isConnected = false;
        robot.category = category;
        robot.teamColor = teamColor;

        allPossibleRobots.append(robot);
    }

    return allPossibleRobots;
}

RobotRole ConfigManager::stringToRole(const QString &roleStr) {
    if (roleStr.compare("Attacker", Qt::CaseInsensitive) == 0) return RobotRole::Attacker;
    if (roleStr.compare("Goalkeeper", Qt::CaseInsensitive) == 0) return RobotRole::Goalkeeper;
    if (roleStr.compare("Defender", Qt::CaseInsensitive) == 0) return RobotRole::Defender;
    return RobotRole::Unknown;
}

QString ConfigManager::roleToString(RobotRole role) {
    switch(role) {
        case RobotRole::Attacker: return "Attacker";
        case RobotRole::Goalkeeper: return "Goalkeeper";
        case RobotRole::Defender: return "Defender";
        default: return "Unknown";
    }
}

