#include "inequalitiespanel.h"
#include "ui_inequalitiespanel.h"
#include <boost/lexical_cast.hpp>

std::vector<std::string> InequalitiesPanel::m_sSign{">=", ">", "<", "<="};


InequalitiesPanel::InequalitiesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InequalitiesPanel)
{
    ui->setupUi(this);

    m_images = QStringList{"://Images/ge.png", "://Images/gt.png", "://Images/lt.png", "://Images/le.png"};

    for (int i=1; i <= MAX_INEQUALITIES; i++) {//inequality_1
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));
        QPushButton * usun = QObject::findChild<QPushButton*>(QString("usun_%1").arg(i));
        QPushButton * lnier = QObject::findChild<QPushButton*>(QString("lnierownosc_%1").arg(i));
        QPushButton * rnier = QObject::findChild<QPushButton*>(QString("rnierownosc_%1").arg(i));
        QDoubleSpinBox * lval = QObject::findChild<QDoubleSpinBox*>(QString("lvalue_%1").arg(i));
        QDoubleSpinBox * rval = QObject::findChild<QDoubleSpinBox*>(QString("rvalue_%1").arg(i));

        ramka->hide();

        usun->setIcon(QIcon("://Images/remove.png"));
        usun->setIconSize(QSize(18, 18));
        usun->setProperty("numer", i);
        lnier->setIcon(QIcon(m_images[m_content[i-1].m_cLeftRight]));
        lnier->setIconSize(QSize(18, 18));
        lnier->setProperty("numer", i);
        rnier->setIcon(QIcon("://Images/add.png"));
        rnier->setIconSize(QSize(18, 18));
        rnier->setProperty("numer", i);

        rval->setVisible(false);

        connect(usun, SIGNAL(clicked()), this, SLOT(removeFormula()));
        connect(lnier, SIGNAL(clicked()), this, SLOT(changeLValue()));
        connect(rnier, SIGNAL(clicked()), this, SLOT(addRValue()));
    }
}

InequalitiesPanel::~InequalitiesPanel()
{
    delete ui;
}

std::vector<InequalitiesPanel::string_pair> InequalitiesPanel::createFormula()
{
    std::vector<string_pair> formula;
    quint32 ynum=0;

    for (int i=1; i <= MAX_INEQUALITIES; i++) {//inequality_1
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));

        if (ramka->isHidden()) {
            break;
        }

        QDoubleSpinBox * lval = QObject::findChild<QDoubleSpinBox*>(QString("lvalue_%1").arg(i));
        QDoubleSpinBox * rval = QObject::findChild<QDoubleSpinBox*>(QString("rvalue_%1").arg(i));
        QLineEdit * le = QObject::findChild<QLineEdit*>(QString("wzor_%1").arg(i));

        std::string str;
        str = "y";
        str += boost::lexical_cast<std::string>(ynum++);
        str += " = ";
        str += le->text().toStdString();
        str += " - ";
        str += boost::lexical_cast<std::string>(lval->value());
        str += ";";

        formula.push_back(string_pair {str, m_sSign[m_content[i-1].m_cLeftRight]});

        if (m_content[i-1].m_bHaveRight) {
            str = "y";
            str += boost::lexical_cast<std::string>(ynum++);
            str += " = ";
            str += le->text().toStdString();
            str += " - ";
            str += boost::lexical_cast<std::string>(rval->value());
            str += ";";

            formula.push_back(string_pair {str, m_sSign[3-m_content[i-1].m_cLeftRight]});
        }
    }

    return formula;
}

QStringList InequalitiesPanel::serialize()
{
    QStringList nierownosci;
    for (int i=1; i <= MAX_INEQUALITIES; i++) {
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));
        if (!ramka->isVisible()) break;

        QDoubleSpinBox * lval = QObject::findChild<QDoubleSpinBox*>(QString("lvalue_%1").arg(i));
        QDoubleSpinBox * rval = QObject::findChild<QDoubleSpinBox*>(QString("rvalue_%1").arg(i));
        QLineEdit * le = QObject::findChild<QLineEdit*>(QString("wzor_%1").arg(i));

        nierownosci << QString::number(lval->value());
        nierownosci << QString::number(rval->value());
        nierownosci << le->text();
        nierownosci << QString::number((int)m_content[i-1].m_cLeftRight);
        nierownosci << QString::number((int)rval->isVisible());
    }

    return nierownosci;
}

void InequalitiesPanel::deserialize(QStringList &data)
{
    for (int i=1; i <= MAX_INEQUALITIES; i++) {
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));
        ramka->setVisible(false);
    }
    for (int i=1; i <= MAX_INEQUALITIES && !data.isEmpty(); i++) {
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));
        QDoubleSpinBox * lval = QObject::findChild<QDoubleSpinBox*>(QString("lvalue_%1").arg(i));
        QDoubleSpinBox * rval = QObject::findChild<QDoubleSpinBox*>(QString("rvalue_%1").arg(i));
        QLineEdit * le = QObject::findChild<QLineEdit*>(QString("wzor_%1").arg(i));
        QPushButton * lnier = QObject::findChild<QPushButton*>(QString("lnierownosc_%1").arg(i));
        QPushButton * rnier = QObject::findChild<QPushButton*>(QString("rnierownosc_%1").arg(i));

        QString buf;
        buf = data.front();
        data.pop_front();
        lval->setValue(buf.toDouble());
        buf = data.front();
        data.pop_front();
        rval->setValue(buf.toDouble());
        buf = data.front();
        data.pop_front();
        le->setText(buf);
        buf = data.front();
        data.pop_front();
        m_content[i-1].m_cLeftRight = buf.toInt();
        buf = data.front();
        data.pop_front();
        m_content[i-1].m_bHaveRight = buf.toInt();
        rval->setVisible(m_content[i-1].m_bHaveRight);
        if (m_content[i-1].m_bHaveRight) {
            rnier->setIcon(QIcon(m_images[m_content[i-1].m_cLeftRight]));
            rnier->setIconSize(QSize(18, 18));
        }
        lnier->setIcon(QIcon(m_images[m_content[i-1].m_cLeftRight]));
        lnier->setIconSize(QSize(18, 18));

        ramka->setVisible(true);
    }
}

void InequalitiesPanel::on_dodaj_clicked()
{
    for (int i=1; i <= MAX_INEQUALITIES; i++) {//inequality_1
        QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(i));

        if (ramka->isHidden()) {
            ramka->setVisible(true);
            return;
        }
    }
}

void InequalitiesPanel::removeFormula()
{
    int senderNumer = sender()->property("numer").toInt();

    QFrame *ramka = QObject::findChild<QFrame*>(QString("inequality_%1").arg(senderNumer));

    if (ramka!=NULL) {
        ramka->hide();
    }
}

void InequalitiesPanel::changeLValue()
{
    int senderNumer = sender()->property("numer").toInt();
    QPushButton *lnier = qobject_cast<QPushButton*>(sender());

    rotIneq(lnier, (QPushButton*)NULL, senderNumer);
}

void InequalitiesPanel::addRValue()
{
    int senderNumer = sender()->property("numer").toInt();
    QPushButton *rnier = qobject_cast<QPushButton*>(sender());
    QDoubleSpinBox * rval = QObject::findChild<QDoubleSpinBox*>(QString("rvalue_%1").arg(senderNumer));

    if (rnier!=NULL && rval != NULL) {
        if (m_content[senderNumer-1].m_bHaveRight) {
            if (m_content[senderNumer-1].m_cLeftRight==0) {
                m_content[senderNumer-1].m_bHaveRight = false;
                rnier->setIcon(QIcon("://Images/add.png"));
                rval->setVisible(false);
                return;
            }
        } else
            m_content[senderNumer-1].m_bHaveRight = true;

        rotIneq((QPushButton*)NULL, rnier, senderNumer);
        rval->setVisible(true);
    }
}

void InequalitiesPanel::rotIneq(QPushButton *lnier, QPushButton *rnier, int idx)
{
    m_content[idx-1].m_cLeftRight = (++m_content[idx-1].m_cLeftRight)%4;

    if (lnier==NULL) {
        lnier = QObject::findChild<QPushButton*>(QString("lnierownosc_%1").arg(idx));
        if (lnier==NULL) {
            return;
        }
    }

    if (m_content[idx-1].m_bHaveRight) {
        if (rnier==NULL) {
            rnier = QObject::findChild<QPushButton*>(QString("rnierownosc_%1").arg(idx));
            if (rnier==NULL) {
                return;
            }
        }
        rnier->setIcon(QIcon(m_images[m_content[idx-1].m_cLeftRight]));
        rnier->setIconSize(QSize(18, 18));
    }

    lnier->setIcon(QIcon(m_images[m_content[idx-1].m_cLeftRight]));
    lnier->setIconSize(QSize(18, 18));
}
