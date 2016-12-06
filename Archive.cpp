#include "Archive.h"
#include <wx/wx.h>

using namespace foo;

int foo::LengthOfInteger(int w) //Returns the lenth of the number(to know how many bytes myst be reserved)
{
	std::stringstream tmp;
	tmp << w;
	return tmp.str().length();
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
bool foo::IsParent(path p1, path p2)
{
	if (p1 == p2) return true;
	while (p2.has_parent_path())
	{
		p2 = p2.parent_path();
		if (p1 == p2) return true;
	}
	return false;
}
void foo::NumberOfObjects(path& Folder, int& count)
{
	directory_iterator itr(Folder);
	path tmp;
	try
	{
		while (itr != directory_iterator())
		{
			tmp = *itr;
			if (is_directory(tmp))
			{
				NumberOfObjects(tmp, count);
				++count;
			}
			if (is_regular_file(tmp))++count;
			++itr;
		}
	}
	catch (filesystem_error& e)
	{
		++itr;
		tmp = *itr;
	}
	count++;
}
void foo::ReplaceSpaces(string& Target)
{
	std::replace(Target.begin(), Target.end(), ' ', '_');
}
bool foo::crypt(Kuznyechik cipher, TextBlock ToCrypt, int mode)
{
	if (mode == 1) cipher.encrypt(*ToCrypt, *ToCrypt);
	else cipher.decrypt(*ToCrypt, *ToCrypt);
	return true;
}
string foo::ComposeInfoBlock(const path &File, const string &Directory)
{
	std::ifstream _ToInsert(File.string(), std::ifstream::binary);
	_ToInsert.seekg(0, ios::end);
	int f_size = _ToInsert.tellg();
	_ToInsert.seekg(0, ios::beg);
	string Safe;
	string Info = "<<";
	if (is_directory(File)) Info.append("0><");
	else Info.append("1><");
	Safe = File.filename().string();
	foo::ReplaceSpaces(Safe);
	Info.append(Safe);
	Info.append("><");
	if (Directory == "") Info.append("0");
	else
	{
		Safe = Directory;
		foo::ReplaceSpaces(Safe);
		Info.append(Safe);
	}
	Info.append("><");
	if (is_regular_file(File))
	{
		std::stringstream _tmp;
		if ((f_size % 16) != 0)
		{
			f_size = BUFFERSIZE*(f_size / BUFFERSIZE) + BUFFERSIZE;
		}
		_tmp << f_size;
		Info.append(_tmp.str());
	}
	else Info.append("0");
	Info.append(">>");
	_ToInsert.close();
	return Info;
}
void foo::WriteInfoBlocks(std::ifstream& fr, std::ofstream& fw, int old)
{
	int i = 0;
	char c;
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
			if (i == NUMBEROFSEPARATORS * old)
			{
				break;
			}
		}
	}
}
void foo::CopyDir(path const & Source, path const & Destination, string CurrentPath, wxProgressDialog& dialog, int iter)
{
	path tmp;
	if (CurrentPath == "")
	{
		tmp = Destination;
		tmp.append(Source.filename().string());
		create_directory(tmp);
		CurrentPath = Source.filename().string();
	}
	directory_iterator itr(Source);
	while (itr != directory_iterator())
	{
		tmp = *itr;
		if (is_regular_file(tmp))
		{
			path ToPaste = Destination;
			ToPaste.append("\\");
			ToPaste.append(CurrentPath);
			ToPaste.append("\\");
			ToPaste.append(itr->path().filename().string());
			copy_file(tmp, ToPaste);
			++iter;
			dialog.Update(iter);
		}
		if (is_directory(tmp))
		{
			path ToPaste = Destination;
			ToPaste.append("\\");
			ToPaste.append(CurrentPath);
			ToPaste.append("\\");
			ToPaste.append(itr->path().filename().string());
			create_directory(ToPaste);
			string _tmp = CurrentPath;
			_tmp.append("\\");
			_tmp.append(itr->path().filename().string());
			foo::CopyDir(tmp, Destination, _tmp, dialog, iter);
			++iter;
			dialog.Update(iter);
		}
		++itr;
	}
}
void foo::WriteAndIgnore(const vector<ToWrite> &Source,std::ifstream& From,std::ofstream& To)
{
	int AddedBytes = 0;
	char buf[BUFFERSIZE];
	for (const auto& itr :Source)
	{
		AddedBytes = 0;
		if (itr.Write < BUFFERSIZE)
		{
			From.read(buf, itr.Write);
			To.write(buf, itr.Write);
		}
		else
		{
			while (!From.eof())
			{
				if ((itr.Write - AddedBytes) < BUFFERSIZE)
				{
					From.read(buf, itr.Write - AddedBytes);
					To.write(buf, itr.Write - AddedBytes);
					break;
				}
				From.read(buf, sizeof(buf));
				To.write(buf, sizeof(buf));
				AddedBytes += BUFFERSIZE;
			}
		}
		From.seekg(itr.Ignore, std::ios::cur);
	}
}
void foo::WriteToTheLast(std::ifstream& From, std::ofstream* To)
{

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

Archive::Archive()
{
	_IsLocked = true;
}
Archive::Archive(const path& File)
{
	if (exists(File))Main = File;
	std::ifstream GetStarted(Main.string());
	GetStarted.seekg(0, ios::end);
	int FileSize = GetStarted.tellg();
	GetStarted.seekg(0, ios::beg);
	if (FileSize == 0)
	{
		GetStarted.close();
		NumberOfFiles = 0;
		std::ofstream NewArchive(Main.string());
		NewArchive << "0";
		NewArchive.close();
	}
	else
	{
		GetStarted >> NumberOfFiles;
		GetStarted.close();
	}
	_IsLocked = true;
}
void Archive::SkipInfoBlock(std::ifstream& fr)
{
	char find;
	int _find = 0;
	while (!fr.eof())
	{
		fr >> find;
		if (find == '>')
		{
			_find++;
		}
		if (_find == NUMBEROFSEPARATORS * NumberOfFiles) break;
	}
}
void Archive::GetInfo()
{
	int type = 0;
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
	vector<char> tmp;
	AddedObjects.clear();
	for (int i = 0; i < NumberOfFiles; i++)
	{
		GetInfoBlock(tmp, fr);
		InfoBlock str;
		str.Input(tmp);
		CommonInfo obj;
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
void Archive::RemoveObject(const string &Name, const path &SearchDir, int Type)
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
	int type = 0;
	ToWrite obj;
	string search, NecSize;
	path _SearchDir;
	int NewCount = NumberOfFiles;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		GetInfoBlock(tmp, fr);
		InfoBlock str;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
		_SearchDir = str.SearchDir(SearchDir.string());
		type = str.Type();
		tmp.clear();
		if (((Type == File) && (type == Type) && (search == Name) && (IsParent(SearchDir, str.FullDir()))) || ((Type == Directory) && (IsParent(SearchDir, str.FullDir()))))
		{
			NewCount--;
			obj.Ignore = atoi(NecSize.c_str());
			obj.Write = WrFl;
			FilesToIgnore.push_back(obj);
			obj.Write = WrInf;
			obj.Ignore = str.Length()+1;
			InfoToIgnore.push_back(obj);
			WrFl = 0;
			WrInf = 0;
		}
		else
		{
			WrFl += atoi(NecSize.c_str());
			WrInf += (str.Length()+1);
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
	foo::WriteAndIgnore(InfoToIgnore, fr, fw);
	foo::WriteAndIgnore(FilesToIgnore, fr, fw);
	fw.close();
	fr.close();
	EndSession(trash, trash_tmp);
}
void Archive::OpenNew(path& File)
{
	Main = File;
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(ios::end);
	int check = fr.tellg();
	if (check == 0)
	{
		int tmp = 0;
		fr >>tmp;
		return;
	}
	fr.seekg(ios::beg);
	fr >> NumberOfFiles;
}
void Archive::Insert(const path &File, const string &Directory)
{
	if (_IsLocked) return;
	if ((exists(File)) && (is_regular_file(File)))
	{
		std::ifstream _ToInsert(File.string(), std::ifstream::binary);
		NumberOfFiles++;
		string trash = Main.parent_path().string();
		trash.append("\\trash");
		trash.append(Main.extension().string());
		string trash_tmp = Main.parent_path().string();
		trash_tmp.append("\\temp");
		trash_tmp.append(Main.extension().string());
		std::ofstream fw(trash, std::ofstream::binary);
		std::ifstream fr(Main.string(), std::ifstream::binary);
		fr.seekg(0, ios::end);
		int full = fr.tellg(),old;
		fr.seekg(ios::beg);
		fr >> old;
		string Info = foo::ComposeInfoBlock(File, Directory);
		fw << NumberOfFiles << Info;
		foo::WriteInfoBlocks(fr, fw, old);
		char *buf = new char[MT_BUFFER];
		const int FSize = full - fr.tellg();
		_ToInsert.seekg(0, ios::end);
		long long RealSize = _ToInsert.tellg();
		_ToInsert.seekg(ios::beg);
		vector<TextBlock> EncryptedBlocks;
		int Ready = 0;
		vector<future<bool>> Threads;
		char SmallBuffer[16];
		while (!_ToInsert.eof())
		{
			if (RealSize - Ready == 0) break;
			if ((RealSize - Ready) < MT_BUFFER)
			{
				while (!_ToInsert.eof())
				{
					if (RealSize - Ready < 16)
					{
						_ToInsert.read(SmallBuffer, RealSize - Ready);
						for (int i = RealSize - Ready; i < 16; i++)
						{
							SmallBuffer[i] = 0;
						}
						ByteBlock _ToCrypt((BYTE*)SmallBuffer, 16);
						Cipher->encrypt(_ToCrypt, _ToCrypt);
						fw.write((char*)_ToCrypt.byte_ptr(), 16);
						break;
					}
					_ToInsert.read(SmallBuffer, 16);
					ByteBlock __ToCrypt((BYTE*)SmallBuffer, 16);
					Cipher->encrypt(__ToCrypt, __ToCrypt);
					char* EncryptedPtr = (char*)__ToCrypt.byte_ptr();
					fw.write(EncryptedPtr, 16);
					Ready += 16;
				}
				break;
			}
			_ToInsert.read(buf, MT_BUFFER);
			int Shift = 0;
			for (int repeat = 0; repeat < 16; repeat++)
			{
				for (int j = 0; j < NUMBEROFTHREADS; j++)
				{
					EncryptedBlocks.push_back(make_shared<ByteBlock>((BYTE*)(buf + Shift), 16));
					Threads.push_back(async(foo::crypt, *Cipher, EncryptedBlocks[j], 1));
					Shift += 16;
				}
				for (int k = 0; k < NUMBEROFTHREADS; k++)
				{
					if (Threads[k].get())
					{
						fw.write((char*)EncryptedBlocks[k]->byte_ptr(), 16);
					}
				}
				Threads.clear();
				EncryptedBlocks.clear();
			}
			Ready += MT_BUFFER;
		}
	delete[] buf;
	char BUFFER[BUFFERSIZE];
	_ToInsert.close();
	int tmp = 0;
	if (old != 0)
	{
		while (!fr.eof())
		{
			if ((FSize - tmp) < BUFFERSIZE)
			{
				fr.read(BUFFER, FSize - tmp);
				fw.write(BUFFER, FSize - tmp);
				break;
			}
			fr.read(BUFFER, sizeof(BUFFER));
			fw.write(BUFFER, sizeof(BUFFER));
		}
	}
	fw.close();
	fr.close();
	EndSession(trash, trash_tmp);
  }
  else return;
}
void Archive::Insert(const vector<ToInsert> &Files)
{
	if (!Cipher) return;
	for (auto& itr : Files) { if (!exists(itr.File)) return; }
	char buf[BUFFERSIZE];
	int old = NumberOfFiles;
	NumberOfFiles += Files.size();
	string trash = Main.parent_path().string();
	trash.append("\\trash");
	trash.append(Main.extension().string());
	string trash_tmp = Main.parent_path().string();
	trash_tmp.append("\\temp");
	trash_tmp.append(Main.extension().string());
	std::ofstream fw(trash, std::ofstream::binary);
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(0,ios::end);
	int full = fr.tellg();
	fr.seekg(0,ios::beg);
	std::ifstream _ToInsert;
	_ToInsert.seekg(0, ios::end);
	fw << NumberOfFiles;
	for (auto& itr : Files)
	{
		string tmp;
		_ToInsert.open(itr.File.string(), std::ifstream::binary);
		_ToInsert.seekg(0, ios::end);
		int f_size = _ToInsert.tellg();
		_ToInsert.close();
		string Info = foo::ComposeInfoBlock(itr.File, itr.Directory);
		fw << Info;
	}
	foo::WriteInfoBlocks(fr, fw, old);
	const int MainSize = full - fr.tellg();
	long long Written=0;
	long long RealSize=0;
	char *Buffer = new char[MT_BUFFER];
	for (auto& itr : Files)
	{
		if (is_regular_file(itr.File))
		{
			_ToInsert.open(itr.File.string(), std::ifstream::binary);
			_ToInsert.seekg(0,ios::end);
			RealSize = _ToInsert.tellg();
			_ToInsert.seekg(0,ios::beg);
			vector<TextBlock> EncryptedBlocks;
			int Ready = 0;
			vector<future<bool>> Threads;
			char SmallBuffer[16];
			while (!_ToInsert.eof())
			{
				if (RealSize - Ready == 0) break;
				if ((RealSize - Ready) < MT_BUFFER)
				{
					while (!_ToInsert.eof())
					{
						if (RealSize - Ready < 16)
						{
							_ToInsert.read(SmallBuffer, RealSize - Ready);
							for (int i = RealSize - Ready; i < 16; i++)
							{
								SmallBuffer[i] = 0;
							}
							ByteBlock _ToCrypt((BYTE*)SmallBuffer, 16);
							Cipher->encrypt(_ToCrypt, _ToCrypt);
							fw.write((char*)_ToCrypt.byte_ptr(), 16);
							break;
						}
						_ToInsert.read(SmallBuffer, 16);
						ByteBlock __ToCrypt((BYTE*)SmallBuffer, 16);
						Cipher->encrypt(__ToCrypt, __ToCrypt);
						char* EncryptedPtr = (char*)__ToCrypt.byte_ptr();
						fw.write(EncryptedPtr, 16);
						Ready += 16;
					}
					break;
				}
				_ToInsert.read(Buffer, MT_BUFFER);
				int Shift = 0;
				for (int repeat = 0; repeat < 16; repeat++)
				{
					for (int j = 0; j < NUMBEROFTHREADS; j++)
					{
						EncryptedBlocks.push_back(make_shared<ByteBlock>((BYTE*)(Buffer + Shift), 16));
						Threads.push_back(async(foo::crypt, *Cipher, EncryptedBlocks[j], 1));
						Shift += 16;
					}
					for (int k = 0; k < NUMBEROFTHREADS; k++)
					{
						if (Threads[k].get())
						{
							fw.write((char*)EncryptedBlocks[k]->byte_ptr(), 16);
						}
					}
					Threads.clear();
					EncryptedBlocks.clear();
				}
				Ready += MT_BUFFER;
				cout << RealSize - Ready << '\n';
			}
			_ToInsert.close();
		}
	}
	delete[] Buffer;
    Written = 0;
	if (old != 0)
	{
		if (MainSize < BUFFERSIZE)
		{
			fr.read(buf, MainSize);
			fw.write(buf, MainSize);
		}
		else
		{
			while (!_ToInsert.eof())
			{
				if ((MainSize - Written) < BUFFERSIZE)
				{
					fr.read(buf, MainSize - Written);
					fw.write(buf, MainSize - Written);
					break;
				}
				fr.read(buf, BUFFERSIZE);
				fw.write(buf, BUFFERSIZE);
				Written += BUFFERSIZE;
			}
		}
	}
	fw.close();
	fr.close();
	EndSession(trash, trash_tmp);
}
void Archive::ExtractDirectory(const path &SearchDir, const path Directory)
{
	std::ifstream fr(Main.string(), std::ifstream::binary);
	fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
	vector<char> tmp;
	vector<ToExtract> Files;
	int type = 0;
	path BegPath = SearchDir;
	string search, NecSize, FullDir;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		GetInfoBlock(tmp, fr);
		InfoBlock str;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
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

		if (foo::IsParent(SearchDir,FullDir))
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
void Archive::ExtractFile(const string &FileName, path Directory, const path &SearchDir)
{
	if (!Cipher) return;
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
	string search, NecSize;
	path  _SearchDir;
	int trash = 0, _NecSize = 0;
	vector<char> tmp;
	for (int i = 0; i < NumberOfFiles; i++)
	{
		GetInfoBlock(tmp, fr);
		InfoBlock str;
		str.Input(tmp);
		search = str.Name();
		NecSize = str.Size();
		_SearchDir = str.FullDir();
		tmp.clear();
		if ((FileName==search) && (_SearchDir==SearchDir))
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
		int _find = 0;
		fr.seekg(LengthOfInteger(NumberOfFiles), std::ios::beg);
		SkipInfoBlock(fr);
		fr.seekg(trash, std::ios_base::cur);
		char *buf = new char[MT_BUFFER];
		vector<TextBlock> DecryptedBlocks;
		int Ready = 0;
		vector<future<bool>> Threads;
		char* SmallBuffer = new char[16];
		while (!fr.eof())
		{
			if (_NecSize - Ready == 0) break;
			if (_NecSize - Ready < MT_BUFFER)
			{
				for (int written = 0; written < _NecSize - Ready; written += 16)
				{
					fr.read(SmallBuffer, 16);
					ByteBlock ToCrypt((BYTE*)SmallBuffer, 16);
					Cipher->decrypt(ToCrypt, ToCrypt);
					for (int k = 0; k < 16; ++k)
					{
						if ((char)ToCrypt.byte_ptr()[k] != 0) fw << ToCrypt.byte_ptr()[k];
					}
				}
				break;
			}
			fr.read(buf, MT_BUFFER);
			int Shift = 0;
			for (int repeat = 0; repeat < 16; repeat++)
			{
				for (int j = 0; j < NUMBEROFTHREADS; j++)
				{
					DecryptedBlocks.push_back(make_shared<ByteBlock>((BYTE*)buf + Shift, 16));
					Threads.push_back(async(crypt, *Cipher, DecryptedBlocks[j], 0));
					Shift += 16;
				}
				for (int k = 0; k < NUMBEROFTHREADS; k++)
				{
					if (Threads[k].get());
					fw.write((char*)DecryptedBlocks[k]->byte_ptr(), 16);
				}
				Threads.clear();
				DecryptedBlocks.clear();
			}
			Ready += MT_BUFFER;
		}
		delete[] buf;
		delete[] SmallBuffer;
		fw.close();
	}
	fr.close();
}
void Archive::InsertDirectory(path Directory, string Path, vector<ToInsert>& ObjectsToInsert, int iter)
{
	ToInsert SingleObject;
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
		SingleObject.File = Directory;
		SingleObject.Directory = New;
		ObjectsToInsert.push_back(SingleObject);
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
				SingleObject.File = tmp;
				SingleObject.Directory = New;
				ObjectsToInsert.push_back(SingleObject);
				InsertDirectory(tmp, New, ObjectsToInsert,iter);
				New = Path;
			}
			if (is_regular_file(tmp))
			{
				SingleObject.File = tmp;
				SingleObject.Directory = Path;
				ObjectsToInsert.push_back(SingleObject);
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
void Archive::ExploreDirectory(vector<path>&Files, vector<path>& Folders,const path& Directory)
{
	std::ifstream fr(Main.string(), ios::binary);
	if (NumberOfFiles > 0)
	{
		Archive::InfoBlock str;
		vector<char> tmp;
		string Search;
		path Path;
		int Type = 0;
		fr.seekg(foo::LengthOfInteger(NumberOfFiles), ios::beg);
		for (int i = 0; i < NumberOfFiles; i++)
		{
			GetInfoBlock(tmp, fr);
			str.Input(tmp);
			Path = str.FullDir();
			Type = str.Type();
			if ((Type == Type::File) && (Path == Directory) && (Directory == "0"))
			{
				Search = str.Name();
				Files.push_back(Search);
			}
			if ((Type == Type::Directory) && (!Path.has_parent_path()) && (Directory == "0"))
			{
				Search = str.Name();
				Folders.push_back(Search);
			}
			if ((Type == Type::File) && (Path == Directory) && (Directory != "0"))
			{
				Search = str.Name();
				Files.push_back(Search);
			}
			if ((Type == Type::Directory) && (Path.parent_path() == Directory) && (Directory != "0"))
			{
				Search = str.Name();
				Folders.push_back(Search);
			}
			tmp.clear();
		}
	}
}
void Archive::GetInfoBlock(vector<char>& str,std::ifstream& File)
{
	str.clear();
	int s = 0;
	char t;
	while (!File.eof())
	{
		File >> t;
		if (t == '>')
		{
			s++;
			if (s == 5) break;
		}
		str.push_back(t);
	}
}
void Archive::EndSession(const string& s1, const string& s2)
{
	try
	{
		rename(Main, s2);
		rename(s1, Main);
	}
	catch (filesystem_error &e)
	{
		cout << e.what();
	}
	boost::filesystem::remove(s2);
}
path Archive::GetFilePath(){return Main;}
void Archive::Lock()
{
	if (!_IsLocked)
	{
		delete Cipher;
	}
	_IsLocked = true;
}
void Archive::InputKey(string& key)
{
	Lock();
	BYTE* tmp = (BYTE*)key.c_str();
	ByteBlock KEY(tmp, 32);
	Cipher = new Kuznyechik(KEY);
	_IsLocked = false;
}
int Archive::GetNumberOfFiles(){return NumberOfFiles;}
Archive::~Archive(){ if (!_IsLocked)delete Cipher;}
