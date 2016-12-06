#pragma once

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/listctrl.h>
#include <wx/listbook.h>
#include <wx/progdlg.h>
#include <wx/utils.h> 
#include <wx/busyinfo.h>
#include "Archive.h"
#endif

enum TypeOfObject {Folder = 0, DefaultFile = 1, Text  = 2, Music =3, Zip =4, Photo= 5, App =6, Video =7 };



#define ROOTLENGTH 5
class ArchiveForm;
class Form :public wxFrame
{
public:
	Form();
	void ListInit();
	void ListObjectOpened(wxListEvent& event);
	void SelectPath(wxListEvent& event);
	void UpList(wxCommandEvent& event);
	void BackList(wxCommandEvent& event);
	void HomeList(wxCommandEvent& event);
	void ForwardList(wxCommandEvent& event);
	void SetList(wxCommandEvent& event);
	void UpdateList(wxCommandEvent& event);
	void CopyObject(wxCommandEvent& event);
	void PasteObject(wxCommandEvent& event);
	void RemoveObject(wxCommandEvent& event);
	void CreateNewObject(wxCommandEvent& event);
	template<class T> void ErrorOutput(T &exception);
	void CloseProgram(wxCommandEvent& event);
	void Terminate(wxCloseEvent& event);
	string GetListPath();
	path GetPathToCopy(){ return PathToCopy; }
	void PastedToArchive(){ IsPasted = true; }
	ArchiveForm* ArchiveWindow;
	void CreateWindow(string& key,path&);
	~Form();
private:
	int ListIndex;
	vector<path> FolderHistory;
	wxListCtrl* FileList;
	wxTextCtrl* PathControl;
	path ListPath;
	path SelectedPath,PathToCopy;
	bool CanCopy,IsPasted;
	wxBitmapButton* RemoveButton;
	wxBitmapButton* UpButton;
	wxBitmapButton* HomeButton;
	wxBitmapButton* BackButton;
	wxBitmapButton* ForwardButton;
	wxBitmapButton* UpdateButton;
	wxBitmapButton* PasteButton;
	wxBitmapButton* CopyButton;
	wxBoxSizer *vbox;
	wxBoxSizer *hbox2;
	wxPanel* MainPanel;
	wxImageList* Icos;
	wxMenuBar *MainMenuBar;
	wxMenu *SettingsMenu;
	wxMenu *FileMenu;
	DECLARE_EVENT_TABLE()
};

struct SelectedArchiveObject
{
	int Type;
	string Name;
	path Path;
};

class ArchiveForm : public wxFrame
{
public:
	ArchiveForm(Form* parent, path& File,string& Key);
	void ListInit();
	void ListObjectOpened(wxListEvent& event);
	void SelectPath(wxListEvent& event);
	void UpList(wxCommandEvent& event);
	void BackList(wxCommandEvent& event);
	void HomeList(wxCommandEvent& event);
	void ForwardList(wxCommandEvent& event);
	void SetList(wxCommandEvent& event);
	void UpdateList(wxCommandEvent& event);
	void CopyObject(wxCommandEvent& event);
	void PasteObject(wxCommandEvent& event);
	void RemoveObject(wxCommandEvent& event);
	template<class T> void ErrorOutput(T &exception);
	void Terminate(wxCloseEvent& event);
	string GetListPath();
	void ExtractToFilesystem();
	~ArchiveForm();
private:
	Form* Parent;
	Archive File;
	SelectedArchiveObject SelectedOne;
	SelectedArchiveObject CopiedOne;
	int ListIndex;
	vector<path> FolderHistory;
	wxListCtrl* ArchiveList;
	wxTextCtrl* PathControl;
	path ListPath;
	bool CanCopy, IsPasted;
	wxBitmapButton* RemoveButton;
	wxBitmapButton* UpButton;
	wxBitmapButton* HomeButton;
	wxBitmapButton* BackButton;
	wxBitmapButton* ForwardButton;
	wxBitmapButton* UpdateButton;
	wxBitmapButton* PasteButton;
	wxBitmapButton* CopyButton;
	wxBoxSizer *vbox;
	wxBoxSizer *hbox2;
	wxPanel* MainPanel;
	wxImageList* Icos;
	wxMenuBar *MainMenuBar;
	wxMenu *SettingsMenu;
	wxMenu *FileMenu;
	DECLARE_EVENT_TABLE()
};


class SaveAndExit : public wxDialog
{
public:
	SaveAndExit(wxFrame*);
	void SafetyClosing(wxCommandEvent& event);
	void Return(wxCommandEvent& event);
private:
	wxFrame* Parent;
};

class NewFileDialog : public wxDialog
{
public:
	NewFileDialog(Form*);
	void Create(wxCommandEvent& event);
	void MakeFile(wxCommandEvent& event);
	void MakeFolder(wxCommandEvent& event);
private:
	Form* Parent;
	wxTextCtrl* GetText;
	path ObjectToCreate;
	bool IsFile;
	DECLARE_EVENT_TABLE()
};
class ConfirmDeleting : public wxDialog
{
public:
	ConfirmDeleting(Form* parent,const path& Object);
	void Return(wxCommandEvent& event);
	void Delete(wxCommandEvent& event);
private:
	Form* Parent;
	const path ToDel;
};
class PasswordInput : public wxDialog
{
public:
	PasswordInput(Form* ptr,path& _ToOpen) : wxDialog(NULL, -1, "Input key", wxDefaultPosition, wxSize(260, 100))
	{
		par = ptr;
		ToOpen = _ToOpen;
		wxPanel* Panel = new wxPanel(this, wxID_ANY);
		wxButton* YesButton = new wxButton(Panel, wxID_OK, wxT("OK"), wxPoint(140, 20));
		PasswordBox = new wxTextCtrl(Panel, wxID_FILE4, "", wxPoint(10, 20), wxSize(120, 25),  wxTE_PASSWORD);
		Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PasswordInput::SetPassword));
	}
private:
	void SetPassword(wxCommandEvent& event)
	{
		string Result = PasswordBox->GetValue();
		par->CreateWindow(Result,ToOpen);
		Destroy();
	}
	Form* par;
	path ToOpen;
	wxTextCtrl* PasswordBox;
	
};
class ObjectRemovingDialog : public wxDialog
{
public:
	ObjectRemovingDialog() : wxDialog(NULL, -1, "Do You really want to remove it?", wxDefaultPosition, wxSize(260, 150))
	{
		wxPanel* Panel = new wxPanel(this, wxID_ANY);
		wxButton* YesButton = new wxButton(Panel, wxID_YES, wxT("Yes"), wxPoint(30, 60));
		wxButton* NoButton = new wxButton(Panel, wxID_NO, wxT("No"), wxPoint(130, 60));
	}
	void Return(wxCommandEvent& event);
	void Delete(wxCommandEvent& event);
private:
	Form* Parent;
	const path ToDel;
	path File;
};
class PleaseWait : public wxDialog
{
public:
	PleaseWait(string text) : wxDialog(NULL, -1, text, wxDefaultPosition, wxSize(260, 100))
	{
	}
};
class ObjectRemovig : public wxThread
{
public:
	ObjectRemovig(ArchiveForm *ptr,Archive *_File,SelectedArchiveObject& _SelectedOne) : wxThread(wxTHREAD_DETACHED)
	{
		PTR = ptr;
		File = _File;
		SelectedOne = _SelectedOne;
	}
	virtual void *Entry()
	{
		PTR->Disable();
		File->RemoveObject(SelectedOne.Name, SelectedOne.Path, SelectedOne.Type);
		PTR->Enable();
		PTR->ListInit();
		return 0;
	}
private:
	Archive* File;
	SelectedArchiveObject SelectedOne;
	ArchiveForm * PTR;
};
class ObjectInserting : public wxThread
{
public:
	ObjectInserting(ArchiveForm*PTR,Archive *_File, SelectedArchiveObject& _SelectedOne) : wxThread(wxTHREAD_DETACHED)
	{
		_PTR = PTR;
		File = _File;
		SelectedOne = _SelectedOne;
	}
	virtual void *Entry()
	{
		_PTR->Disable();
		if (SelectedOne.Type == Type::File)
		{
			File->Insert(SelectedOne.Name, SelectedOne.Path.string());
		}
		else
		{
			if (SelectedOne.Path.string() == "0") SelectedOne.Path = "";
			vector<Archive::ToInsert> vec;
			File->InsertDirectory(SelectedOne.Name, SelectedOne.Path.string(), vec);
			File->Insert(vec);
		}
		_PTR->Enable();
		_PTR->ListInit();
		return 0;
	}
private:
	Archive *File;
	ArchiveForm*_PTR;
	SelectedArchiveObject SelectedOne;
};
class ObjectExtracting : public wxThread
{
public:
	ObjectExtracting(ArchiveForm*PTR,Form*__PTR, Archive *_File, SelectedArchiveObject& _SelectedOne) : wxThread(wxTHREAD_DETACHED)
	{
		F_PTR = __PTR;
		_PTR = PTR;
		File = _File;
		SelectedOne = _SelectedOne;
	}
	virtual void *Entry()
	{
		_PTR->Disable();
		if (SelectedOne.Type == Type::File)
		{
			path tmp = F_PTR->GetListPath();
			File->ExtractFile(SelectedOne.Name, tmp, SelectedOne.Path);
		}
		else
		{
			string tmp = F_PTR->GetListPath();
			File->ExtractDirectory(SelectedOne.Path, tmp);
		}
		_PTR->Enable();
		F_PTR->ListInit();
		return 0;
	}
private:
	Archive *File;
	Form* F_PTR;
	ArchiveForm*_PTR;
	SelectedArchiveObject SelectedOne;
};