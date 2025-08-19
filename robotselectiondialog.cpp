#include "robotselectiondialog.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialogButtonBox>
#include <QIcon>
#include <QPixmap>

RobotSelectionDialog::RobotSelectionDialog(const QList<RobotData>& availableRobots, QWidget *parent)
    : QDialog(parent)
{
    setupUi(availableRobots);
    setWindowTitle("Selecionar Robô");
    setMinimumWidth(300);
}

void RobotSelectionDialog::setupUi(const QList<RobotData>& availableRobots)
{
    m_listWidget = new QListWidget(this);
    m_listWidget->setIconSize(QSize(40, 40));

    for (const RobotData& robot : availableRobots) {
        QListWidgetItem* item = new QListWidgetItem(m_listWidget);
        item->setText(robot.name);
        QString imagePath = QString(":/robots/tags/%1_%2_%3.png")
                               .arg(robot.category.toLower())
                               .arg(robot.teamColor.toLower())
                               .arg(robot.id);
        item->setIcon(QIcon(imagePath));

        item->setData(Qt::UserRole, QVariant::fromValue(robot));
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RobotSelectionDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RobotSelectionDialog::reject);
    
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &RobotSelectionDialog::onAccepted);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_listWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void RobotSelectionDialog::onAccepted()
{
    QListWidgetItem* currentItem = m_listWidget->currentItem();
    if (currentItem) {
        m_selectedRobot = currentItem->data(Qt::UserRole).value<RobotData>();
        accept();
    }
}

RobotData RobotSelectionDialog::getSelectedRobot() const
{
    return m_selectedRobot;
}