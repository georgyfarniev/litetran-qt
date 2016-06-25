#include "models.h"
#include <QIcon>

void LanguageVector::setEnabledLanguages(const QStringList &codes)
{
    if (codes.isEmpty())
        return;

    for(Language &lang : *this)
        lang.enabled = codes.contains(lang.code);
}

QStringList LanguageVector::enabledLanguages() const
{
    QStringList codes;
    for(const Language &lang : *this)
    {
        if (lang.enabled)
            codes << lang.code;
    }

    return codes;
}

//-----------------------------------------------------------------------------

LanguageFilter::LanguageFilter(QObject *parent) : QSortFilterProxyModel(parent) {}

bool LanguageFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	return sourceModel()->data(sourceModel()->index(sourceRow, 1, sourceParent), Qt::CheckStateRole).toBool();
}

//-----------------------------------------------------------------------------

QVariant LanguageComboboxModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid())
	{
		if(role == Qt::DisplayRole && index.column() == (int)Columns::Name)
			return mLangs.at(index.row()).name;
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

void LanguageComboboxModel::reload()
{
	beginResetModel();
	endResetModel();
}
