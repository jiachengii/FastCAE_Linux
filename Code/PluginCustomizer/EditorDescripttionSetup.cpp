#include "EditorDescripttionSetup.h"
#include "ui_EditorDescripttionSetup.h"
#include "CustomParameterModel.h"
#include <QMenu>
#include <QAction>
#include "EditorIntValue.h"
#include "EditorBoolValue.h"
#include "EditorDoubleValue.h"
#include "EditorStringValue.h"
#include "EditorEnumValue.h"
#include "EditorTableValue.h"
#include "EditorNameValue.h"
#include "EditorPathValue.h"
#include "DataProperty/ParameterInt.h"
#include "DataProperty/ParameterBool.h"
#include "DataProperty/ParameterDouble.h"
#include "DataProperty/ParameterString.h" 
#include "DataProperty/ParameterSelectable.h"
#include "DataProperty/ParameterTable.h"
#include "DataProperty/DataBase.h"
#include "DataProperty/ParameterGroup.h"
#include "DataProperty/ParameterPath.h"
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>
#include <QMessageBox>
#include "InputValidator.h"
#include <QFileDialog>
#include "DataProperty/modelTreeItemType.h"

#pragma execution_character_set("utf-8")

namespace FastCAEDesigner
{
	EditorDescripttionSetup::EditorDescripttionSetup(QTreeWidgetItem* treeItem, CustomParameterModel* model, QWidget *parent) :
		QDialog(parent),
		ui(new Ui::EditorDescripttionSetup),
		_model(model),
		_treeItem(treeItem)
	{
		ui->setupUi(this);
		//resizeEvent(0);
		Init();
		//ui->groupBox->hide();
	}

	EditorDescripttionSetup::~EditorDescripttionSetup()
	{
		delete ui;
	}

	void EditorDescripttionSetup::resizeEvent(QResizeEvent *e)
	{
		ResizeTableWidget(ui->tableWidget_PList, _paraListColNum);
		ResizeTableWidget(ui->tableWidget_GList, _groupListColNum);
		ResizeTableWidget(ui->tableWidget_GPList, _paraListColNum);
	}

	void EditorDescripttionSetup::ResizeTableWidget(QTableWidget* widget, int colNum)
	{
		int totalwidth = widget->width();

		if (widget->verticalScrollBar()->isVisible())
			totalwidth -= widget->verticalScrollBar()->width();

		for (int i = 0; i < colNum; i++)
			widget->setColumnWidth(i, totalwidth / colNum - 4);
	}

	void EditorDescripttionSetup::showEvent(QShowEvent *e)
	{
		resizeEvent(nullptr);
	}

	void EditorDescripttionSetup::Init()
	{
		//����ڵ�Ϊ���桢��������ػ�����Ϣ�༭UI
		if (_model->GetType() == TreeItemType::ProjectSimulationSetting ||
			_model->GetType() == TreeItemType::ProjectSolver)
		{
			ui->groupBox->setHidden(true);
		}


		QString title = _model->GetChnName();
		setWindowTitle(title);
		UpdateDataToUi();
		//SetIsEdit(_model->GetIsEdit()); 
		connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(OnBtnOkClicked()));
		connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
		connect(ui->btnLoadIcon, SIGNAL(clicked()), this, SLOT(OnBtnLoadIconClicked()));
		CreateAddParameterMenu();

		//�����б���ť��
		connect(ui->btnAdd_s, SIGNAL(clicked()), this, SLOT(OnBtnAddParaClicked()));
		connect(ui->btnEdit_s, SIGNAL(clicked()), this, SLOT(OnBtnEditParaClicked()));
		connect(ui->btnDel_s, SIGNAL(clicked()), this, SLOT(OnBtnDelParaClicked()));
		connect(ui->btnDelAll_s, SIGNAL(clicked()), this, SLOT(OnBtnClearAllParaClicked()));
		connect(ui->tableWidget_PList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(TableWidgetParaDoubleClicked(QModelIndex)));
		connect(ui->tableWidget_PList, SIGNAL(clicked(QModelIndex)), this, SLOT(TableWidgetParaClicked(QModelIndex)));
		//connect(ui->tableWidget_PList, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(TableWidgetParaCurrentCellChanged()));

		//�������б���ť��
		connect(ui->btnAdd_G, SIGNAL(clicked()), this, SLOT(OnBtnAddGroupClicked()));
		connect(ui->btnEdit_G, SIGNAL(clicked()), this, SLOT(OnBtnEditGroupClicked()));
		connect(ui->btnDel_G, SIGNAL(clicked()), this, SLOT(OnBtnDelGroupClicked()));
		connect(ui->btnClearAll_G, SIGNAL(clicked()), this, SLOT(OnBtnClearAllGroupClicked()));
		connect(ui->tableWidget_GList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(TableWidgetGoupDoubleClicked(QModelIndex)));
		connect(ui->tableWidget_GList, SIGNAL(clicked(QModelIndex)), this, SLOT(TableWidgetGroupClicked(QModelIndex)));
		connect(ui->tableWidget_GList, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(TableWidgetGroupCurrentCellChanged()));

		//������--�����б���ť��
		connect(ui->btnAdd_S_G, SIGNAL(clicked()), this, SLOT(OnBtnAddGroupParaClicked()));
		connect(ui->btnEdit_S_G, SIGNAL(clicked()), this, SLOT(OnBtnEditGroupParaClicked()));
		connect(ui->btnDel_S_G, SIGNAL(clicked()), this, SLOT(OnBtnDelGroupParaClicked()));
		connect(ui->btnClearAll_S_G, SIGNAL(clicked()), this, SLOT(OnBtnClearAllGroupParaClicked()));
		connect(ui->tableWidget_GPList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(TableWidgetGroupParaDoubleClicked(QModelIndex)));
		connect(ui->tableWidget_GPList, SIGNAL(clicked(QModelIndex)), this, SLOT(TableWidgetGroupParaClicked(QModelIndex)));
		connect(ui->tableWidget_GPList, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(TableWidgetGroupParaCurrentCellChanged()));
		
		connect(ui->btnShowParameterGroup, SIGNAL(clicked()), this, SLOT(OnBtnShowGroupClicked()));
		ui->groupBox_3->hide();

		if (TreeItemType::MaterialChild == _model->GetType())
		{
			//ui->txtIcon->setText(Image_Material);
			ui->txtIcon->setEnabled(false);
			ui->btnLoadIcon->setEnabled(false);
			ui->lbl_chnName->setText(tr("Name"));
			ui->txtEnglishName->hide();
			ui->lbl_engName->hide();
			connect(ui->txtChineseName, SIGNAL(textChanged(QString)), ui->txtEnglishName, SLOT(setText(QString)));
		}

		ui->tableWidget_GList->setMaximumHeight(160);
	}

	//���������б�
	void EditorDescripttionSetup::FillParameterList()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		QList<DataProperty::ParameterBase*> paraList = dataBase->getParaList();
		int count = paraList.count();
		ui->tableWidget_PList->reset();
		ui->tableWidget_PList->clear();
		ui->tableWidget_PList->setRowCount(count);
		ui->tableWidget_PList->setColumnCount(4);
		//ui->tableWidget->setColumnWidth(0,50);
		//ui->tableWidget->setColumnWidth(1,335);

		QStringList headerList;
		headerList.append(tr("No"));
		headerList.append(tr("Name"));
		headerList.append(tr("Type"));
		headerList.append(tr("Value"));

		ui->tableWidget_PList->setHorizontalHeaderLabels(headerList);
		//ui->tableWidget_PList->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		//ui->tableWidget_PList->horizontalHeader()->setVisible(false);
		ui->tableWidget_PList->verticalHeader()->setVisible(false);

		ui->tableWidget_PList->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ��һ��
		ui->tableWidget_PList->setSelectionMode(QAbstractItemView::SingleSelection); //����ֻ��ѡ��һ�У����ܶ���ѡ��
		ui->tableWidget_PList->setEditTriggers(QAbstractItemView::NoEditTriggers);   //����ÿ�����ݲ��ɸ���
		ui->tableWidget_PList->setAlternatingRowColors(false);                        //���ø�һ�б�һ��ɫ������һ��һ��
		//ui->tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
		//ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui->tableWidget_PList->resizeColumnsToContents();
		ui->tableWidget_PList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		for (int i = 0; i < count; i++)
		{
			DataProperty::ParameterBase *parameter = paraList.at(i);

			if (nullptr == parameter)
				return;

			QStringList list = GetModelString(parameter);

			if (list.count() < 3)
				return;

			ui->tableWidget_PList->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
			ui->tableWidget_PList->setItem(i, 1, new QTableWidgetItem(list[0]));
			ui->tableWidget_PList->setItem(i, 2, new QTableWidgetItem(list[1]));
			ui->tableWidget_PList->setItem(i, 3, new QTableWidgetItem(list[2]));

			ui->tableWidget_PList->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui->tableWidget_PList->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui->tableWidget_PList->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui->tableWidget_PList->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		resizeEvent(0);

		if (count > 0)
			ui->btnDelAll_s->setEnabled(true);
		else
			ui->btnDelAll_s->setEnabled(false);
	}

	//��ָ�������ؼ������Ӳ����б�
	void EditorDescripttionSetup::FillParameterList(DataProperty::ParameterList* parameterList, QTableWidget* tableWidget)
	{
		if (nullptr == parameterList)
			return;

		QList<DataProperty::ParameterBase*> paraList = parameterList->getParaList();
		//int count = parameterList->getParameterCount();
		int count = paraList.count();
		tableWidget->reset();
		tableWidget->clear();
		tableWidget->setRowCount(count);
		tableWidget->setColumnCount(4);
		
		QStringList headerList;
		headerList.append(tr("No"));
		headerList.append(tr("Name"));
		headerList.append(tr("Type"));
		headerList.append(tr("Value"));

		tableWidget->setHorizontalHeaderLabels(headerList);
		//ui->tableWidget_PList->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		//ui->tableWidget_PList->horizontalHeader()->setVisible(false);
		tableWidget->verticalHeader()->setVisible(false);

		tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ��һ��
		tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //����ֻ��ѡ��һ�У����ܶ���ѡ��
		tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //����ÿ�����ݲ��ɸ���
		tableWidget->setAlternatingRowColors(false);                        //���ø�һ�б�һ��ɫ������һ��һ��
		//ui->tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
		//ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		tableWidget->resizeColumnsToContents();
		tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		for (int i = 0; i < count; i++)
		{
			//DataProperty::ParameterBase *parameter = parameterList->getParameterAt(i);
			DataProperty::ParameterBase *parameter = paraList.at(i);

			if (nullptr == parameter)
				continue;

			QStringList list = GetModelString(parameter);

			if (list.count() < 3)
				continue;

			tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
			tableWidget->setItem(i, 1, new QTableWidgetItem(list[0]));
			tableWidget->setItem(i, 2, new QTableWidgetItem(list[1]));
			tableWidget->setItem(i, 3, new QTableWidgetItem(list[2]));

			tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		resizeEvent(0);
	}
	void EditorDescripttionSetup::FillParameterList( QTableWidget* tableWidget)
	{
		tableWidget->reset();
		tableWidget->clear();
		tableWidget->setRowCount(0);
		tableWidget->setColumnCount(4);

		QStringList headerList;
		headerList.append(tr("No"));
		headerList.append(tr("Name"));
		headerList.append(tr("Type"));
		headerList.append(tr("Value"));

		tableWidget->setHorizontalHeaderLabels(headerList);
		//ui->tableWidget_PList->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		//ui->tableWidget_PList->horizontalHeader()->setVisible(false);
		tableWidget->verticalHeader()->setVisible(false);

		tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ��һ��
		tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //����ֻ��ѡ��һ�У����ܶ���ѡ��
		tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //����ÿ�����ݲ��ɸ���
		tableWidget->setAlternatingRowColors(false);                        //���ø�һ�б�һ��ɫ������һ��һ��
		//ui->tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
		//ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		tableWidget->resizeColumnsToContents();
		tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		resizeEvent(0);
	}

	//���Ӳ����������б�
	void EditorDescripttionSetup::FillGroupList()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		int count = dataBase->getParameterGroupCount();
		//QList<DataProperty::ParameterGroup*> paraGroupList;
		QList<QString> groupNameList;

		for (int i = 0; i < count; i++)
		{
			DataProperty::ParameterGroup* group = dataBase->getParameterGroupAt(i);
			if (nullptr == group)
				continue;
			groupNameList.append(group->getDescribe());
		}

		count = groupNameList.count();
		ui->tableWidget_GList->reset();
		ui->tableWidget_GList->clear();
		ui->tableWidget_GList->setRowCount(count);
		ui->tableWidget_GList->setColumnCount(2);

		QStringList headerList;
		headerList.append(tr("No"));
		headerList.append(tr("Name"));
		//headerList.append(tr("Type"));
		//headerList.append(tr("Value"));

		ui->tableWidget_GList->setHorizontalHeaderLabels(headerList);
		//ui->tableWidget_PList->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
		//ui->tableWidget_PList->horizontalHeader()->setVisible(false);
		ui->tableWidget_GList->verticalHeader()->setVisible(false);

		ui->tableWidget_GList->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ��һ��
		ui->tableWidget_GList->setSelectionMode(QAbstractItemView::SingleSelection); //����ֻ��ѡ��һ�У����ܶ���ѡ��
		ui->tableWidget_GList->setEditTriggers(QAbstractItemView::NoEditTriggers);   //����ÿ�����ݲ��ɸ���
		ui->tableWidget_GList->setAlternatingRowColors(false);                        //���ø�һ�б�һ��ɫ������һ��һ��
		//ui->tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
		//ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui->tableWidget_GList->resizeColumnsToContents();
		ui->tableWidget_GList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		for (int i = 0; i < count; i++)
		{
			QString name = groupNameList.at(i);

			ui->tableWidget_GList->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
			ui->tableWidget_GList->setItem(i, 1, new QTableWidgetItem(name));
			//ui->tableWidget_PList->setItem(i, 2, new QTableWidgetItem(list[1]));
			//ui->tableWidget_PList->setItem(i, 3, new QTableWidgetItem(list[2]));

			ui->tableWidget_GList->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui->tableWidget_GList->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//ui->tableWidget_PList->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//ui->tableWidget_PList->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}

		ui->tableWidget_GList->selectRow(count - 1);//ѡ�����һ��
		//resizeEventGroupNameList();
		//ResizeTableWidget(ui->tableWidget_GList, _groupListColNum);
		//_currentParameterGroup = nullptr;                   
		//_currentParameterGroupList_Model = nullptr; 
		resizeEvent(0);
	}

	//��ȡ������Ҫ��ʾ���ַ����б�
	QStringList EditorDescripttionSetup::GetModelString(DataProperty::ParameterBase* parameter)
	{
		QStringList strList;

		if (nullptr == parameter)
			return strList;

		QString name = parameter->getDescribe();
		QString type = parameter->ParaTypeToString(parameter->getParaType());
		QString value;

		if (parameter->getParaType() == DataProperty::ParaType::Para_Table)
		{
			int rows = ((DataProperty::ParameterTable*)(parameter))->getRowCount();
			int columns = ((DataProperty::ParameterTable*)(parameter))->getColumnCount();
			value = QString("%1x%2").arg(rows).arg(columns);
		}
		else if (parameter->getParaType() == DataProperty::ParaType::Para_Double)
		{
			int accuracy = ((DataProperty::ParameterDouble*)(parameter))->getAccuracy();
			double d = ((DataProperty::ParameterDouble*)(parameter))->getValue();
			value = QString("%1").arg(d, 0, 'f', accuracy, '0');
		}
		else
		{
			value = parameter->valueToString();
		}

		strList.append(name);
		strList.append(type);
		strList.append(value);
		return strList;
	}

	//�����б� -- ���ӵĲ���--�ۺ���
	void EditorDescripttionSetup::OnBtnAddParaClicked()
	{
		_currentOpObject = ParaList;
		topMenu->exec(QCursor::pos());
	}

	//�����б� -- �༭����--�ۺ���
	void EditorDescripttionSetup::OnBtnEditParaClicked()
	{
		int rowNo = ui->tableWidget_PList->currentRow();

		if (rowNo < 0)
			return;

		DataProperty::ParameterBase* model = GetCurrentSelecctedParameter();

		if (nullptr == model)
			return;
		
		EditModel(model);
		ui->tableWidget_PList->selectRow(rowNo);
	}

	//�����б� --ɾ������--�ۺ���
	void EditorDescripttionSetup::OnBtnDelParaClicked()
	{
		int rowNo = ui->tableWidget_PList->currentRow();
		DataProperty::ParameterBase* model = GetCurrentSelecctedParameter();
		
		if (nullptr == model)
			return;

		QString title = tr("Warning");
		QString msg = tr("Deleted Parameter %1?").arg(model->getDescribe());
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);
		
		if (QMessageBox::Yes == result)
		{
			DeleteParameter(model);
			FillParameterList();
		}
		
		DataProperty::DataBase* dataBase = _model->GetDataBase();
		int count = dataBase->getParameterCount();
		
		if (0 == count)
			return;

		rowNo = (rowNo == count) ? count - 1 : rowNo;
		ui->tableWidget_PList->selectRow(rowNo);
	}

	//�����б� -- ������Ѳ���--�ۺ���
	void EditorDescripttionSetup::OnBtnClearAllParaClicked()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();
		int count = dataBase->getParameterCount();

		if (0 == count)
			return;

		QString title = tr("Warning");
		QString msg = tr("Deleted all parameter?");
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);

		if (QMessageBox::No == result)
			return;

		QList<DataProperty::ParameterBase*> paraList = dataBase->getParaList();
		
		for (int i = 0; i < paraList.count(); i++)
		{
			DataProperty::ParameterBase* item = paraList.at(i);
			if (nullptr == item)
				continue;
			dataBase->removeParameter(item);
		}

		FillParameterList();
		ui->btnDel_s->setEnabled(false);
		ui->btnEdit_s->setEnabled(false);
		ui->btnDelAll_s->setEnabled(false);
	}
	
	//���������ؼ���˫��
	void EditorDescripttionSetup::TableWidgetParaDoubleClicked(QModelIndex modelIndex)
	{
		_currentOpObject = ParaList;
		OnBtnEditParaClicked();
	}

	//���������ؼ�������
	void  EditorDescripttionSetup::TableWidgetParaClicked(QModelIndex modelIndex)
	{
		/*
		int selectIndex = ui->tableWidget_PList->currentIndex().row();
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		QList<DataProperty::ParameterBase*> paraList = dataBase->getParaList();

		if (selectIndex < 0 || selectIndex >= paraList.count())
			return;

		DataProperty::ParameterBase* model = paraList.at(selectIndex);

		if (nullptr == model)
			return;

		_currentParameterList_Model = model;
		_currentOpObject = ParaList;
		return;
		*/
		_currentOpObject = ParaList;
		//ui->btnAdd_s->setEnabled(true);
		ui->btnDel_s->setEnabled(true);
		ui->btnEdit_s->setEnabled(true);
		ui->btnDelAll_s->setEnabled(true);

	}

	//����������ؼ�---�кŸı�ۺ���
	void EditorDescripttionSetup::TableWidgetParaCurrentCellChanged()
	{
	}

	DataProperty::ParameterBase* EditorDescripttionSetup::GetCurrentSelecctedParameter()
	{
		int selectIndex = ui->tableWidget_PList->currentIndex().row();
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return nullptr;

		QList<DataProperty::ParameterBase*> paraList = dataBase->getParaList();

		if (selectIndex < 0 || selectIndex >= paraList.count())
			return nullptr;

		DataProperty::ParameterBase* model = paraList.at(selectIndex);
		return model;
	}

	void EditorDescripttionSetup::DeleteParameter(DataProperty::ParameterBase* model)
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		dataBase->removeParameter(model);
	}

	//������--�����µĲ�����
	void EditorDescripttionSetup::OnBtnAddGroupClicked()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		QList<QString> usedNameList = GetParameterGroupNameList();
		EditorNameValue dlg(this);
		dlg.SetUsedNameList(usedNameList);
		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			DataProperty::ParameterGroup* group = new DataProperty::ParameterGroup();
			group->setDescribe(dlg.GetNameString());
			dataBase->appendParameterGroup(group);
			this->FillGroupList();
			ui->btnAdd_S_G->setEnabled(true);
			ui->btnEdit_S_G->setEnabled(false);
			ui->btnDel_S_G->setEnabled(false);
			ui->btnClearAll_S_G->setEnabled(true);
		}
	}

	//�༭������
	void EditorDescripttionSetup::OnBtnEditGroupClicked()
	{
		/*
		if (nullptr == _currentParameterGroup)
			return;

		int row = ui->tableWidget_GList->currentRow();

		QString name = _currentParameterGroup->getDescribe();
		QList<QString> usedNameList = GetParameterGroupNameList();
		usedNameList.removeOne(name);

		EditorNameValue dlg(this);
		dlg.SetNameString(name);
		dlg.SetUsedNameList(usedNameList);
		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			_currentParameterGroup->setDescribe(dlg.GetNameString());
			this->FillGroupList();
			ui->tableWidget_GList->selectRow(row);
		}
		*/
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
		if (nullptr == group)
			return;

		int row = ui->tableWidget_GList->currentRow();

		QString name = group->getDescribe();
		QList<QString> usedNameList = GetParameterGroupNameList();
		usedNameList.removeOne(name);

		EditorNameValue dlg(this);
		dlg.SetNameString(name);
		dlg.SetUsedNameList(usedNameList);
		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			group->setDescribe(dlg.GetNameString());
			this->FillGroupList();
			ui->tableWidget_GList->selectRow(row);
		}
	}

	//ɾ��������
	void EditorDescripttionSetup::OnBtnDelGroupClicked()
	{
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();

		if (nullptr == group)
			return;

		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		int rowNo = ui->tableWidget_GList->currentRow();

		QString title = tr("Warning");
		QString msg = tr("Deleted parameter group %1?").arg(group->getDescribe());
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);

		if (QMessageBox::Yes == result)
		{
			dataBase->removeParameterGroup(group);
			FillGroupList();
		}

		int count = dataBase->getParameterGroupCount();

		if (0 == count)
		{
			FillParameterList(ui->tableWidget_GPList);
			return;
		}
		
		rowNo = (rowNo == count) ? count - 1 : rowNo;
		ui->tableWidget_GList->selectRow(rowNo);
	}

	//������в�����
	void EditorDescripttionSetup::OnBtnClearAllGroupClicked()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();
		
		if (nullptr == dataBase)
			return;

		int count = dataBase->getParameterGroupCount();
		
		if (count <= 0)
			return;

		QString title = tr("Warning");
		QString msg = tr("Deleted all parameter group?");
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);

		if (QMessageBox::No == result)
			return;
		
		QList<DataProperty::ParameterGroup* > groupList;

		for (int i = 0; i < count; i++)
		{
			dataBase->removeParameterGroupAt(0);
		}

		FillGroupList();
		FillParameterList(ui->tableWidget_GPList);
	}

	//���������Ʊ���--˫���ۺ���
	void EditorDescripttionSetup::TableWidgetGoupDoubleClicked(QModelIndex modelIndex)
	{
		OnBtnEditGroupClicked();
	}

	//���������Ʊ���--�����ۺ���
	void EditorDescripttionSetup::TableWidgetGroupClicked(QModelIndex modelIndex)
	{
		TableWidgetGroupCurrentCellChanged();
	}

	//����������ؼ�---�кŸı�ۺ���
	void EditorDescripttionSetup::TableWidgetGroupCurrentCellChanged()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		int count = dataBase->getParameterGroupCount();
		int selectIndex = ui->tableWidget_GList->currentIndex().row();

		if (selectIndex < 0 || selectIndex >= count)
			return;

		DataProperty::ParameterGroup* group = dataBase->getParameterGroupAt(selectIndex);
		//_currentParameterGroup = group;
		FillParameterList(group, ui->tableWidget_GPList);
		ui->btnAdd_S_G->setEnabled(true);
		ui->btnEdit_S_G->setEnabled(false);
		ui->btnDel_S_G->setEnabled(false);
		ui->btnClearAll_S_G->setEnabled(true);
	}

	DataProperty::ParameterGroup* EditorDescripttionSetup::GetCurrentSelecctedGroup()
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return nullptr;

		int count = dataBase->getParameterGroupCount();
		int selectIndex = ui->tableWidget_GList->currentIndex().row();

		if (selectIndex < 0 || selectIndex >= count)
			return nullptr;

		DataProperty::ParameterGroup* group = dataBase->getParameterGroupAt(selectIndex);
		return group;
	}

	//������--���������ۺ���
	void EditorDescripttionSetup::OnBtnAddGroupParaClicked()
	{
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();

		if (nullptr == group)
			return;

		_currentOpObject = ParaGroupList;
		topMenu->exec(QCursor::pos());
	}
	
	//������--��������--�༭����
	void EditorDescripttionSetup::OnBtnEditGroupParaClicked()
	{
		int rowNo = ui->tableWidget_GPList->currentRow();
		
		if (rowNo < 0)
			return;

		DataProperty::ParameterBase* model = GetSelectedGroupParameter();
		
		if (nullptr == model)
			return;

		EditModel(model);
		ui->tableWidget_GPList->selectRow(rowNo);
	}

	//������--��������--ɾ��һ������
	void EditorDescripttionSetup::OnBtnDelGroupParaClicked()
	{
		int rowNo = ui->tableWidget_GPList->currentRow();
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
		
		if (nullptr == group)
			return;

		DataProperty::ParameterBase* model = GetSelectedGroupParameter();

		if (nullptr == model)
			return;

		QString title = tr("Warning");
		QString msg = tr("Deleted Parameter %1?").arg(model->getDescribe());
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);

		if (QMessageBox::Yes == result)
		{
			group->removeParameter(model);
			FillParameterList(group, ui->tableWidget_GPList);
		}

		int count = group->getParameterCount();

		if (0 == count)
			return;

		rowNo = (rowNo == count) ? count - 1 : rowNo;
		ui->tableWidget_GPList->selectRow(rowNo);
	}

	//������--��������-�����������
	void EditorDescripttionSetup::OnBtnClearAllGroupParaClicked()
	{
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
		
		if (nullptr == group)
			return;

		int count = group->getParameterCount();

		if (0 == count)
			return;

		QString title = tr("Warning");
		QString msg = tr("Deleted all parameter?");
		QMessageBox::StandardButton result = QMessageBox::warning(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);

		if (QMessageBox::No == result)
			return;

		QList<DataProperty::ParameterBase*> paraList = group->getParaList();

		for (int i = 0; i < paraList.count(); i++)
		{
			DataProperty::ParameterBase* item = paraList.at(i);
			if (nullptr == item)
				continue;
			group->removeParameter(item);
		}

		FillParameterList(group, ui->tableWidget_GPList); 
		ui->btnAdd_S_G->setEnabled(true);
		ui->btnEdit_S_G->setEnabled(false);
		ui->btnDel_S_G->setEnabled(false);
		ui->btnClearAll_S_G->setEnabled(false);
	}

	//������--����������˫���ۺ���
	void EditorDescripttionSetup::TableWidgetGroupParaDoubleClicked(QModelIndex modelIndex)
	{
		_currentOpObject = ParaGroupList;
		OnBtnEditGroupParaClicked();
		ui->btnAdd_S_G->setEnabled(true);
		ui->btnEdit_S_G->setEnabled(true);
		ui->btnDel_S_G->setEnabled(true);
		ui->btnClearAll_S_G->setEnabled(true);
	}

	//������--���������������ۺ���
	void EditorDescripttionSetup::TableWidgetGroupParaClicked(QModelIndex modelIndex)
	{
		_currentOpObject = ParaGroupList;
		ui->btnAdd_S_G->setEnabled(true);
		ui->btnEdit_S_G->setEnabled(true);
		ui->btnDel_S_G->setEnabled(true);
		ui->btnClearAll_S_G->setEnabled(true);
	}

	//������--���������ؼ�ѡ���кű仯�ۺ���
	void EditorDescripttionSetup::TableWidgetGroupParaCurrentCellChanged()
	{

	}

	DataProperty::ParameterBase* EditorDescripttionSetup::GetSelectedGroupParameter()
	{
		DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
		
		if (nullptr == group)
			return nullptr;

		QList<DataProperty::ParameterBase*> paraList = group->getParaList();
		int selectIndex = ui->tableWidget_GPList->currentIndex().row();

		if (selectIndex < 0 || selectIndex >= paraList.count())
			return nullptr;

		DataProperty::ParameterBase* model = paraList.at(selectIndex);
		return model;
	}

	//���������˵�
	void EditorDescripttionSetup::CreateAddParameterMenu()
	{
		topMenu = new QMenu(this);
		QAction*  action_int = new QAction(tr("Int"), topMenu);
		QAction*  action_double = new QAction(tr("Double"), topMenu);
		QAction*  action_bool = new QAction(tr("Bool"), topMenu);
		QAction*  action_string = new QAction(tr("String"), topMenu);
		QAction*  action_enum = new QAction(tr("Enum"), topMenu);
		QAction*  action_table = new QAction(tr("Table"), topMenu);
		QAction*  action_path = new QAction(tr("Path"), topMenu);

		topMenu->addAction(action_int);
		topMenu->addAction(action_double);
		topMenu->addAction(action_bool);
		topMenu->addAction(action_string);
		topMenu->addAction(action_enum);
		topMenu->addAction(action_table);
		topMenu->addAction(action_path);

		connect(action_int, SIGNAL(triggered()), this, SLOT(OnCreateIntClicked()));
		connect(action_bool, SIGNAL(triggered()), this, SLOT(OnCreateBoolClicked()));
		connect(action_double, SIGNAL(triggered()), this, SLOT(OnCreateDoubleClicked()));
		connect(action_string, SIGNAL(triggered()), this, SLOT(OnCreateStringClicked()));
		connect(action_enum, SIGNAL(triggered()), this, SLOT(OnCreateEnumClicked()));
		connect(action_table, SIGNAL(triggered()), this, SLOT(OnCreateTableClicked()));
		connect(action_path, SIGNAL(triggered()), this, SLOT(OnCreatePathClicked()));
	}

	//��ȡ�����������б�
	QList<QString> EditorDescripttionSetup::GetParameterGroupNameList()
	{
		QList<QString> usedNameList;
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return usedNameList;

		int count = dataBase->getParameterGroupCount();
		
		for (int i = 0; i < count; i++)
		{
			DataProperty::ParameterGroup* group = dataBase->getParameterGroupAt(i);
			usedNameList.append(group->getDescribe());
		}
		
		return usedNameList;
	}

	//��ȡ���������б��������б�
	QList<QString> EditorDescripttionSetup::GetParameterNameList(QList<DataProperty::ParameterBase*> parameterList)
	{
		QList<QString> usedNameList;
		
		foreach(DataProperty::ParameterBase* item, parameterList)
		{
			if (nullptr == item)
				continue;

			usedNameList.append(item->getDescribe());
		}
		return usedNameList;
	}

	//��ȡ�����б��������б�
	QList<QString> EditorDescripttionSetup::GetParameterNameList()
	{
		QList<QString> usedNameList;
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return usedNameList;
		
		if (_currentOpObject == ParaList)//��ȡ�����б��еĲ�������
		{
			usedNameList = GetParameterNameList(dataBase->getParaList());
		}
		else//��ȡ������--�����б��еĲ�������
		{
			DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
			
			if (nullptr == group)
				return usedNameList;
		
			usedNameList = GetParameterNameList(group->getParaList());
		}

		return usedNameList;
	}

	
	//ʵ�ֽ��������뵽�����б���������б���
	void EditorDescripttionSetup::InsertModelInParameterList(DataProperty::ParameterBase* model)
	{
		DataProperty::DataBase* dataBase = _model->GetDataBase();

		if (nullptr == dataBase)
			return;

		if (_currentOpObject == ParaList)
		{
			dataBase->appendParameter(model);
			FillParameterList();
			return;
		}
		
		if (_currentOpObject == ParaGroupList)
		{
			/*
			if (nullptr == _currentParameterGroup)
				return;

			_currentParameterGroup->appendParameter(model);
			FillParameterList(_currentParameterGroup, ui->tableWidget_GPList);
			return;
			*/
			DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
			
			if (nullptr == group)
				return;

			group->appendParameter(model);
			FillParameterList(group, ui->tableWidget_GPList);
			return;
		}
	}
	
	//���������Ͳ���
	void EditorDescripttionSetup::OnCreateBoolClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterBool *modelBool = new DataProperty::ParameterBool();
		EditorBoolValue dlg(modelBool, this);
		dlg.SetUsedNameList(usedNameList);

		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelBool);
		}
	}
	
	//�������β���
	void EditorDescripttionSetup::OnCreateIntClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterInt *modelInt = new DataProperty::ParameterInt();
		EditorIntValue dlg(modelInt, this);
		dlg.SetUsedNameList(usedNameList);

		int r = dlg.exec();
		
		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelInt);
		}
	}

	//���������Ͳ���
	void EditorDescripttionSetup::OnCreateDoubleClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterDouble *modelDouble = new DataProperty::ParameterDouble();
		EditorDoubleValue dlg(modelDouble, this);
		dlg.SetUsedNameList(usedNameList);

		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelDouble);
		}
	}

	//�����ַ��������
	void EditorDescripttionSetup::OnCreateStringClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterString *modelString = new DataProperty::ParameterString();

		EditorStringValue dlg(modelString, this);
		dlg.SetUsedNameList(usedNameList);

		int r = dlg.exec();

		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelString);
		}
	}
	
	//����ö���Ͳ���
	void EditorDescripttionSetup::OnCreateEnumClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterSelectable *modelEnum = new DataProperty::ParameterSelectable();
		EditorEnumValue dlg(modelEnum, this);
		dlg.SetUsedNameList(usedNameList);

		int r = dlg.exec();
		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelEnum);
		}

	}


	void EditorDescripttionSetup::OnCreateTableClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterTable *modelTable = new DataProperty::ParameterTable();

		EditorTableValue dlg(modelTable, this);
		dlg.setNameUsedList(usedNameList);

		int r = dlg.exec();
		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelTable);
		}
	}

	void EditorDescripttionSetup::OnCreatePathClicked()
	{
		QList<QString> usedNameList = GetParameterNameList();
		DataProperty::ParameterPath *modelPath = new DataProperty::ParameterPath();

		EditorPathValue dlg(modelPath, this);
		dlg.setNameUsedList(usedNameList);

		int r = dlg.exec();
		if (r == QDialog::Accepted)
		{
			InsertModelInParameterList(modelPath);
		}
	}


	//�༭����
	void EditorDescripttionSetup::EditModel(DataProperty::ParameterBase* model)
	{
		if (nullptr == model)
			return;

		QList<QString> usedNameList = GetParameterNameList();
		QString modelName = model->getDescribe();
		usedNameList.removeOne(modelName);
		int r = 0;

		if (model->getParaType() == DataProperty::ParaType::Para_Int)
		{
			DataProperty::ParameterInt* modelInt = (DataProperty::ParameterInt*) model;
			EditorIntValue dlg(modelInt, this);
			dlg.SetUsedNameList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::ParaType::Para_Bool)
		{
			DataProperty::ParameterBool* modelBool = (DataProperty::ParameterBool*) model;
			EditorBoolValue dlg(modelBool, this);
			dlg.SetUsedNameList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::ParaType::Para_Double)
		{
			DataProperty::ParameterDouble* modelDouble = (DataProperty::ParameterDouble*) model;
			EditorDoubleValue dlg(modelDouble, this);
			dlg.SetUsedNameList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::ParaType::Para_Selectable)
		{
			DataProperty::ParameterSelectable* modelEnum = (DataProperty::ParameterSelectable*) model;
			EditorEnumValue dlg(modelEnum, this);
			dlg.SetUsedNameList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::Para_String)
		{
			DataProperty::ParameterString *modelString = (DataProperty::ParameterString*) model;
			EditorStringValue dlg(modelString, this);
			dlg.SetUsedNameList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::Para_Table)
		{
			DataProperty::ParameterTable *modelTable = (DataProperty::ParameterTable *) model;
			EditorTableValue dlg(modelTable, this);
			dlg.setNameUsedList(usedNameList);
			r = dlg.exec();
		}
		else if (model->getParaType() == DataProperty::Para_Path)
		{
			DataProperty::ParameterPath *modelPath = (DataProperty::ParameterPath *) model;

			EditorPathValue dlg(modelPath, this);
			dlg.setNameUsedList(usedNameList);
			r = dlg.exec();
		}


		if (r != QDialog::Accepted)
			return;

		if (_currentOpObject == ParaList)
		{
			FillParameterList();
		}
		else
		{
			//FillParameterList(_currentParameterGroup, ui->tableWidget_GPList);
			DataProperty::ParameterGroup* group = GetCurrentSelecctedGroup();
			FillParameterList(group, ui->tableWidget_GPList);
		}

	}
	//ˢ�����ݵ�UI
	void EditorDescripttionSetup::UpdateDataToUi()
	{
		if (nullptr == _model)
			return;

		ui->txtChineseName->setText(_model->GetChnName());
		ui->txtEnglishName->setText(_model->GetEngName());
		ui->txtIcon->setText(_model->GetIconName());
		FillParameterList();
		FillGroupList();

		ui->btnDel_s->setEnabled(false);
		ui->btnEdit_s->setEnabled(false);
		ui->btnDelAll_s->setEnabled(true);
	}

	//ˢ��UI��Ϣ������
	void EditorDescripttionSetup::UpdateUiToData()
	{
		if (nullptr == _model)
			return;

		_model->SetChnName(ui->txtChineseName->text());
		_model->SetEngName(ui->txtEnglishName->text());
		_model->SetIconName(ui->txtIcon->text());
	}

	//�趨���������Ƿ���Ա༭״̬
	void EditorDescripttionSetup::SetIsEdit(bool b)
	{
		ui->txtChineseName->setEnabled(b);
		ui->txtEnglishName->setEnabled(b);
		ui->txtIcon->setEnabled(b);
		ui->btnLoadIcon->setEnabled(b);
	}
	//�ۺ���---Ok��ť
	void EditorDescripttionSetup::OnBtnOkClicked()
	{
		QString nameChn = ui->txtChineseName->text().trimmed();
		QString nameEng = ui->txtEnglishName->text().trimmed();
		int errorCode = InputValidator::getInstance()->ValidateModeEditName(_model, nameChn, nameEng, true);

		if (0 != errorCode)
		{
			QString errorMsg =  InputValidator::getInstance()->GetErrorMsg(errorCode);
			ui->lbl_info->setText(errorMsg);
			ui->lbl_info->show();
			QTimer::singleShot(3000, this, SLOT(OnTimeout()));
			return;
		}

		UpdateUiToData();

		if (nullptr != _treeItem)
		{
			_treeItem->setText(0, ui->txtChineseName->text());
			QString icon = ui->txtIcon->text().trimmed();
			_treeItem->setIcon(0, QIcon(icon));
		}

		this->accept();
		close();
	}
	
	//��ʱ���ۺ���
	void EditorDescripttionSetup::OnTimeout()
	{
		ui->lbl_info->setText("");
		ui->lbl_info->hide();
	}
	
	void EditorDescripttionSetup::OnBtnLoadIconClicked()
	{
		//QString suffixName = QString("*.*");
		QString suffixName = QString("JPG file(*.jpg);;PNG file(*.png);;ICO(*.ico);;All file(*.*)");
		QString file_name = QFileDialog::getOpenFileName(NULL, tr("Open image file"), ".", suffixName);
		
		if (!file_name.isEmpty())
			ui->txtIcon->setText(file_name);
	}

	void EditorDescripttionSetup::OnBtnShowGroupClicked()
	{
		if (ui->groupBox_3->isVisible())
		{
			ui->groupBox_3->hide();
			ui->btnShowParameterGroup->setText(tr("Show parameter group"));
		}
		else
		{
			ui->groupBox_3->show();
			ui->btnShowParameterGroup->setText(tr("Hide parameter group"));
		}

		resizeEvent(nullptr);
	}
}