#include "robotwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class StatusIndicator : public QWidget {
public:
    explicit StatusIndicator(QWidget *parent = nullptr) : QWidget(parent), m_isConnected(false) {
        setFixedSize(29, 29);
    }
    void setConnected(bool connected) {
        m_isConnected = connected;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_isConnected ? QColor("#4DE439") : QColor("#E5393C"));
        painter.drawEllipse(rect());
    }
private:
    bool m_isConnected;
};

RobotWidget::RobotWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void RobotWidget::setupUi()
{
    m_nameLabel = new QLabel("ZEUS (0)");
    m_roleComboBox = new QComboBox();
    m_statusIndicator = new StatusIndicator();
    
    m_robotImageLabel = new QLabel();
    m_robotImageLabel->setFixedSize(68, 68);
    m_robotImageLabel->setScaledContents(true);

    m_roleComboBox->addItem("ATACANTE", QVariant::fromValue(RobotRole::Attacker));
    m_roleComboBox->addItem("GOLEIRO", QVariant::fromValue(RobotRole::Goalkeeper));
    m_roleComboBox->addItem("ZAGUEIRO", QVariant::fromValue(RobotRole::Defender));

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_robotImageLabel);

    QVBoxLayout *nameAndRoleLayout = new QVBoxLayout();
    nameAndRoleLayout->addWidget(m_nameLabel);
    nameAndRoleLayout->addWidget(m_roleComboBox);
    
    topLayout->addLayout(nameAndRoleLayout);
    //topLayout->addStretch();
    topLayout->addWidget(m_statusIndicator);
    
    this->setStyleSheet("RobotWidget { background-color: #f0f0f0; border: 1px solid #dcdcdc; border-radius: 8px; }");
    this->setLayout(topLayout);
}

void RobotWidget::updateData(const RobotData &data)
{
    m_nameLabel->setText(QString("%1 (%2)").arg(data.name).arg(data.id));
    m_statusIndicator->setConnected(data.isConnected);

    int index = m_roleComboBox->findData(QVariant::fromValue(data.role));
    if (index != -1) {
       m_roleComboBox->setCurrentIndex(index);
    }

    QString imagePath = QString(":/robots/tags/%1_%2_%3.png")
                           .arg(data.category.toLower())   // "vsss"
                           .arg(data.teamColor)  // "blue"
                           .arg(data.id);       // 1

    QPixmap robotPixmap(imagePath);

    if(robotPixmap.isNull()){
        qWarning() << "Imagem não encontrada:" << imagePath;
        m_robotImageLabel->setText("?");
    } else {
        m_robotImageLabel->setPixmap(robotPixmap);
    }

    m_currentData = data;
    m_nameLabel->setText(QString("%1 (%2)").arg(data.name).arg(data.id));
}

void RobotWidget::mousePressEvent(QMouseEvent *event)
{
    emit idChanged(m_currentData);
    QWidget::mousePressEvent(event);
}

RobotData RobotWidget::getCurrentData() const
{
    return m_currentData;
}
