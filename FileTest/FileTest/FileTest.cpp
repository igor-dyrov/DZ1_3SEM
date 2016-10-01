// FileTest.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>   
#include <boost\filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <stdlib.h>
#include <clocale>                    
#include <vector>
using namespace std;
using namespace boost::filesystem;

path Find(path BegPath, string Name, string Type)
{
	directory_iterator itr(BegPath);
	path tmp,test,NullPath;
	while (itr != directory_iterator{})
	{
		try
		{
			tmp = *itr;
			if (is_directory(tmp))
			{
				test = Find(tmp, Name, Type);
				if ((is_regular_file(test)) && (test.extension().string() == Type) && (test.stem().string() == Name))
				{
					return test;
				}
			}
			if ((is_regular_file(tmp)) && (tmp.extension().string() == Type) && (tmp.stem().string() == Name))
			{
				return tmp;
			}
			itr++;
		}
		catch(filesystem_error &e)
		{
			cout << e.what() << endl;
			itr++;
			tmp = *itr;
		}
	}
	return NullPath;
}
int digs(double w)
{
	if (w<10) return 1;
	int yield = 0;
	while (w>10) 
	{
		yield++;
		w /= 10; 
	}
	return yield + 2;
}


class Archive
{
private:
	struct ToIns
	{
		string Name, SearchDir;
		path WhereToInsert;
	};
	struct CommonInfo
	{
		int size;
		bool isFile;
		string FullDir, Name;
	};
	struct ToWrite
	{
		int Ignore, Write;
	};
	path Main;
	int NumberOfFiles;
	vector<CommonInfo> AddedObjects;
	void CreateDir(path ToCreate)
	{
		bool profit = false;
		while (!profit)
		{
			try
			{
				create_directory(ToCreate);
				profit = true;
			}
			catch (filesystem_error &e)
			{
				CreateDir(ToCreate.parent_path());
			}
		}
	}
	void FixData(int NewCount, string NewInfo, path ToInsert)
	{
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\shit");
		trash_tmp.append(Main.extension().string());
		FILE *fw = fopen(trash.c_str(), "w");
		fclose(fw);
		fw = fopen(trash.c_str(), "wb");
		FILE *fr = fopen(Main.string().c_str(), "rb");
		int old;
		fscanf(fr, "%d", &old);
		fprintf(fw, "%d", NewCount);
		fprintf(fw, "%s", NewInfo.c_str());
		char buf[1];
		char tmp[30];
		fseek(fr, digs(old), SEEK_SET);
		fseek(fw, 0, SEEK_END);
		char one;
		int i = 0;
		while (!feof(fw))
		{
			one = fgetc(fr);
			if (one != 'я')
			{
				fputc(one, fw);
			}
			if (one == '>')
			{
				i++;
			}
			if (i == 5 * old)
			{
				break;
			}
		}
		if (is_regular_file(ToInsert))
		{
			FILE *_ToInsert = fopen(ToInsert.string().c_str(), "rb");
			while (!feof(_ToInsert))
			{
				if (fread(buf, 1, 1, _ToInsert) == 1) fwrite(buf, 1, 1, fw);
			}
			fclose(_ToInsert);
		}
		while (!feof(fr))
		{
			if (fread(buf, 1, 1, fr) == 1)
				fwrite(buf, 1, 1, fw);
		}
		fclose(fw);
		fclose(fr);
		try
		{
			rename(Main, trash_tmp);
			rename(trash, Main);
		}
		catch (filesystem_error &e)
		{
			cout << e.what();
		}
		boost::filesystem::remove(trash_tmp);
	}
	string MakePathCorrect(string BegPath, string Search)
	{
		if (strcmp(BegPath.c_str(), Search.c_str()) == 0) return BegPath;
		string::iterator itr;
		char * str1 = new char[BegPath.length() + 1];
		int i = 0;
		for (itr = BegPath.begin(); itr != BegPath.end(); itr++)
		{
			str1[i] = *itr;
			i++;
		}
		str1[BegPath.length()] = 0;
		char * str2 = new char[Search.length() + 1];
		i = 0;
		for (itr = Search.begin(); itr != Search.end(); itr++)
		{
			str2[i] = *itr;
			i++;
		}
		str2[Search.length()] = 0;
		char *ptr = strstr(str1, str2);
		if (!ptr) return "";
		string Result(ptr);
		delete[] str1;
		delete[] str2;
		return Result;
	}
	struct InfoBlock
	{
	private:
		char* tmp;
	public:
		void Input(char *New)
		{
			tmp = New;
		}
		string Search()
		{
			int slength = 0;
			char *search;
			for (int i = 5; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				slength++;
			}
			if (slength == 0) return "";
			int j = 0;
			search = new char[slength + 1];
			for (int i = 5; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				search[j] = tmp[i];
				j++;
			}
			search[slength] = 0;
			string Result(search);
			delete[] search;
			return Result;
		}
		string Size()
		{
			int SizeBeg = 0, count = 0;
			for (int i = 0; i < 64; i++)
			{
				if (tmp[i] == '>') count++;
				if (count == 3)
				{
					SizeBeg = i + 2;
					break;
				}
			}
			int szlength = 0;
			for (int i = SizeBeg; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				szlength++;
			}
			if (szlength == 0) return "";
			int j = 0;
			char * _NecSize = new char[szlength + 1];
			for (int i = SizeBeg; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				_NecSize[j] = tmp[i];
				j++;
			}
			_NecSize[szlength] = 0;
			string Result(_NecSize);
			delete[] _NecSize;
			return Result;
		}
		string SearchDir(string __SearchDir)
		{
			int DirBeg = 0, count = 0;
			for (int i = 0; i < 64; i++)
			{
				if (tmp[i] == '>') count++;
				if (count == 2)
				{
					DirBeg = i + 2;
					break;
				}
			}
			if (__SearchDir.length() == 0) return "";
			char*_SearchDir = new char[__SearchDir.length() + 1];
			int j = 0;
			for (int i = DirBeg; i < DirBeg + __SearchDir.length(); i++)
			{
				if (tmp[i] == '>') break;
				_SearchDir[j] = tmp[i];
				j++;
			}
			_SearchDir[__SearchDir.length()] = 0;
			string Result(_SearchDir);
			delete[] _SearchDir;
			return Result;
		}
		string FullDir()
		{
			int DirBeg = 0, count = 0, DirLength = 0;
			for (int i = 0; i < 64; i++)
			{
				if (tmp[i] == '>') count++;
				if (count == 2)
				{
					DirBeg = i + 2;
					break;
				}
			}
			for (int i = DirBeg; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				DirLength++;
			}
			if (DirLength == 0) return"";
			char *FullDir = new char[DirLength + 1];
			int j = 0;
			for (int i = DirBeg; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				FullDir[j] = tmp[i];
				j++;
			}
			FullDir[DirLength] = 0;
			string Result(FullDir);
			delete[] FullDir;
			return Result;
		}
		int Type()
		{
			return (int)tmp[2] - (int)'0';
		}
		int Length()
		{
			int Result = 0;
			for (int i = 0; i < 64; i++)
			{
				if (tmp[i] != 0) Result++;
			}
			return Result;
		}
	};
public:
	Archive(string CreateNew, string Name)
	{
		NumberOfFiles = 0;
		CreateNew.append("\\");
		CreateNew.append(Name);
		CreateNew.append(".");
		CreateNew.append("arc");
		Main = CreateNew;
		FILE *_Main = fopen(Main.string().c_str(), "w");
		fprintf(_Main, "%d", NumberOfFiles);
		fclose(_Main);
	}
	Archive(path Real) 
	{
		if(exists(Real))Main = Real;
		FILE *GetCount = fopen(Main.string().c_str(), "r");
		fscanf(GetCount, "%d", &NumberOfFiles);
		fclose(GetCount);
	}
	void print()
	{
		cout << NumberOfFiles << endl;
		vector<CommonInfo>::iterator itr;
		for (itr = AddedObjects.begin(); itr != AddedObjects.end(); itr++)
		{
			cout << itr->Name << endl;
			cout << itr->size << endl;
			cout << itr->FullDir << endl;
			if (itr->isFile) cout << "File" << endl;
			else cout << "Directory" << endl;
			cout << "___________________________________" << endl;
		}
	}
	void GetInfo()
	{
		int type=0;
		FILE *fr = fopen(Main.string().c_str(), "rb");
		fseek(fr, digs((double)NumberOfFiles), SEEK_SET);
		char tmp[64];
		char t;
		InfoBlock str;
		CommonInfo obj;
		int s = 0;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fscanf(fr, "%c", &t);
				if (t == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t;
			}
			s = 0;
			str.Input(tmp);
			obj.FullDir = str.FullDir();
			obj.size = atoi(str.Size().c_str());
			obj.Name = str.Search();
			type = str.Type();
			if(type==1)obj.isFile = true;
			else obj.isFile = false;
			AddedObjects.push_back(obj);
		}
		fclose(fr);
	}
	void DeleteObject(string Name, string SearchDir, int Type)
	{
		vector<ToWrite> FilesToIgnore;
		vector<ToWrite> InfoToIgnore;
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\shit");
		trash_tmp.append(Main.extension().string());
		FILE *fw = fopen(trash.c_str(), "w");
		fclose(fw);
		fw = fopen(trash.c_str(), "wb");
		FILE *fr = fopen(Main.string().c_str(), "rb");
		int WrFl =0, WrInf = 0,old = NumberOfFiles;
		fseek(fr, digs(NumberOfFiles), SEEK_SET);
		char tmp[64];
		char t;
		int s = 0, type = 0;
		InfoBlock str;
		ToWrite obj;
		string search, NecSize, _SearchDir;
		int NewCount = NumberOfFiles;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fscanf(fr, "%c", &t);
				if (t == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t;
			}
			s = 0;
			str.Input(tmp);
			search = str.Search();
			NecSize = str.Size();
			_SearchDir = str.SearchDir(SearchDir);
			type = str.Type();
			bool needed = false;
			if ((Type == 1) && (type == Type) && (strcmp(search.c_str(), Name.c_str()) == 0) && (strcmp(_SearchDir.c_str(), SearchDir.c_str()) == 0) || ((Type == 0)&& (strcmp(_SearchDir.c_str(), SearchDir.c_str()) == 0))) needed = true;
			for (int i = 0; i < 1; i++)
			{
				if (needed)
				{
					NewCount--;
					obj.Ignore = atoi(NecSize.c_str());
					obj.Write = WrFl;
					FilesToIgnore.push_back(obj);
					obj.Write = WrInf;
					obj.Ignore = str.Length() + 1;
					InfoToIgnore.push_back(obj);
					WrFl = 0;
					WrInf = 0;
				}
				if (!needed)
				{
					WrFl += atoi(NecSize.c_str());
					WrInf += (str.Length() + 1);
				}
			}
		}
		obj.Ignore = 0;
		obj.Write = WrFl;
		FilesToIgnore.push_back(obj);
		obj.Write = WrInf;
		obj.Ignore = 0;
		InfoToIgnore.push_back(obj);
		fseek(fr, digs(old), SEEK_SET);
		NumberOfFiles = NewCount;
		fprintf(fw, "%d", NumberOfFiles);
		vector<ToWrite>::iterator itr;
		char buf[1];
		int AddedBytes;
		for (itr = InfoToIgnore.begin(); itr != InfoToIgnore.end(); itr++)
		{
			AddedBytes = 0;
			while (!feof(fr))
			{
				if (AddedBytes == itr->Write) break;
				fread(buf, 1, 1, fr);
				fwrite(buf, 1, 1, fw);
				AddedBytes++;
			}
			fseek(fr, itr->Ignore, SEEK_CUR);
		}
		for (itr = FilesToIgnore.begin(); itr != FilesToIgnore.end(); itr++)
		{
			AddedBytes = 0;
			while (!feof(fr))
			{
				if (AddedBytes == itr->Write) break;
				fread(buf, 1, 1, fr);
				fwrite(buf, 1, 1, fw);
				AddedBytes++;
			}
			fseek(fr, itr->Ignore, SEEK_CUR);
		}
		fclose(fw);
		fclose(fr);
		try
		{
			rename(Main, trash_tmp);
			rename(trash, Main);
		}
		catch (filesystem_error &e)
		{
			cout << e.what();
		}
		boost::filesystem::remove(trash_tmp);
	}
	void Insert(path File,string Directory)
	{
		char *tmp;
		if (exists(File))
		{
			boost::filesystem::ifstream RE(File);
			RE.seekg(0, ios::end);
			int f_size = RE.tellg();
			RE.seekg(0, ios::beg);
			string Info = "<<";
			if (is_directory(File)) Info.append("0><");
			else Info.append("1><");
			Info.append(File.filename().string());
			Info.append("><");
			if (Directory == "") Info.append("0");
			else Info.append(Directory.c_str());
			Info.append("><");
			if (is_regular_file(File))
			{
				tmp = new char[digs(f_size) + 1];
				tmp[digs(f_size)] = 0;
				itoa(f_size, tmp, 10);
				Info.append(tmp);
				delete[] tmp;
			}
			else Info.append("0");
			Info.append(">>");
			NumberOfFiles++;
			FixData(NumberOfFiles, Info,File);
		}
		else return;
	}
	void ExtractDirectory(string SearchDir,path Directory)
	{
		FILE *fr = fopen(Main.string().c_str(), "rb");
		fseek(fr, digs(NumberOfFiles), SEEK_SET);
		char tmp[64];
		char t;
		vector<ToIns> Files;
		int s = 0,type=0;
		path BegPath = SearchDir;
		InfoBlock str;
		string find = BegPath.filename().string();
		string search, NecSize, _SearchDir, FullDir;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fscanf(fr, "%c", &t);
				if (t == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t;
			}
			s = 0;
			str.Input(tmp);
			search = str.Search();
			NecSize = str.Size();
			_SearchDir = str.SearchDir(SearchDir);
			FullDir = str.FullDir();
			path WhereToIns;
			WhereToIns = Directory;
			WhereToIns.append(MakePathCorrect(FullDir, find));
			type= (int)tmp[2]-(int)'0';
			if (strcmp(SearchDir.c_str(), _SearchDir.c_str()) == 0)
			{
				if (type == 1)
				{
			      	ToIns a;
					a.Name = search;
					a.SearchDir = FullDir;
					a.WhereToInsert = WhereToIns;
					Files.push_back(a);
				}
				else
				{
					CreateDir(WhereToIns);
				}
			}
			type = 0;
		}
		vector<ToIns>::iterator a;
		for (a = Files.begin(); a < Files.end(); a++)
		{
			CreateDir(a->WhereToInsert.parent_path());
			ExtractFile(a->Name, a->WhereToInsert, a->SearchDir);
		}
	}
	void ExtractFile(string FileName, path Directory, string SearchDir)
	{
		FILE *fr  = fopen(Main.string().c_str(), "rb");
		string work;
		if (is_directory(Directory))
		{
			work = Directory.string();
			work.append("\\");
			work.append(FileName);
		}
		else return;
		fseek(fr, digs(NumberOfFiles), SEEK_SET);
		char tmp[64];
		InfoBlock str;
		string search, NecSize, _SearchDir;
		int num = 0,trash=0, _NecSize=0;
		char t;
		int s = 0;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fscanf(fr, "%c", &t);
				if (t == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t; 
			}
			s = 0;
			str.Input(tmp);
			search = str.Search();
			NecSize = str.Size();
			_SearchDir = str.FullDir();
			if ((strcmp(FileName.c_str(), search.c_str()) == 0) && (strcmp(_SearchDir.c_str(), SearchDir.c_str()) == 0))
			{
				_NecSize = atoi(NecSize.c_str());
				break;
			}
			int tr = atoi(NecSize.c_str());
			trash += tr;
		}
		if (_NecSize != 0)
		{
			FILE *fw = fopen(work.c_str(), "w");
			freopen(work.c_str(), "wb", fw);
			char find;
			int _find = 0;
			fseek(fr, digs(NumberOfFiles), SEEK_SET);
			while (!feof(fr))
			{
				find = fgetc(fr);
				if (find == '>')
				{
					_find++;
				}
				if (_find == 5 * NumberOfFiles) break;
			}
			fseek(fr, trash, SEEK_CUR);
			char buf[1];
			int i = 0;
			while (!feof(fr))
			{
				if (fread(buf, 1, 1, fr) == 1) fwrite(buf, 1, 1, fw);
				i++;
				if (i == _NecSize) break;
			}
			fclose(fw);
		}
		fclose(fr);
	}
	void InsertDirectory(path Directory, string Path, int iter)
	{
		if (!is_directory(Directory)) return;
		directory_iterator itr(Directory);
		path tmp;
		string New;
		if (iter == 0)
		{
			Insert(Directory, Directory.filename().string());
			Path = Directory.filename().string();
			iter++;
		}
		while (itr != directory_iterator{})
		{
			try
			{
				tmp = *itr;
				if (is_directory(tmp))
				{
					New = Path;
					New.append("//");
					New.append(tmp.filename().string());
					Insert(tmp,New);
					InsertDirectory(tmp, New, iter);
					New = Path;
				}
				if (is_regular_file(tmp))
				{
					Insert(tmp, Path);
				}
				itr++;
			}
			catch (filesystem_error &e)
			{
				cout << e.what() << endl;
				itr++;
				tmp = *itr;
			}
		}
	}
	~Archive(){}
};


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	Archive b("F:\\1.arc"); //обязательно файл с раширением arc
	//Примеры:
	//b.ExtractDirectory("Test", "F:\\45"); //1 - название папки в архиве, 2 - куда извлекать
	//b.InsertDirectory("F:\\Test", "Test//Directory", ""); //1 - путь к папке, которую надо записать в архив, 2 - в какую папку(в архиве) записать("", если нужно записать в корень архива), 3 - переменная для рекурсии(нужно указать 0)
	//b.Insert("F:\\2.jpg", "Test//55"); //записать ФАЙЛ, 1 - путь к файлу, 2 - в какую папку(в архиве) записать("", если нужно записать в корень архива)
	//b.DeleteObject("33", "Test//33", 0); // удалить файл/папку, 1 - имя ФАЙЛА, 2 - папка в архиве(в которой лежит удаляемый файл или которую надо полностью стереть из архива, "", если нужна корневая папка архива), 3 - тип объекта(1-файл, 0 - папка)
	//b.ExtractFile("1.mp3", "F:\\Test", "0"); //1 - имя файла, 2 - куда извлечь, 3 - папка архива, содержащая файл (0, если файл находится в корне архива)
	//папки в архиве разделаются с помощью "//", поэтому для Insert, DeleteObject и ExtractFile надо указывать папки в формать Dir//Dir//Dir
	system("pause");
	return 0;
}

