#include <QMainWindow>
#include <QDialog>

class QTreeWidget;
class QLineEdit;
class QListWidget;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QTextEdit;
class QCheckBox;

class CPropertyIOBase : public QDialog
{
public:
    enum BaseType
    {
        IO = 0,
        PROPERTY,
        INVALID,
    };
    CPropertyIOBase(QWidget* parent){};
    virtual QString parse() { return {}; };
    virtual void fromString(const QString& str) {};
    virtual QString type() { return "INVALID"; };
    virtual BaseType baseType() { return INVALID; };
    virtual QString name() { return "INVALID"; };
};

class CPropertyCreator : public CPropertyIOBase
{
    QLineEdit* propertyName;
    QComboBox* propertyType;
    QCheckBox* readOnly;
    QCheckBox* report;
    QLineEdit* propertyDisplayName;
    QLineEdit* propertyDefaultValue;
    QLineEdit* propertyDescription;

public:
    explicit CPropertyCreator(QWidget* parent);
    QString parse() override;
    void fromString(const QString& str) override;
    QString type() override;
    BaseType baseType() override { return BaseType::PROPERTY; };
    QString name() override;

};

class CIOCreator : public CPropertyIOBase
{
    QComboBox *ioType;
    QLineEdit* ioName;
    QComboBox* ioValueType;
    QTextEdit *ioDescription;
public:
    explicit CIOCreator(QWidget* parent);
    QString parse() override;
    void fromString(const QString& str) override;
    QString type() override;
    BaseType baseType() override { return BaseType::IO; };
    QString name() override;
};

class CEntityCreator : public QDialog
{
    QComboBox *classType;
    QTextEdit *classProperties;
    QLineEdit *className;
    QTextEdit *classDescription;
    QComboBox *classParent;


    QListWidget *classParentListWidget;

    std::map<uintptr_t, CPropertyIOBase*> propertyList;
//    std::map<intptr_t, CIOCreator> ioList;
    QListWidget *propertyListWidget;

public:
    explicit CEntityCreator(QWidget* parent);
    void provideParentList(const QStringList& parents);
    QString parse();
    void fromString(const QString& str);
};

class CCentralWidget : public QWidget
{
public:
    explicit CCentralWidget(QWidget* parent);
    QListWidget* classTypeList;
    QLineEdit* customTypeEdit;
    QComboBox* classPropertiesCombo;
    QLineEdit* customProperty;

    //Axis
    QDoubleSpinBox* axisX1;
    QDoubleSpinBox* axisY1;
    QDoubleSpinBox* axisZ1;
    QDoubleSpinBox* axisX2;
    QDoubleSpinBox* axisY2;
    QDoubleSpinBox* axisZ2;


    //Color
    QSpinBox* colorR;
    QSpinBox* colorG;
    QSpinBox* colorB;

    //Cylinder
    QSpinBox* cylinderR;
    QSpinBox* cylinderG;
    QSpinBox* cylinderB;

    QLineEdit* classNameEdit;
    QTreeWidget* fgdList;
};

class CMainWindow : public QMainWindow
{
public:
    explicit CMainWindow(QWidget* parent);
};