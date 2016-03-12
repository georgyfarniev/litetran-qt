#include "models.h"
#include <QIcon>

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
	if (index.isValid())
	{
		if(role == Qt::DisplayRole && index.column() == (int)Columns::Name)
			return mLangs.at(index.row()).name;
//		else if (role == Qt::DecorationRole && index.column() == (int)Columns::Name)
//			return QIcon(":/icons/flags/" + mLangs.at(index.row()).code.toUpper() + ".png");
		else if (role == Qt::CheckStateRole && index.column() == (int)Columns::State)
			return mLangs.at(index.row()).enabled ? Qt::Checked : Qt::Unchecked;
	}

	return QVariant();
}

bool LanguageComboboxModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::CheckStateRole && index.column() == (int)Columns::State)
	{
		mLangs[index.row()].enabled = value.toBool();
		return true;
	}
	return false;
}

Qt::ItemFlags LanguageComboboxModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags aflags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
	if (index.column() == (int)Columns::State)
		aflags |= Qt::ItemIsEditable;
	return aflags;
}

QVariant LanguageComboboxModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
		return section == (int)Columns::Name ? "Name" : "#";
	return QVariant();
}

void LanguageComboboxModel::reload()
{
	beginResetModel();
	endResetModel();
}
