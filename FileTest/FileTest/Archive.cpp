#include "stdafx.h"
#include "Archive.h"

using namespace foo;

int foo::LengthOfInteger(double w) //Returns the lenth of the number(to know how many bytes myst be reserved)
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
void  foo::CreateDir(const path &ToCreate)
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
};
path foo::Find(path BegPath, string Name, string Type)
{
	directory_iterator itr(BegPath);
	path tmp, test, NullPath;
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
		catch (filesystem_error &e)
		{
			cout << e.what() << endl;
			itr++;
			tmp = *itr;
		}
	}
	return NullPath;
};
string foo::MakePathCorrect(const string &BegPath, const string &Search)
{
	if (strcmp(BegPath.c_str(), Search.c_str()) == 0) return BegPath;
	int find = BegPath.find(Search);
	if (find == BegPath.npos) return BegPath;
	string Result = BegPath.substr(find + Search.length());
	return Result;

}

void Archive::InfoBlock::Input(const vector<char> &New)
{
	int i = 0;
	tmp.clear();
	for_each(New.begin(), New.end(), [this, &i](char a){tmp.append(" "); tmp[i] = a; i++; });
};
string Archive::InfoBlock::Name()
{
	int slength = 0;
	for (int i = FirstNameChar; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') break;
		slength++;
	}
	if (slength == 0) return "";
	string Result = tmp.substr(FirstNameChar, slength);
	return Result;
}
string Archive::InfoBlock::Size()
{
	int SizeBeg = 0, count = 0;
	for (int i = 0; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') count++;
		if (count == 3)
		{
			SizeBeg = i + 2;
			break;
		}
	}
	int szlength = 0;
	for (int i = SizeBeg; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') break;
		szlength++;
	}
	string Result = tmp.substr(SizeBeg, szlength);
	return Result;
}
string Archive::InfoBlock::SearchDir(string __SearchDir)
{
	int DirBeg = 0, count = 0;
	for (int i = 0; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') count++;
		if (count == 2)
		{
			DirBeg = i + 2;
			break;
		}
	}
	if (__SearchDir.length() == 0) return "";
	string Result = tmp.substr(DirBeg, __SearchDir.length());
	return Result;
}
string Archive::InfoBlock::FullDir()
{
	int DirBeg = 0, count = 0, DirLength = 0;
	for (int i = 0; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') count++;
		if (count == 2)
		{
			DirBeg = i + 2;
			break;
		}
	}
	for (int i = DirBeg; i < tmp.length(); i++)
	{
		if (tmp[i] == '>') break;
		DirLength++;
	}
	if (DirLength == 0) return"";
	string Result = tmp.substr(DirBeg, DirLength);
	return Result;
}
int Archive::InfoBlock::Type()
{
	return (int)tmp[TypeChar] - (int)'0';
}
int Archive::InfoBlock::Length()
{
	return tmp.length();
}

Archive::Archive(const string &CreateNew, string &Name)
{
	NumberOfFiles = 0;
	Main = CreateNew;
	Main.append("\\");
	Name.append(".arc");
	Main.append(Name);
	std::ofstream _Main(Main.string());
	_Main << NumberOfFiles;
	_Main.close();
}
Archive::Archive(const path& Real)
{
	if (exists(Real))Main = Real;
	std::ifstream GetCount(Main.string());
	GetCount >> NumberOfFiles;
	GetCount.close();
}

void Archive::print()
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
void Archive::GetInfo()
{
	int type = 0;
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
	vector<char> tmp;
	char t[1];
	InfoBlock str;
	CommonInfo obj;
	AddedObjects.clear();
	int s = 0;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		while (!fr.eof())
		{
			fr.read(t, sizeof(t));
			if (t[0] == '>')
			{
				s++;
				if (s == 5) break;
			}
			tmp.push_back(t[0]);
		}
		s = 0;
		str.Input(tmp);
		obj.FullDir = str.FullDir();
		obj.size = atoi(str.Size().c_str());
		obj.Name = str.Name();
		type = str.Type();
		tmp.clear();
		if (type == 1)obj.isFile = true;
		else obj.isFile = false;
		AddedObjects.push_back(obj);
	}
	fr.close();
}
void Archive::RemoveObject(const string &Name, const string &SearchDir, int Type)
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
	int WrFl = 0, WrInf = 0, old = NumberOfFiles;
	fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
	vector<char> tmp;
	char t;
	int s = 0, type = 0;
	InfoBlock str;
	ToWrite obj;
	string search, NecSize, _SearchDir;
	int NewCount = NumberOfFiles;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		while (!fr.eof())
		{
			fr >> t;
			if (t == '>')
			{
				s++;
				if (s == 5) break;
			}
			tmp.push_back(t);
		}
		s = 0;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
		_SearchDir = str.SearchDir(SearchDir);
		type = str.Type();
		tmp.clear();
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
	char buf[1];
	int AddedBytes;
	for (const auto& itr : InfoToIgnore)
	{
		AddedBytes = 0;
		while (!fr.eof())
		{
			if (AddedBytes == itr.Write) break;
			fr.read(buf, sizeof(buf));
			fw.write(buf, sizeof(buf));
			AddedBytes++;
		}
		fr.seekg(itr.Ignore, std::ios::cur);
	}
	for (const auto& itr : FilesToIgnore)
	{
		AddedBytes = 0;
		while (!fr.eof())
		{
			if (AddedBytes == itr.Write) break;
			fr.read(buf, sizeof(buf));
			fw.write(buf, sizeof(buf));
			AddedBytes++;
		}
		fr.seekg(itr.Ignore, std::ios::cur);
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
void Archive::Insert(const path &File, const string &Directory)
{
	if (exists(File))
	{
		std::ifstream _ToInsert(File.string(), std::ifstream::binary);
		_ToInsert.seekg(0, ios::end);
		int f_size = _ToInsert.tellg();
		_ToInsert.seekg(0, ios::beg);
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
			std::stringstream tmp;
			tmp << f_size;
			Info.append(tmp.str());
		}
		else Info.append("0");
		Info.append(">>");
		NumberOfFiles++;
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\temp");
		trash_tmp.append(Main.extension().string());
		std::ofstream fw(trash, std::ofstream::binary);
		std::ifstream fr(Main.string(), std::ifstream::binary);
		char c;
		int old;
		fr >> old;
		fw << NumberOfFiles << Info;
		char buf[1];
		int i = 0;
		if (old != 0)
		{
			fr.seekg(LengthOfInteger(old), std::ios::beg);
			while (!fr.eof())
			{
				fr >> c;
				if (c != 'ÿ')
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
		if (is_regular_file(File))
		{
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
	else return;
}
void Archive::ExtractDirectory(const string &SearchDir, const path Directory)//extracts the directory with all objects in it(1 - the path to the directory in archive(FULL PATH), 2 - the path in the external storage(where to insert)
{
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
	vector<char> tmp;
	char t;
	vector<ToExtract> Files;
	int s = 0, type = 0;
	path BegPath = SearchDir;
	InfoBlock str;
	string search, NecSize, _SearchDir, FullDir;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		while (!fr.eof())
		{
			fr >> t;
			if (t == '>')
			{
				s++;
				if (s == 5) break;
			}
			tmp.push_back(t);
		}
		s = 0;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
		_SearchDir = str.SearchDir(SearchDir);
		FullDir = str.FullDir();
		type = str.Type();
		tmp.clear();
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
	}
	fr.close();
	for (const auto &itr : Files)
	{
		CreateDir(itr.WhereToInsert.parent_path());
		ExtractFile(itr.Name, itr.WhereToInsert, itr.SearchDir);
	}
}
void Archive::ExtractFile(const string &FileName, path Directory, const string &SearchDir)
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
	vector<char> tmp;
	InfoBlock str;
	string search, NecSize, _SearchDir;
	int num = 0, trash = 0, _NecSize = 0;
	char t;
	int s = 0;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		while (!fr.eof())
		{
			fr >> t;
			if (t == '>')
			{
				s++;
				if (s == 5) break;
			}
			tmp.push_back(t);
		}
		s = 0;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
		_SearchDir = str.FullDir();
		tmp.clear();
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
		std::ofstream fw(work, std::ofstream::binary);
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
void Archive::InsertDirectory(path Directory, string Path, int iter)
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
				Insert(tmp, New);
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

Archive::~Archive(){}
