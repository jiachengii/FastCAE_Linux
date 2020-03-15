#ifndef MESHSET_H_
#define MESHSET_H_

#include "meshDataAPI.h"
#include "DataProperty/DataBase.h"
#include <QString>
#include <QMultiHash>

class vtkDataSet;
class vtkUnstructuredGrid;
class vtkIdTypeArray;

namespace MeshData
{
	enum  SetType
	{
		None = 0,
		Node,
		Element,
		Family,
		BCZone,
	};
	
	class SetMember;

	class MESHDATAAPI MeshSet : public DataProperty::DataBase
	{
	public :
		//���캯��
		MeshSet(QString name, SetType type);
		MeshSet();
		~MeshSet();
		//��ȡ���ID
		int static getMaxID();
		//�������ID
		void static resetMaxID();
		///����ID����������
		void setID(int id) override;
		///�������� 
		void setType(SetType t);
		///��ȡ����
		SetType getSetType();
		//���ӳ�Ա
		void appendMember(int ker, int id);
		//��ȡKernal ID�б�
		QList<int> getKernals();
		//����kernal ID��ȡ��Ա
		QList<int> getKernalMembers(int k);
		//��ȡ����
		int getAllCount();
		//��ʱ����MemberID����void setKeneralID(int id)ʱ��գ�ָ��ΪKeneralΪid�ĵ��Ӽ�
		void appendTempMem(int m);
		//����Kenenal�� �� void appendTempMem(int m)���ʹ��
		void setKeneralID(int id);
		//�Ƿ����kernal
		bool isContainsKernal(int id);
		//�ϲ����
		void merge(MeshSet* set);
		//��ȥ���
		void cut(MeshSet* set);
		//md5
		void dataToStream(QDataStream* s) override;	
		//д����XML�ļ�
		virtual QDomElement& writeToProjectFile(QDomDocument* doc, QDomElement* parent) override;
		//��XML�ļ���ȡ����
		virtual void readDataFromProjectFile(QDomElement* e) override;
		//д���������ļ�
		virtual void writeBinaryFile(QDataStream* dataStream);
		//����������ļ�
		virtual void readBinaryFile(QDataStream* dataStream);
		//���ɿ�����ʾ��ģ��, ÿ��ʵ��ֻ�ܵ���һ��
		virtual void generateDisplayDataSet();
		//��ȡ��ʾģ��
		vtkDataSet* getDisplayDataSet();
		//�ַ���ת��Ϊö��
		static SetType stringToSettype(QString s);



	protected:
		SetType _type{ None };
		SetMember* _member{};
		QMultiHash<int, int> _members{};  //keneralID - node/elementID

		QList<int> _tempMemberID{};

		vtkDataSet* _displayDataSet{};

	private:
		static int maxID;
	};



}


#endif