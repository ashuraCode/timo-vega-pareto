#ifndef INEQUALITIESPANEL_H
#define INEQUALITIESPANEL_H

#include <QWidget>
#include <QPushButton>
#define MAX_INEQUALITIES 10

namespace Ui {
class InequalitiesPanel;
}

struct InequalityStruc {
    bool m_bHaveRight;
    char m_cLeftRight;
    double m_dLeft;
    std::string m_sFormula;
    double m_dRight;

    InequalityStruc()
        :   m_bHaveRight(false)
        ,   m_cLeftRight(0)
        ,   m_dLeft(0)
        ,   m_sFormula("")
        ,   m_dRight(0)
    {}
}; //ge, gt, le, lt

class InequalitiesPanel : public QWidget
{
    Q_OBJECT

    static std::vector<std::string> m_sSign;
public:
    explicit InequalitiesPanel(QWidget *parent = 0);
    ~InequalitiesPanel();

    typedef std::pair<std::string, std::string> string_pair;
    std::vector<string_pair> createFormula();

    QStringList serialize();
    void deserialize(QStringList &data);

private slots:
    void on_dodaj_clicked();

    void removeFormula();
    void changeLValue();
    void addRValue();

private:
    Ui::InequalitiesPanel *ui;
    InequalityStruc m_content[MAX_INEQUALITIES];
    QStringList m_images;

    void rotIneq(QPushButton *l, QPushButton *r, int i);
};

#endif // INEQUALITIESPANEL_H
