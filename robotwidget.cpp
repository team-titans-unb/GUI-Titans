#include "robotwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QEvent>

class CenteredItemDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem centeredOption = option;
        centeredOption.displayAlignment = Qt::AlignCenter;
        
        QStyledItemDelegate::paint(painter, centeredOption, index);
    }
};

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
    m_nameLabel->setStyleSheet("font-weight: bold;");
    m_roleComboBox = new QComboBox();
    m_statusIndicator = new StatusIndicator();
    
    m_robotImageLabel = new QLabel();
    m_robotImageLabel->setFixedSize(68, 68);
    m_robotImageLabel->setScaledContents(true);
    m_robotImageLabel->installEventFilter(this);

    m_roleComboBox->addItem("ATACANTE", QVariant::fromValue(RobotRole::Attacker));
    m_roleComboBox->addItem("GOLEIRO", QVariant::fromValue(RobotRole::Goalkeeper));
    m_roleComboBox->addItem("ZAGUEIRO", QVariant::fromValue(RobotRole::Defender));
    for (int i = 0; i < m_roleComboBox->count(); ++i) {
        m_roleComboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    m_roleComboBox->setItemDelegate(new CenteredItemDelegate(m_roleComboBox));
    m_roleComboBox->setStyleSheet(
        "QComboBox {"
        "    background-color: #E8E8E8;"
        "    border: none;"
        "    border-radius: 10px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QComboBox::drop-down { border: none; }"

        "QComboBox QFrame {"
        "    border: none;"
        "}"

        "QComboBox QAbstractItemView {"
        "    background-color: #E8E8E8;"
        "    border: none;"
        "    border-radius: 10px;"
        "}"

        "QComboBox QAbstractItemView::item {"
        "    height: 54px;"
        "    width: 183px;"
        "}"

        "QComboBox QAbstractItemView::item:hover {"
        "    background-color: #D4D4D4;"
        "    border-radius: 10px;"
        "    margin: 3px 10px;"
        "}"
    );
    m_roleComboBox->setFixedSize(208, 61);

    connect(m_roleComboBox, QOverload<int>::of(&QComboBox::activated), this, &RobotWidget::onRoleSelectionChanged);

    QWidget *infoContainer = new QWidget();
    infoContainer->setStyleSheet(
        "background-color: white;"
        "border-radius: 10px;"
    );

    QHBoxLayout *infoLayout = new QHBoxLayout(infoContainer);
    infoLayout->setContentsMargins(5, 5, 5, 5);
    infoLayout->addWidget(m_robotImageLabel);
    infoLayout->addWidget(m_roleComboBox);
    infoLayout->addWidget(m_statusIndicator);

    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(m_nameLabel);
    topLayout->addWidget(infoContainer);

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

bool RobotWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_robotImageLabel && event->type() == QEvent::MouseButtonPress) {
        emit idChanged(m_currentData);
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

RobotData RobotWidget::getCurrentData() const
{
    return m_currentData;
}

void RobotWidget::onRoleSelectionChanged(int index)
{
    QVariant variantData = m_roleComboBox->itemData(index);
    RobotRole newRole = variantData.value<RobotRole>();

    m_currentData.role = newRole;

    emit roleChanged(m_currentData);
}