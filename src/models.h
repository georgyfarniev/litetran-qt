#pragma once

#include <QAbstractItemModel>
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

typedef QVector<Language> LanguageVector;

class LanguageFilter : public QSortFilterProxyModel
{
public:
	LanguageFilter(QObject * parent = 0);

	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class LanguageComboboxModel : public QAbstractItemModel
{
public:
	LanguageComboboxModel(LanguageVector &langs, QObject *parent = 0);
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex	parent(const QModelIndex & index) const override;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	void reload();
private:
	 LanguageVector &mLangs;

	 enum class Columns
	 {
		 Name,
		 State
	 };
};
