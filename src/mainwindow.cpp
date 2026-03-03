#include <QDirIterator>
#include <toolpp/FGD.h>
#include <QGridLayout>
#include "mainwindow.h"

const QStringList classTypes = {"@BaseClass", "@SolidClass", "@PointClass", "@NPCClass", "@KeyFrameClass", "@MoveClass", "@FilterClass"};
const QStringList propertyTypes = {"void", "string", "integer", "float", "boolean"}; //Flags and Choices are handled separately.

#include <QTreeWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent) {

    this->setCentralWidget(new CCentralWidget(this));

}

CCentralWidget::CCentralWidget(QWidget *parent)
{
//    QDirIterator snippetIt{"/media/trico/Storage Drive X2/GithubProjects/HammerAddons/fgd/", {"*/snippets/*.fgd"},QDir::NoDotAndDotDot | QDir::Files,QDirIterator::FollowSymlinks | QDirIterator::Subdirectories };
    qInfo() << "/home/trico/Documents/pettuce/p2ce/p2ce.fgd";
    auto fgd = toolpp::FGD("/home/trico/Documents/pettuce/p2ce/p2ce.fgd");

    QStringList classTypesList;

    for(const auto& entity : fgd.getEntities())
    {
        classTypesList << std::string(entity.first).c_str();
    }
    classTypesList.sort();
    auto newEntity = CEntityCreator(this);
    newEntity.provideParentList(classTypesList);
    if(!newEntity.exec()) {
        this->close();
        return;
    }

    auto stringWishList = new QTextEdit(this);
    stringWishList->setText(newEntity.parse());
    auto layout = QGridLayout(this);
    layout.addWidget(stringWishList);

}

CEntityCreator::CEntityCreator(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Add Entity Class"));
    auto layout = new QGridLayout(this);

    auto classTypeLabel = new QLabel(tr("Entity Class Type:"),this);
    layout->addWidget(classTypeLabel,0,0, Qt::AlignCenter);
    this->classType = new QComboBox(this);
    for(const auto& type : classTypes)
    {
        this->classType->addItem(type);
    }
    layout->addWidget(this->classType,0,1);

    auto classNameLabel = new QLabel(tr("Entity Class Name:"), this);
    layout->addWidget(classNameLabel,0,2);
    this->className = new QLineEdit(this);
    this->className->setMaxLength(63);
    layout->addWidget(this->className,0,3);

    auto classParentLabel = new QLabel(tr("Add Class Base:"));
    layout->addWidget(classParentLabel,1,0);
    this->classParent = new QComboBox(this);
    layout->addWidget(this->classParent,1, 1);
    auto addParentButton = new QPushButton(tr("Add:"),this);
    layout->addWidget(addParentButton, 1, 2, 1, 2);
    auto classParentListLabel = new QLabel(tr("Bases"),this);
    layout->addWidget(classParentListLabel, 2,0,1,4, Qt::AlignCenter);
    this->classParentListWidget = new QListWidget(this);
    layout->addWidget(this->classParentListWidget, 3, 0, 1, 4);
    auto removeParentButton = new QPushButton(tr("Remove Class Base"),this);
    layout->addWidget(removeParentButton, 4, 0, 1, 4);

    auto classDescriptionLabel = new QLabel(tr("Entity Class Description"));
    layout->addWidget(classDescriptionLabel, 5, 0, 1, 4, Qt::AlignCenter);
    this->classDescription = new QTextEdit(this);
    layout->addWidget(this->classDescription, 6, 0, 1, 4);

    auto classPropertiesLabel = new QLabel("Entity Class Properties",this);
    layout->addWidget(classPropertiesLabel, 7, 0, 1, 4, Qt::AlignCenter);
    this->classProperties = new QTextEdit(this);
    layout->addWidget(this->classProperties, 8, 0, 1, 4);

    auto propertyAddButton = new QPushButton(tr("Add KV"), this);
    layout->addWidget(propertyAddButton,9 ,0, 1, 2);

    auto ioAddButton = new QPushButton(tr("Add IO"), this);
    layout->addWidget(ioAddButton,9 ,2, 1, 2);

    auto propertyListWidgetLabel = new QLabel(tr("Properties"));
    layout->addWidget(propertyListWidgetLabel,10 ,0, 1, 4, Qt::AlignCenter);

    propertyListWidget = new QListWidget(this);
    layout->addWidget(propertyListWidget,11 ,0, 1, 4);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox, 12 ,0, 1, 4);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(addParentButton, &QPushButton::clicked, this, [&]{
        if(this->classParentListWidget->findItems(this->classParent->currentText(), Qt::MatchFlag::MatchCaseSensitive).isEmpty())
            this->classParentListWidget->addItem(this->classParent->currentText());
    });
    connect(removeParentButton, &QPushButton::clicked, this, [&]{
        if(!this->classParentListWidget->selectedItems().isEmpty())
            this->classParentListWidget->takeItem(this->classParentListWidget->currentRow());
    });

    connect(propertyAddButton, &QPushButton::clicked, this, [&]{
        auto property = new CPropertyCreator(this);
        if(!property->exec())
            return;

        auto raw = reinterpret_cast<uintptr_t>(property);
        this->propertyList[raw] = property;

        auto item = new QListWidgetItem(property->type() + " " + property->name(),this->propertyListWidget);
        item->setData(Qt::UserRole, quint64(raw));
    });

    connect(ioAddButton, &QPushButton::clicked, this, [&]{
        auto property = new CIOCreator(this);
        if(!property->exec())
            return;

        auto raw = reinterpret_cast<uintptr_t>(property);
        this->propertyList[raw] = property;

        auto item = new QListWidgetItem(property->type() + " " + property->name(),this->propertyListWidget);
        item->setData(Qt::UserRole, quint64(raw));
    });

}

void CEntityCreator::provideParentList(const QStringList& parents)
{
    if(!this->classParent)
        return;

    this->classParent->clear();

    for(const auto& classParentItem  : parents)
    {
        this->classParent->addItem(classParentItem);
    }
}

QString CEntityCreator::parse() {
    QString entity;
    entity += this->classType->currentText();
    entity += " ";
    entity += "base( ";
//    entity +=
    for(int i = 0; i < this->classParentListWidget->count(); i++)
    {
        auto item = this->classParentListWidget->item(i);
        entity += item->text();
        if(i < this->classParentListWidget->count() - 1)
            entity += ", ";
    }
    entity += " ) ";
    entity += this->classProperties->toPlainText(); //We hail merry that the user formatted this correctly.
    entity += "\n = ";
    entity += this->className->text();
    entity += ": ";
    for(int i = 0; i < this->classDescription->toPlainText().length(); i += 125)
    {
        if(i * 125 + 125 < this->classDescription->toPlainText().length())
            entity += "\"" + this->classDescription->toPlainText().mid(i, 125) + "\" + ";
        else
            entity += "\"" + this->classDescription->toPlainText().mid(i) + "\"";
    }
    entity += "\n   [\n    ";

    std::vector<CPropertyIOBase*> ios;

    for(const auto& item : this->propertyList)
    {
        if(item.second->baseType() == CPropertyIOBase::IO)
        {
            ios.push_back(item.second);
            continue;
        }
        entity += item.second->parse();
        entity += "\n   ";
    }

    for(const auto& io : ios)
    {
        entity += io->parse();
        entity += "\n   ";
    }

    entity += "   ]";

    return entity;
}

QString CPropertyCreator::type() {
    return tr("Property");
}

QString CPropertyCreator::name() {
    return this->propertyName->text() + " (" + this->propertyType->currentText() +") " + this->propertyDisplayName->text();
}

CPropertyCreator::CPropertyCreator(QWidget *parent) : CPropertyIOBase(parent)
{
    auto layout = new QGridLayout(this);

    auto propertyNameLabel = new QLabel(tr("Property Name:"));
    layout->addWidget(propertyNameLabel, 0, 0);

    this->propertyName = new QLineEdit(this);
    layout->addWidget(this->propertyName, 0, 1);

    auto propertyTypeLabel = new QLabel(tr("Type:"));
    layout->addWidget(propertyTypeLabel, 1, 0);

    this->propertyType = new QComboBox(this);
    for(const auto& value : propertyTypes)
    {
        this->propertyType->addItem(value);
    }
    layout->addWidget(this->propertyType, 1, 1);

    this->readOnly = new QCheckBox("Read Only",this);
    layout->addWidget(this->readOnly, 2, 0);

    this->report = new QCheckBox(tr("Report"),this);
    layout->addWidget(this->report, 3, 0);

    auto propertyDisplayNameLabel = new QLabel(tr("Display Name:"));
    layout->addWidget(propertyDisplayNameLabel, 4, 0);

    this->propertyDisplayName = new QLineEdit(this);
    layout->addWidget(this->propertyDisplayName, 4, 1);

    auto propertyDefaultValueLabel = new QLabel(tr("Default Value:"));
    layout->addWidget(propertyDefaultValueLabel, 5, 0);

    this->propertyDefaultValue = new QLineEdit(this);
    layout->addWidget(this->propertyDefaultValue, 5, 1);

    auto propertyDescriptionLabel = new QLabel(tr("Description"));
    layout->addWidget(propertyDescriptionLabel, 6, 0);

    this->propertyDescription = new QLineEdit(this);
    layout->addWidget(this->propertyDescription, 6, 1);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox, 7 ,0, 1, 4);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

QString CPropertyCreator::parse() {
    return this->propertyName->text() + "(" + this->propertyType->currentText() + ") : \"" + this->propertyDisplayName->text() + "\" : \"" + this->propertyDefaultValue->text() + "\" : \"" + this->propertyDescription->text() + "\"";
}

void CPropertyCreator::fromString(const QString &str) {
    CPropertyIOBase::fromString(str);
}

QString CIOCreator::type() {
    return this->ioType->currentText();
}

QString CIOCreator::name() {
    return this->ioName->text() + "(" + this->ioValueType->currentText()+ ")";
}

CIOCreator::CIOCreator(QWidget *parent) : CPropertyIOBase(parent)
{
    auto layout = new QGridLayout(this);

    auto ioTypeLabel = new QLabel(tr("Type:"));
    layout->addWidget(ioTypeLabel, 0, 0);

    this->ioType = new QComboBox(this);
    this->ioType->addItem("input");
    this->ioType->addItem("output");
    layout->addWidget(this->ioType, 0, 1);

    auto ioNameLabel = new QLabel(tr("Name:"));
    layout->addWidget(ioNameLabel, 1, 0);

    this->ioName = new QLineEdit(this);
    layout->addWidget(this->ioName, 1, 1);

    auto ioValueTypeLabel = new QLabel(tr("Value Type:"));
    layout->addWidget(ioValueTypeLabel, 2, 0);

    this->ioValueType = new QComboBox(this);
    for(const auto& value : propertyTypes)
    {
        this->ioValueType->addItem(value);
    }
    layout->addWidget(this->ioValueType, 2, 1);

    auto ioDescriptionLabel = new QLabel(tr("Description"));
    layout->addWidget(ioDescriptionLabel, 3, 0, 1, 2);

    this->ioDescription = new QTextEdit(this);
    layout->addWidget(this->ioDescription, 4, 0, 1, 2);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox, 5 ,0, 1, 4);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


}

QString CIOCreator::parse() {
    return this->ioType->currentText() + " " + this->ioName->text() + "("+this->ioValueType->currentText()+") : \"" + this->ioDescription->toPlainText() + "\"";
}

void CIOCreator::fromString(const QString &str) {
    CPropertyIOBase::fromString(str);
}
