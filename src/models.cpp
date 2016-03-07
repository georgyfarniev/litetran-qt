#include "models.h"

LanguageFilter::LanguageFilter(QObject *parent) : QSortFilterProxyModel(parent) {}

bool LanguageFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	return sourceModel()->data(sourceModel()->index(sourceRow, 1, sourceParent), Qt::CheckStateRole).toBool();
}

//-----------------------------------------------------------------------------

LanguageComboboxModel::LanguageComboboxModel(LanguageVector &langs, QObject *parent) :mLangs(langs), QAbstractItemModel(parent) {}

int LanguageComboboxModel::rowCount(const QModelIndex &parent) const
{
	return mLangs.size();
}

int LanguageComboboxModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QModelIndex LanguageComboboxModel::index(int row, int column, const QModelIndex &parent) const
{
	if (row <= mLangs.size())
		return createIndex(row, column, (void*)&mLangs.at(row).name);
	return QModelIndex();
}

QModelIndex LanguageComboboxModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

QVariant LanguageComboboxModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole && index.column() == 0)
		return mLangs.at(index.row()).name;
	else if (index.column() == 1 && role == Qt::CheckStateRole)
		return mLangs.at(index.row()).enabled ? Qt::Checked : Qt::Unchecked;

	return QVariant();
}

bool LanguageComboboxModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::CheckStateRole && index.column() == 1 && mLangs.size() > index.row())
	{
		mLangs[index.row()].enabled = value.toBool();
		return true;
	}
	return false;
}

Qt::ItemFlags LanguageComboboxModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags aflags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
	if (index.column() == 1)
		aflags |= Qt::ItemIsEditable;
	return aflags;
}

void LanguageComboboxModel::reload()
{
	beginResetModel();
	endResetModel();
}
