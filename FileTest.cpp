// FileTest.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>   
#include <boost\filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <stdlib.h>
#include <fstream>
#include <clocale>                    
#include <vector>
#include <array>
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
int LengthOfInteger(double w) //Returns the lenth of the number(to know how many bytes myst be reserved)
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
string MakePathCorrect(const string &BegPath, const string &Search) //Removes useless part of the path (for "Test//Dir1//Dir2//blabla" and "Test//Dir1" we'll get "//Dir1//Dir2//blabla", because we need just the "Dir1" folder)
{
	if (strcmp(BegPath.c_str(), Search.c_str()) == 0) return BegPath;
	int find = BegPath.find(Search);
	if (find == BegPath.npos) return BegPath;
	string Result = BegPath.substr(find+Search.length());
	return Result;

}

class Archive
{
private:
	struct ToExtract//in the code below I'm creating the vector of the files to Extract using this struct
	{
		string Name, SearchDir;
		path WhereToInsert;
	};
	struct CommonInfo//it's necessary to get Information about Objects in my archive
	{
		int size;
		bool isFile;
		string FullDir, Name;
	};
	struct ToWrite//struct to remove files from the archive
	{
		int Ignore, Write;
	};
	path Main;//path to the archive
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
	void FixData(int NewCount,const string &NewInfo, const path &ToInsert)//function to add new file or directory
	{
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\temp");
		trash_tmp.append(Main.extension().string());
		std::ofstream fw(trash, std::ofstream::binary);
		std::ifstream fr(Main.string(), std::ifstream::binary);
		char one[1],c;
		int old;
		fr >> old;
		cout << old;
		fw << NewCount<<NewInfo;
		char buf[1];
		int i = 0;
		if (old != 0)
		{
			fr.seekg(LengthOfInteger(old), std::ios::beg);
			while (!fr.eof())
			{
				fr >> c;
				if (c != 'я')
				{
					fw << c;
				}
				if (c == '>')
				{
					i++;
				}
				if (i == 5 * old)
				{
					break;
				}
			}
		}
		if (is_regular_file(ToInsert))
		{
			std::ifstream _ToInsert(ToInsert.string(), std::ifstream::binary);
			_ToInsert.read(buf, sizeof(buf));
			while (!_ToInsert.eof())
			{
				fw.write(buf, sizeof(buf));
				_ToInsert.read(buf, sizeof(buf));
			}
			_ToInsert.close();
		}
		if (old != 0)
		{
			fr.read(buf, sizeof(buf));
			while (!fr.eof())
			{
				fw.write(buf, sizeof(buf));
				fr.read(buf, sizeof(buf));
			}
		}
		fw.close();
		fr.close();
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
	struct InfoBlock//Information string that looks like "<<1><File.mp3><Test//Dir1//Dir2><12345>>" 1/0 - type of object(1 - file, 0 - folder), then name of the object, path in the archive and finaly size of the FILE
	{
	private:
		string tmp;
	public:
		void Input(char *New)
		{
			tmp = New;
		}
		//the methods below returns the needed parts of the InfoBlock
		string Name()
		{
			int slength = 0;
			for (int i = 5; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				slength++;
			}
			if (slength == 0) return "";
			char* find = new char[slength + 1];
			int j = 0;
			for (int i = 5; i < 64; i++)
			{
				if (tmp[i] == '>') break;
				find[j] = tmp[i];
				j++;
			}
			find[slength] = 0;
			string Result(find);
			delete[] find;
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
		string SearchDir(string __SearchDir)//the method to check the attachment the object to __Searchdir folder in archive(func returns the part of the object path which lenghth is equal to the arg length)\
			example: file is in Dir1/Dir2/Dir3 folder, we are sending "Dir1/Dir2" as __SearchDir and getting "Dir1/Dir2", it means that it's necessary file
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
		string FullDir()//full path of the object in archive
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
		int Length()//the length of the InfoBlock
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
	Archive(const string &CreateNew,const string &Name)
	{
		NumberOfFiles = 0;
		Main = CreateNew;
		Main.append("\\");
		Main.append(Name);
		Main.append(".arc");
		std::ofstream _Main(Main.string());
		_Main << NumberOfFiles;
		_Main.close();
	}
	Archive(const path& Real) 
	{
		if(exists(Real))Main = Real;
		std::ifstream GetCount(Main.string());
		GetCount >> NumberOfFiles;
		GetCount.close();
	}
	void print()//to print the list of all objects in the archive
	{
		cout << NumberOfFiles << endl;
		AddedObjects.clear();
		GetInfo();
		for (const auto & kpv : AddedObjects)
		{
			cout << kpv.Name << endl;
			cout << kpv.size << endl;
			cout << kpv.FullDir << endl;
			if (kpv.isFile) cout << "File" << endl;
			else cout << "Directory" << endl;
			cout << "___________________________________" << endl;
		}
	}
	void GetInfo()//gets Inforamtion to print it
	{
		int type=0;
		std::ifstream fr(Main.string(), std::ifstream::binary);
		fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
		char tmp[64];
		char t[1];
		InfoBlock str;
		CommonInfo obj;
		AddedObjects.clear();
		int s = 0;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fr.read(t, sizeof(t));
				if (t[0] == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t[0];
			}
			s = 0;
			str.Input(tmp);
			obj.FullDir = str.FullDir();
			obj.size = atoi(str.Size().c_str());
			obj.Name = str.Name();
			type = str.Type();
			if(type==1)obj.isFile = true;
			else obj.isFile = false;
			AddedObjects.push_back(obj);
		}
		fr.close();
	}
	void RemoveObject(const string &Name,const string &SearchDir, int Type)//Deletes some object from the archive(1 - the name of the object, 2-the path to this object in my archive, 3- type of the object(1-file,0 -folder)
	{
		vector<ToWrite> FilesToIgnore;
		vector<ToWrite> InfoToIgnore;
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\temp");
		trash_tmp.append(Main.extension().string());
		std::ofstream fw(trash, std::ofstream::binary);
		std::ifstream fr(Main.string(), std::ifstream::binary);
		int WrFl =0, WrInf = 0,old = NumberOfFiles;
		fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
		char tmp[64];
		char t[1];
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
				fr.read(t, sizeof(t));
				if (t[0] == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t[0];
			}
			s = 0;
			str.Input(tmp);
			search = str.Name();
			NecSize = str.Size();
			_SearchDir = str.SearchDir(SearchDir);
			type = str.Type();
			if ((Type == 1) && (type == Type) && (strcmp(search.c_str(), Name.c_str()) == 0) && (strcmp(_SearchDir.c_str(), SearchDir.c_str()) == 0) || ((Type == 0) && (strcmp(_SearchDir.c_str(), SearchDir.c_str()) == 0)))
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
			else
			{
				WrFl += atoi(NecSize.c_str());
				WrInf += (str.Length() + 1);
			}
		}
		obj.Ignore = 0;
		obj.Write = WrFl;
		FilesToIgnore.push_back(obj);
		obj.Write = WrInf;
		obj.Ignore = 0;
		InfoToIgnore.push_back(obj);
		fr.seekg(LengthOfInteger(old), std::ios::beg);
		NumberOfFiles = NewCount;
		fw << NumberOfFiles;
		vector<ToWrite>::iterator itr;
		char buf[1];
		int AddedBytes;
		for (itr = InfoToIgnore.begin(); itr != InfoToIgnore.end(); itr++)
		{
			AddedBytes = 0;
			while (!fr.eof())
			{
				if (AddedBytes == itr->Write) break;
				fr.read(buf, sizeof(buf));
				fw.write(buf,sizeof(buf));
				AddedBytes++;
			}
			fr.seekg(itr->Ignore, std::ios::cur);
		}
		for (itr = FilesToIgnore.begin(); itr != FilesToIgnore.end(); itr++)
		{
			AddedBytes = 0;
			while (!fr.eof())
			{
				if (AddedBytes == itr->Write) break;
				fr.read(buf, sizeof(buf));
				fw.write(buf, sizeof(buf));
				AddedBytes++;
			}
			fr.seekg(itr->Ignore, std::ios::cur);
		}
		fw.close();
		fr.close();
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
	void Insert(const path &File,const string &Directory)//prepares the InfoBlock and sends it in FixData with the path to the object and new number of objects in archive)\
		Warning: this func can only insert file or the directory(without inserting files and folders in it)
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
				tmp = new char[LengthOfInteger(f_size) + 1];
				tmp[LengthOfInteger(f_size)] = 0;
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
	void ExtractDirectory(string SearchDir,path Directory)//extracts the directory with all objects in it(1 - the path to the directory in archive(FULL PATH), 2 - the path in the external storage(where to insert)
	{
		std::ifstream fr(Main.string(), std::ifstream::binary);
		fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
		char tmp[64];
		char t[1];
		vector<ToExtract> Files;
		int s = 0,type=0;
		path BegPath = SearchDir;
		InfoBlock str;
		string search, NecSize, _SearchDir, FullDir;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fr.read(t, sizeof(t));
				if (t[0] == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t[0];
			}
			s = 0;
			str.Input(tmp);
			search = str.Name();
			NecSize = str.Size();
			_SearchDir = str.SearchDir(SearchDir);
			FullDir = str.FullDir();
			path WhereToIns;
			WhereToIns = Directory;
			string find;
			if (BegPath.has_parent_path())
			{
				find = BegPath.parent_path().string();
				WhereToIns.append(MakePathCorrect(FullDir, find));
			}
			else
			{
				WhereToIns.append(FullDir);
			}
			type= (int)tmp[2]-(int)'0';
			if (strcmp(SearchDir.c_str(), _SearchDir.c_str()) == 0)
			{
				if (type == 1)
				{
			      	ToExtract a;
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
		fr.close();
		for (const auto &itr : Files)
		{
			CreateDir(itr.WhereToInsert.parent_path());
			ExtractFile(itr.Name, itr.WhereToInsert, itr.SearchDir);
		}
	}
	void ExtractFile(string FileName, path Directory, string SearchDir)//1 - the name of the file, 2 - where to insert, 2 - the path to this file in the archive
	{
		std::ifstream fr(Main.string(), std::ofstream::binary);
		string work;
		if (is_directory(Directory))
		{
			work = Directory.string();
			work.append("\\");
			work.append(FileName);
		}
		else return;
		fr.seekg(LengthOfInteger(NumberOfFiles), std::ios_base::beg);
		char tmp[64];
		InfoBlock str;
		string search, NecSize, _SearchDir;
		int num = 0,trash=0, _NecSize=0;
		char t[1];
		int s = 0;
		for (int i = 0; i < NumberOfFiles; i++)
		{
			for (int i = 0; i < 64; i++) tmp[i] = 0;
			for (int y = 0; y < 64; y++)
			{
				fr.read(t, sizeof(t));
				if (t[0] == '>')
				{
					s++;
					if (s == 5) break;
				}
				tmp[y] = t[0]; 
			}
			s = 0;
			str.Input(tmp);
			search = str.Name();
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
			std::ofstream fw(work.c_str(), std::ofstream::binary);
			char find[1];
			int _find = 0;
			fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
			while (!fr.eof())
			{
				fr.read(find, sizeof(find));
				if (find[0] == '>')
				{
					_find++;
				}
				if (_find == 5 * NumberOfFiles) break;
			}
			fr.seekg(trash, std::ios_base::cur);
			char buf[1];
			int i = 0;
			while (!fr.eof())
			{
				if (i == _NecSize) break;
				fr.read(buf, sizeof(buf));
				fw.write(buf, sizeof(buf));
				i++;
			}
			fw.close();
		}
		fr.close();
	}
	void InsertDirectory(path Directory, string Path, int iter=0)//1 - path in the external storage to the needed folder, 2- folder in the archive(where to insert the object), 3 - iteration variable\
		this func inserts ALL objects in the folder to the archive(even folder/folder/folder/1.jpg)
	{
		if (!is_directory(Directory)) return;
		directory_iterator itr(Directory);
		path tmp;
		string New;
		if (iter == 0)
		{
			New = Path;
			if (Path != "") New.append("//");
			New.append(Directory.filename().string());
			Path = New;
			Insert(Directory, New);
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
					if (Path != "") New.append("//");
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
	//Archive b("F:\\1.arc"); //обязательно файл с раширением arc
	system("pause");
	return 0;
}

