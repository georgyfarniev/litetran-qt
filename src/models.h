#pragma once

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QVector>

struct Language
{
	QString code;
	QString name;
	bool enabled;

	Language(const QString &code_ = QString(), const QString &name_ = QString())
		:enabled(true), code(code_), name(name_)
	{
	}
};

class LanguageVector : public QVector<Language>
{
public:
    void setEnabledLanguages(const QStringList &codes);

    QStringList enabledLanguages() const;
};

class LanguageFilter : public QSortFilterProxyModel
{
public:
	LanguageFilter(QObject * parent = 0);
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class LanguageComboboxModel : public QAbstractTableModel
{
	Q_OBJECT
public:
    LanguageComboboxModel(LanguageVector &langs, QObject *parent = 0) :mLangs(langs), QAbstractTableModel(parent) {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return mLangs.size(); }
    int columnCount(const QModelIndex &parent) const override { return 1; }
	QModelIndex	parent(const QModelIndex & index) const override { return QModelIndex(); }
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	void reload();
    LanguageVector languages() const { return mLangs; }

    enum class Columns
    {
        Name,
    };
private:
	 LanguageVector &mLangs;
};
