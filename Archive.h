#pragma once
#include <wx/progdlg.h>
#include "Kuznyechik.hpp"
#include "mycrypto.hpp"
#include <stdio.h>
#include <future>
#include <boost\filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>   
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>        
#include <vector>
using namespace boost::filesystem;
using namespace std;

enum Type{Directory=0,File=1};

#define TypeChar 2
#define FirstNameChar 5
#define BUFFERSIZE 16
#define NUMBEROFSEPARATORS 5
#define MT_BUFFER 2048
#define NUMBEROFTHREADS 8

typedef shared_ptr<ByteBlock> TextBlock;


namespace foo
{
	struct ToWrite
	{
		int Ignore, Write;
	};
	int LengthOfInteger(int w); //Returns the lenth of the number(to know how many bytes myst be reserved)
	void  CreateDir(const path &ToCreate);
	path Find(path BegPath, string Name, string Type);
	string MakePathCorrect(const string &BegPath, const string &Search); //Removes useless part of the path (for "Test//Dir1//Dir2//blabla" and "Test//Dir1" we'll get "//Dir1//Dir2//blabla", because we need just the "Dir1" folder)
	void CopyDir(path const & Source, path const & Destination, string CurrentPath, wxProgressDialog& dialog, int iter = 0);
	bool IsParent(path p1, path p2);
	void NumberOfObjects(path& Folder, int& count);
	void ReplaceSpaces(string& Target);
	void WriteAndIgnore(const vector<ToWrite>& Source, std::ifstream& From, std::ofstream& To);
	void WriteToTheLast(std::ifstream& From,std::ofstream* To);
	bool crypt(Kuznyechik cipher, TextBlock ToCrypt, int mode);
	string ComposeInfoBlock(const path &File, const string &Directory);
	void WriteInfoBlocks(std::ifstream&, std::ofstream&,int old);
}

class Archive
{
private:
	bool _IsLocked;
	int NumberOfFiles;
	void GetInfoBlock(vector<char>&,std::ifstream&);
	void EndSession(const string& s1, const string& s2);
	void SkipInfoBlock(std::ifstream&);
	Kuznyechik *Cipher;
	struct InfoBlock//Information string that looks like "<<1><File.mp3><Test//Dir1//Dir2><12345>>" 1/0 - type of object(1 - file, 0 - folder), then name of the object, path in the archive and finaly size of the FILE
	{
		string tmp;
		void Input(const vector<char> &New);
		//the methods below returns the needed parts of the InfoBlock
		string Name();
		string Size();
		string SearchDir(string __SearchDir);//the method to check the attachment the object to __Searchdir folder in archive(func returns the part of the object path which lenghth is equal to the arg length)\
													example: file is in Dir1/Dir2/Dir3 folder, we are sending "Dir1/Dir2" as __SearchDir and getting "Dir1/Dir2", it means that it's necessary file
		string FullDir();//full path of the object in archive
		int Type();
		int Length();//the length of the InfoBlock
	};
public:
	Archive();
	Archive(const path& File);
	struct ToExtract//in the code below I'm creating the vector of the files to Extract using this struct
	{
		string Name, SearchDir;
		path WhereToInsert;
	};
	struct ToInsert
	{
		path File;
		string Directory;
	};
	struct CommonInfo//it's necessary to get Information about Objects in my archive
	{
		int size;
		bool isFile;
		string FullDir, Name;
	};
	path Main;//path to the archive
	void GetInfo();//gets Inforamtion to print it
	void RemoveObject(const string &Name, const path &SearchDir, int Type);//Deletes some object from the archive(1 - the name of the object, 2-the path to this object in my archive, 3- type of the object(1-file,0 -folder)
	void Insert(const path &File, const string &Directory);
	void Insert(const vector<ToInsert> &Files);//To avoid standart algorithm of inserting single object and to improve speed of this operation I overrided the func, now it'll write new files in sequence\
	The first variant of this func is writing one file and then the old bytes, so for several files this operation is seemed to be slow
	void ExtractDirectory(const path &SearchDir, const path Directory);//extracts the directory with all objects in it(1 - the path to the directory in archive(FULL PATH), 2 - the path in the external storage(where to insert)
	void ExtractFile(const string &FileName, path Directory, const path &SearchDir);//1 - the name of the file, 2 - where to insert, 2 - the path to this file in the archive
	void InsertDirectory(path Directory, string Path,vector<ToInsert>& ObjectsToInsert, int iter =0);//1 - path in the external storage to the needed folder, 2- folder in the archive(where to insert the object), 3 - iteration variable\
				this func inserts ALL objects in the folder to the archive(even folder/folder/folder/1.jpg)
	void OpenNew(path& File);
	void ExploreDirectory(vector<path>& Files,vector<path>& Folders,const path& Directory);
	int GetNumberOfFiles();
	path GetFilePath();
	void Lock();
	void InputKey(string& key);
	vector<CommonInfo> AddedObjects;
	bool IsLocked() { return _IsLocked; }
	~Archive();
};