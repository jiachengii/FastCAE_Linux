#include "ParaLinkageData.h"
#include "DataProperty/ParameterBase.h"
#include "DataProperty/ParameterGroup.h"

namespace FastCAEDesigner{
	ParaLinkageData::ParaLinkageData()
	{
	}


	ParaLinkageData::~ParaLinkageData()
	{
	}

	void ParaLinkageData::setParameterName(QString name)
	{
		_name = name;
	}

	void ParaLinkageData::setParameterType(QString name)
	{
		_caseName = name;
	}

	void ParaLinkageData::setActiveList(QList<DataProperty::ParameterBase*> list)
	{
		_activeList = list;
	}

	void ParaLinkageData::setDrivenList(QList<DataProperty::ParameterBase*> list)
	{
		_drivenList = list;
	}

	void ParaLinkageData::setDrivenGroupList(QList<DataProperty::ParameterGroup*> list)
	{
		_drivenGroupList = list;
	}

	QString ParaLinkageData::getParameterName()
	{
		return _name;
	}

	QString ParaLinkageData::getParameterType()
	{
		return _caseName;
	}

	QList<DataProperty::ParameterBase*> ParaLinkageData::getActiveList()
	{
		return _activeList;
	}

	QList<DataProperty::ParameterBase*> ParaLinkageData::getDrivenList()
	{
		return _drivenList;
	}

	QList<DataProperty::ParameterGroup*> ParaLinkageData::getDrivenGroupList()
	{
		return _drivenGroupList;
	}

}