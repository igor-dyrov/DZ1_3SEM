#include <iostream>   
#include <boost\filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>        
#include <vector>

using namespace std;
using namespace boost::filesystem;


#define TypeChar 2
#define FirstNameChar 5

namespace foo
{
	int LengthOfInteger(double w); //Returns the lenth of the number(to know how many bytes myst be reserved)
	void  CreateDir(const path &ToCreate);
	path Find(path BegPath, string Name, string Type);
	string MakePathCorrect(const string &BegPath, const string &Search); //Removes useless part of the path (for "Test//Dir1//Dir2//blabla" and "Test//Dir1" we'll get "//Dir1//Dir2//blabla", because we need just the "Dir1" folder)
}

using namespace foo;

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
	path Main;//path to the archive
	int NumberOfFiles;
public:
	vector<CommonInfo> AddedObjects;
	Archive(const string &CreateNew, string &Name);
	Archive(const path& Real);
	void print();//to print the list of all objects in the archive
	void GetInfo();//gets Inforamtion to print it
	void RemoveObject(const string &Name, const string &SearchDir, int Type);//Deletes some object from the archive(1 - the name of the object, 2-the path to this object in my archive, 3- type of the object(1-file,0 -folder)
	void Insert(const path &File, const string &Directory);//prepares the InfoBlock and sends it in FixData with the path to the object and new number of objects in archive)\
				Warning: this func can only insert file or the directory(without inserting files and folders in it)
	void ExtractDirectory(const string &SearchDir, const path Directory);//extracts the directory with all objects in it(1 - the path to the directory in archive(FULL PATH), 2 - the path in the external storage(where to insert)
	void ExtractFile(const string &FileName, path Directory, const string &SearchDir);//1 - the name of the file, 2 - where to insert, 2 - the path to this file in the archive
	void InsertDirectory(path Directory, string Path, int iter =0);//1 - path in the external storage to the needed folder, 2- folder in the archive(where to insert the object), 3 - iteration variable\
				this func inserts ALL objects in the folder to the archive(even folder/folder/folder/1.jpg)
	~Archive();
};