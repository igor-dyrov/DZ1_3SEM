#include "GUI.h"

void SetFileIcon(wxListCtrl* List, const string &Type)
{
	if ((Type == ".doc") || (Type == ".docx") || (Type == ".txt") || (Type == ".pdf") || (Type == ".xlsx") || (Type == ".ini") || (Type == ".djvu")) List->SetItemImage(0, Text);
	else if (Type == ".mp3") List->SetItemImage(0, Music);
	else if ((Type == ".zip") || (Type == ".rar") || (Type == ".7z")) List->SetItemImage(0, Zip);
	else if ((Type == ".jpg") || (Type == ".png") || (Type == ".bmp") || (Type == ".ico") || (Type == ".gif")) List->SetItemImage(0, Photo);
	else if (Type == ".exe") List->SetItemImage(0, App);
	else if ((Type == ".mp4") || (Type == ".avi") || (Type == ".flv")) List->SetItemImage(0, Video);
	else List->SetItemImage(0, DefaultFile);
}

SaveAndExit::SaveAndExit(wxFrame* parent) : wxDialog(NULL, -1, "Are you sure?", wxDefaultPosition, wxSize(260, 150))
{
	Parent = parent;
	wxPanel* Panel = new wxPanel(this, wxID_ANY);
	wxButton* YesButton = new wxButton(Panel, wxID_YES, wxT("Yes"), wxPoint(30, 60));
	wxButton* NoButton = new wxButton(Panel, wxID_NO, wxT("No"), wxPoint(130, 60));
	Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveAndExit::SafetyClosing));
	Connect(wxID_NO, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SaveAndExit::Return));
	Centre();
	ShowModal();
	Destroy();
	
}
void SaveAndExit::SafetyClosing(wxCommandEvent& event)
{
	Destroy();
	Parent->Destroy();
}
void SaveAndExit::Return(wxCommandEvent& event)
{
    Destroy();
}

NewFileDialog::NewFileDialog(Form* par): wxDialog(NULL, -1, "New Object", wxDefaultPosition, wxSize(260, 150))
{
	Parent = par;
	wxPanel* Panel = new wxPanel(this, wxID_ANY);
	wxButton* YesButton = new wxButton(Panel, wxID_YES, wxT("Create!"), wxPoint(0, 30),wxSize(60,40));
	wxRadioButton* File = new wxRadioButton(Panel, wxID_FILE1, "File", wxPoint(60, 10), wxDefaultSize,wxRB_GROUP);
	wxRadioButton* _Folder = new wxRadioButton(Panel, wxID_FILE2, "Folder", wxPoint(100, 10));
	GetText = new wxTextCtrl(Panel, wxID_FILE, "", wxPoint(60, 30), wxSize(180, 40));
	IsFile = true;
	Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewFileDialog::Create));
	ShowModal();
	Destroy();
}
void NewFileDialog::MakeFile(wxCommandEvent& event)
{
	IsFile = true;
}
void NewFileDialog::MakeFolder(wxCommandEvent& event)
{
	IsFile = false;
}
void NewFileDialog::Create(wxCommandEvent& event)
{
	string tmp = GetText->GetValue();
	if (tmp == "") return;
	ObjectToCreate = Parent->GetListPath();
	ObjectToCreate.append(tmp);
	if (IsFile)
	{
		std::ofstream ToCreate(ObjectToCreate.string());
		ToCreate.close();
	}
	else create_directory(ObjectToCreate);
	Destroy();
	Parent->ListInit();
}
BEGIN_EVENT_TABLE(NewFileDialog,wxDialog)
EVT_RADIOBUTTON(wxID_FILE1, NewFileDialog::MakeFile)
EVT_RADIOBUTTON(wxID_FILE2, NewFileDialog::MakeFolder)
END_EVENT_TABLE()

ConfirmDeleting::ConfirmDeleting(Form* parent,const path& Object) :wxDialog(NULL, -1, "Do You really want to remove it?", wxDefaultPosition, wxSize(260, 150)), Parent(parent), ToDel(Object)
{
	wxPanel* Panel = new wxPanel(this, wxID_ANY);
	wxButton* YesButton = new wxButton(Panel, wxID_YES, wxT("Yes"), wxPoint(30, 60));
	wxButton* NoButton = new wxButton(Panel, wxID_NO, wxT("No"), wxPoint(130, 60));
	wxStaticText* _Text = new wxStaticText(Panel, wxID_ABOUT, "Object: \n" + ToDel.string(),wxPoint(0,0));
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	vbox->Add(_Text, wxALIGN_CENTRE | wxEXPAND | wxALL, 1);
	hbox->Add(YesButton, 0, wxALL, 1);
	hbox->AddStretchSpacer(1);
	hbox->Add(NoButton, 0, wxALL, 1);
	vbox->Add(hbox, 0, wxALIGN_TOP | wxGROW);
	Panel->SetSizer(vbox);
	vbox->Fit(this);
	vbox->SetSizeHints(this);
	Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfirmDeleting::Delete));
	Connect(wxID_NO, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfirmDeleting::Return));
	Centre();
	ShowModal();
	Destroy();
}
void ConfirmDeleting::Delete(wxCommandEvent& event)
{
	try
	{
		remove_all(ToDel);
		Parent->ListInit();
	}
	catch (filesystem_error &e)
	{
		Parent->ErrorOutput(e);
	}
	Destroy();
}
void ConfirmDeleting::Return(wxCommandEvent& event)
{
	Destroy();
}


Form::Form() : wxFrame(NULL, wxID_ANY, "Explorer", wxDefaultPosition, wxSize(340, 350))
{
	wxIcon MainIcon(wxT("Explorer"));
	wxIcon FileImage(wxT("FileStandart"));
	wxIcon FolderImage(wxT("FolderStandart"));
	wxIcon TextFile(wxT("TextFile"));
	wxIcon MusFile(wxT("MusFile"));
	wxIcon ZipFile(wxT("ZipFile"));
	wxIcon PhotoFile(wxT("PhotoFile"));
	wxIcon ExeFile(wxT("ExeFile"));
	wxIcon VideoFile(wxT("VideoFile"));
	wxIcon Home(wxT("Home"));
	wxIcon Up(wxT("Up"));
	wxIcon Back(wxT("Back"));
	wxIcon Forward(wxT("Forward"));
	wxIcon Remove(wxT("Delete"));
	wxIcon Update(wxT("Update"));
	wxIcon Copy(wxT("Copy"));
	wxIcon Paste(wxT("Paste"));
	wxBitmap _Home, _Up, _Back, _Forward, _Remove, _Paste, _Copy, _Update;
	SetIcon(MainIcon);
	_Home.CopyFromIcon(Home);
	_Up.CopyFromIcon(Up);
	_Back.CopyFromIcon(Back);
	_Forward.CopyFromIcon(Forward);
	_Remove.CopyFromIcon(Remove);
	_Copy.CopyFromIcon(Copy);
	_Paste.CopyFromIcon(Paste);
	_Update.CopyFromIcon(Update);
	MainPanel = new wxPanel(this, -1);
	vbox = new wxBoxSizer(wxVERTICAL);
	hbox2 = new wxBoxSizer(wxHORIZONTAL);
	MainMenuBar = new wxMenuBar;
	FileMenu = new wxMenu;
	SettingsMenu = new wxMenu;
	PathControl = new wxTextCtrl(MainPanel, wxID_FILE4, "", wxPoint(0, 50), wxSize(320, 20), wxTE_PROCESS_ENTER);
	UpButton = new wxBitmapButton(MainPanel, wxID_UP, _Up, wxPoint(120, 5));
	HomeButton = new wxBitmapButton(MainPanel, wxID_HOME, _Home, wxPoint(80, 5));
	BackButton = new wxBitmapButton(MainPanel, wxID_BACKWARD, _Back, wxPoint(0, 5));
	ForwardButton = new wxBitmapButton(MainPanel, wxID_FORWARD, _Forward, wxPoint(40, 5));
	RemoveButton = new wxBitmapButton(MainPanel, wxID_DELETE, _Remove, wxPoint(160, 5));
	UpdateButton = new wxBitmapButton(MainPanel, wxID_RETRY, _Update, wxPoint(200, 5));
	CopyButton = new wxBitmapButton(MainPanel, wxID_COPY, _Copy, wxPoint(240, 5));
	PasteButton = new wxBitmapButton(MainPanel, wxID_PASTE, _Paste, wxPoint(280, 5));
	FileList = new wxListCtrl(MainPanel, wxID_FILE1, wxPoint(0, 90), wxSize(320, 220));
	hbox2->Add(UpButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(HomeButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(BackButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(ForwardButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(RemoveButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(UpdateButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(CopyButton,0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(PasteButton, 0, wxALL, 1);
	vbox->Add(hbox2, 0, wxALIGN_TOP | wxGROW);
	vbox->Add(PathControl, 0, wxALIGN_CENTRE | wxEXPAND | wxALL, 1);
	vbox->Add(FileList, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 10);
	MainPanel->SetSizer(vbox);
	vbox->Fit(this);
	vbox->SetSizeHints(this);
	FileMenu->Append(wxID_NEW, wxT("&New"));
	FileMenu->Append(wxID_STOP, wxT("&Quit"));
	MainMenuBar->Append(FileMenu, wxT("&File"));
	SettingsMenu->Append(wxID_INFO, wxT("&Info"));
	SettingsMenu->Append(wxID_SETUP, wxT("&Configuration"));
	MainMenuBar->Append(SettingsMenu, wxT("&Settings"));
	ListPath = "D:\\FILES\\Downloads\\MOZILLA";
	FolderHistory.push_back(ListPath);
	ListIndex = 0;
	Icos = new wxImageList(16, 16);
	Icos->Add(FolderImage);
	Icos->Add(FileImage);
	Icos->Add(TextFile);
	Icos->Add(MusFile);
	Icos->Add(ZipFile);
	Icos->Add(PhotoFile);
	Icos->Add(ExeFile);
	Icos->Add(VideoFile);
	FileList->SetImageList(Icos, wxIMAGE_LIST_NORMAL);
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Form::Terminate));
	Connect(wxID_STOP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Form::CloseProgram));
	Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Form::CreateNewObject));
	Connect(wxID_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::UpList));
	Connect(wxID_HOME, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::HomeList));
	Connect(wxID_BACKWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::BackList));
	Connect(wxID_FORWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::ForwardList));
	Connect(wxID_RETRY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::UpdateList));
	Connect(wxID_COPY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::CopyObject));
	Connect(wxID_PASTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::PasteObject));
	Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Form::RemoveObject));
	CanCopy = false;
	IsPasted = true;
	SetMenuBar(MainMenuBar);
	ListInit();
	Centre();
};
Form::~Form()
{
	//delete ArchiveWindow;
	delete Icos;
}
void Form::ListInit()
{
	PathControl->Clear();
	FileList->ClearAll();
	PathControl->AppendText(ListPath.string());
	directory_iterator itr(ListPath);
	vector<path> Files;
	while (itr != directory_iterator{})
	{
		try
		{
			if (is_regular_file(*itr))
			{
				Files.push_back(*itr);
			}
			if (is_directory(*itr))
			{
				FileList->InsertItem(0, itr->path().filename().string(), 0);
				FileList->SetItemImage(0, Folder);
			}
			itr++;
		}
		catch (filesystem_error &e)
		{
			itr++;
		}
	}
	for (auto & i : Files)
	{
		FileList->InsertItem(0, i.filename().string(), 0);
		SetFileIcon(FileList, i.extension().string());
	}
	
}
void Form::ListObjectOpened(wxListEvent& event)
{
	int Image = event.GetImage();
	path tmp = event.GetText();
	if (Image == Folder)
	{
		string FolderName = event.GetText();
		ListPath.append(FolderName);
		if (FolderHistory.size()>1) FolderHistory.erase(FolderHistory.begin() + ListIndex+1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
		ListInit();
	}
	if (tmp.extension() == ".arc")
	{
		path ToOpen = ListPath;
		ToOpen.append(tmp.string());
		PasswordInput * dial = new PasswordInput(this,ToOpen);
		dial->Show();
	}
	else
	{
		string FileName = event.GetText();
		path tmp = ListPath;
		tmp.append(FileName);
		wxLaunchDefaultApplication(tmp.string());
		return;
	}
}
void Form::SelectPath(wxListEvent& event)
{
	if (IsPasted)
	{
		SelectedPath = ListPath;
		string a = event.GetText();
		SelectedPath.append(a);
	}
}
void Form::UpList(wxCommandEvent& event)
{
	if (ListPath.has_parent_path())
	{
		ListPath = ListPath.parent_path();
		if (FolderHistory.size()>1) FolderHistory.erase(FolderHistory.begin() + ListIndex + 1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
		ListInit();
	}
}
void Form::BackList(wxCommandEvent& event)
{
	if ((FolderHistory.size() > 1) && (ListIndex-1>=0))
	{
		ListIndex--;
		ListPath = FolderHistory[ListIndex];
		ListInit();
	}
}
void Form::HomeList(wxCommandEvent& event)
{
	path old = ListPath;
	ListPath = "C:\\";
	if (old != ListPath)
	{
		if (FolderHistory.size() > 1) FolderHistory.erase(FolderHistory.begin() + ListIndex + 1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
	}
	ListInit();
}
void Form::ForwardList(wxCommandEvent& event)
{
	if ((FolderHistory.size() > 1) && (ListIndex + 1<= FolderHistory.size()-1))
	{
		ListIndex++;
		ListPath = FolderHistory[ListIndex];
		ListInit();
	}
}
void Form::SetList(wxCommandEvent &event)
{
	path PathToTry = PathControl->GetValue();
	file_status s = status(PathToTry);
	if (exists(s))
	{
		ListPath = PathToTry;
		if (FolderHistory.size()>1) FolderHistory.erase(FolderHistory.begin() + ListIndex + 1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
		ListInit();
	}
}
void Form::UpdateList(wxCommandEvent& event)
{
	ListInit();
}
void Form::PasteObject(wxCommandEvent& event)
{
/*	if (!foo::IsParent(PathToCopy, ListPath))
	{
		try
		{
			if (is_regular_file(PathToCopy))
			{
				path tmp = ListPath;
				tmp.append(PathToCopy.filename().string());
				wxProgressDialog dialog(wxT("Copying..."), wxT("Please wait"), 1, this,wxPD_APP_MODAL);
				copy_file(PathToCopy, tmp);
				dialog.Update(1);
			}
			if (is_directory(PathToCopy))
			{
				int count = 0;
				foo::NumberOfObjects(PathToCopy, count);
				wxProgressDialog dialog(wxT("Copying..."), wxT("Please wait"),	count,    this,wxPD_APP_MODAL);
			    foo::CopyDir(PathToCopy, ListPath,"",dialog);
			}
			IsPasted = true;
		}
		catch (filesystem_error& e)
		{
			ErrorOutput(e);
			IsPasted = true;
		}
*/
	ArchiveWindow->ExtractToFilesystem();
	ListInit();
	//else wxMessageBox("The destination folder is parent for the source one!", "Error!", wxOK);
}
void Form::CopyObject(wxCommandEvent& event)
{
	IsPasted = false;
	PathToCopy = SelectedPath;
}
void Form::RemoveObject(wxCommandEvent& event)
{
	path PathToTry = SelectedPath;
	file_status s = status(PathToTry);
	if (exists(s))
	{
		ConfirmDeleting* NewDlg = new ConfirmDeleting(this, SelectedPath);
		NewDlg->Show();
	}
}
void Form::CreateNewObject(wxCommandEvent& event)
{
	NewFileDialog* Dial = new NewFileDialog(this);
	Dial->Show();
}
template<class T>
void Form::ErrorOutput(T &exception)
{
	wxMessageBox(exception.what(), "Error!",wxOK);
}
void Form::CloseProgram(wxCommandEvent& event)
{
	Close(true);
	return;
}
void Form::Terminate(wxCloseEvent& event)
{
	SaveAndExit* question = new SaveAndExit(this);
	question->Show();
}
string Form::GetListPath()
{
	return ListPath.string();
}
BEGIN_EVENT_TABLE(Form, wxFrame)
EVT_LIST_ITEM_ACTIVATED(wxID_FILE1, Form::ListObjectOpened)
EVT_LIST_ITEM_SELECTED(wxID_FILE1,Form::SelectPath)
EVT_TEXT_ENTER(wxID_FILE4, Form::SetList)
END_EVENT_TABLE()

ArchiveForm::ArchiveForm(Form* parent,path& _File,string& Key) : wxFrame(NULL, wxID_ANY, "Private Storage", wxDefaultPosition, wxSize(340, 350))
{
	File.OpenNew(_File);
	File.InputKey(Key);
	Parent = parent;
	wxIcon MainIcon(wxT("Main"));
	wxIcon FileImage(wxT("FileStandart"));
	wxIcon FolderImage(wxT("FolderStandart"));
	wxIcon TextFile(wxT("TextFile"));
	wxIcon MusFile(wxT("MusFile"));
	wxIcon ZipFile(wxT("ZipFile"));
	wxIcon PhotoFile(wxT("PhotoFile"));
	wxIcon ExeFile(wxT("ExeFile"));
	wxIcon VideoFile(wxT("VideoFile"));
	wxIcon Home(wxT("Home"));
	wxIcon Up(wxT("Up"));
	wxIcon Back(wxT("Back"));
	wxIcon Forward(wxT("Forward"));
	wxIcon Remove(wxT("Delete"));
	wxIcon Update(wxT("Update"));
	wxIcon Copy(wxT("Copy"));
	wxIcon Paste(wxT("Paste"));
	wxBitmap _Home, _Up, _Back, _Forward, _Remove, _Paste, _Copy, _Update;
	SetIcon(MainIcon);
	_Home.CopyFromIcon(Home);
	_Up.CopyFromIcon(Up);
	_Back.CopyFromIcon(Back);
	_Forward.CopyFromIcon(Forward);
	_Remove.CopyFromIcon(Remove);
	_Copy.CopyFromIcon(Copy);
	_Paste.CopyFromIcon(Paste);
	_Update.CopyFromIcon(Update);
	MainPanel = new wxPanel(this, -1);
	vbox = new wxBoxSizer(wxVERTICAL);
	hbox2 = new wxBoxSizer(wxHORIZONTAL);
	PathControl = new wxTextCtrl(MainPanel, wxID_FILE4, "", wxPoint(0, 50), wxSize(320, 20), wxTE_PROCESS_ENTER);
	UpButton = new wxBitmapButton(MainPanel, wxID_UP, _Up, wxPoint(120, 5));
	HomeButton = new wxBitmapButton(MainPanel, wxID_HOME, _Home, wxPoint(80, 5));
	BackButton = new wxBitmapButton(MainPanel, wxID_BACKWARD, _Back, wxPoint(0, 5));
	ForwardButton = new wxBitmapButton(MainPanel, wxID_FORWARD, _Forward, wxPoint(40, 5));
	RemoveButton = new wxBitmapButton(MainPanel, wxID_DELETE, _Remove, wxPoint(160, 5));
	UpdateButton = new wxBitmapButton(MainPanel, wxID_RETRY, _Update, wxPoint(200, 5));
	CopyButton = new wxBitmapButton(MainPanel, wxID_COPY, _Copy, wxPoint(240, 5));
	PasteButton = new wxBitmapButton(MainPanel, wxID_PASTE, _Paste, wxPoint(280, 5));
	ArchiveList = new wxListCtrl(MainPanel, wxID_FILE1, wxPoint(0, 90), wxSize(320, 220));
	hbox2->Add(UpButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(HomeButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(BackButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(ForwardButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(RemoveButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(UpdateButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(CopyButton, 0, wxALL, 1);
	hbox2->AddStretchSpacer(1);
	hbox2->Add(PasteButton, 0, wxALL, 1);
	vbox->Add(hbox2, 0, wxALIGN_TOP | wxGROW);
	vbox->Add(PathControl, 0, wxALIGN_CENTRE | wxEXPAND | wxALL, 1);
	vbox->Add(ArchiveList, 1, wxALIGN_CENTRE | wxEXPAND | wxALL, 10);
	MainPanel->SetSizer(vbox);
	vbox->Fit(this);
	vbox->SetSizeHints(this);
	ListPath = "0";
	FolderHistory.push_back(ListPath);
	ListIndex = 0;
	Icos = new wxImageList(16, 16);
	Icos->Add(FolderImage);
	Icos->Add(FileImage);
	Icos->Add(TextFile);
	Icos->Add(MusFile);
	Icos->Add(ZipFile);
	Icos->Add(PhotoFile);
	Icos->Add(ExeFile);
	Icos->Add(VideoFile);
	ArchiveList->SetImageList(Icos, wxIMAGE_LIST_NORMAL);
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ArchiveForm::Terminate));
	//Connect(wxID_STOP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ArchiveForm::CloseProgram));
	//Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ArchiveForm::CreateNewObject));
	Connect(wxID_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::UpList));
	Connect(wxID_HOME, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::HomeList));
	Connect(wxID_BACKWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::BackList));
	Connect(wxID_FORWARD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::ForwardList));
	Connect(wxID_RETRY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::UpdateList));
	Connect(wxID_COPY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::CopyObject));
	Connect(wxID_PASTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::PasteObject));
	Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ArchiveForm::RemoveObject));
	CanCopy = false;
	IsPasted = true;
	ListInit();
	Centre();
};
void Form::CreateWindow(string&key,path&ToOpen)
{

	ArchiveWindow = new ArchiveForm(this, ToOpen, key);
	ArchiveWindow->Show();
}
void ArchiveForm::ListInit()
{
	PathControl->Clear();
	ArchiveList->ClearAll();
	if (ListPath == "0")
	{
		PathControl->AppendText("$root");
	}
	else
	{
		path tmp = "$root";
		tmp.append(ListPath.string());
		PathControl->AppendText(tmp.string());
	}
	vector<path> Files;
	vector<path> Folders;
	File.ExploreDirectory(Files, Folders, ListPath);
	for (auto &i : Folders)
	{
	  ArchiveList->InsertItem(0, i.string(), 0);
	  ArchiveList->SetItemImage(0, Folder);
	}
	for (auto &i : Files)
	{
		ArchiveList->InsertItem(0, i.string(), 0);
		SetFileIcon(ArchiveList, i.extension().string());
	}
}
void ArchiveForm::ExtractToFilesystem()
{
	ObjectExtracting* thread = new ObjectExtracting(this, Parent, &File, CopiedOne);
	thread->Run();
	IsPasted = true;
}
void ArchiveForm::CopyObject(wxCommandEvent& event)
{
	if (IsPasted) CopiedOne = SelectedOne;
	IsPasted = false;
}
ArchiveForm::~ArchiveForm()
{
	delete Icos;
}
void ArchiveForm::ListObjectOpened(wxListEvent& event)
{
	int Image = event.GetImage();
	if (Image == Folder)
	{
		string FolderName = event.GetText();
		if (ListPath == "0")
		{
			ListPath = FolderName;
		}
		else ListPath.append(FolderName);
		if (FolderHistory.size()>1) FolderHistory.erase(FolderHistory.begin() + ListIndex + 1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
		ListInit();
	}
}
void ArchiveForm::SelectPath(wxListEvent& event)
{
	if (IsPasted)
	{
		SelectedOne.Name = event.GetText();
		if (event.GetImage() == Folder)
		{
			SelectedOne.Type = 0;
			if(ListPath!="0")SelectedOne.Path = ListPath;
			SelectedOne.Path.append(SelectedOne.Name);
		}
		else
		{
			SelectedOne.Type = 1;
			SelectedOne.Path = ListPath;
		}
	}
}
void ArchiveForm::RemoveObject(wxCommandEvent &event)
{
	ObjectRemovig* Thread = new ObjectRemovig(this,&File, SelectedOne);
	Thread->Run();
}
void ArchiveForm::PasteObject(wxCommandEvent& event)
{
	path ToTry = Parent->GetPathToCopy();
	Type t;
	if (is_regular_file(ToTry)) t = Type::File;
	else t = Type::Directory;
	path tmp = ListPath;
	if (tmp == "0") tmp = "";
	else tmp = tmp.relative_path();
	SelectedArchiveObject obj;
	obj.Name = ToTry.string();
	obj.Path = tmp;
	obj.Type = t;
	ObjectInserting* thread = new ObjectInserting(this,&File, obj);
	thread->Run();
	Parent->PastedToArchive();
}
void ArchiveForm::SetList(wxCommandEvent &event)
{
	path PathToTry = PathControl->GetValue();
	if (PathToTry == "$root")
	{
		ListPath ="0";
		ListInit();
	}
	else
	{
		ListPath = PathToTry.string().substr(ROOTLENGTH+1);
		ListInit();
	}
}
void ArchiveForm::UpList(wxCommandEvent& event)
{
	if (ListPath == "0") return;
	if (!ListPath.has_parent_path()) ListPath = "0";
	else ListPath = ListPath.parent_path();
	ListInit();
}
void ArchiveForm::UpdateList(wxCommandEvent& event)
{
	ListInit();
}
void ArchiveForm::HomeList(wxCommandEvent& event)
{
	path old = ListPath;
	ListPath = "0";
	if (old != "0")
	{
		if (FolderHistory.size() > 1) FolderHistory.erase(FolderHistory.begin() + ListIndex + 1, FolderHistory.end());
		FolderHistory.push_back(ListPath);
		ListIndex = FolderHistory.size() - 1;
	}
	ListInit();
}
void ArchiveForm::BackList(wxCommandEvent& event)
{
	if ((FolderHistory.size() > 1) && (ListIndex - 1 >= 0))
	{
		ListIndex--;
		ListPath = FolderHistory[ListIndex];
		ListInit();
	}
}
void ArchiveForm::ForwardList(wxCommandEvent& event)
{
	if ((FolderHistory.size() > 1) && (ListIndex + 1 <= FolderHistory.size() - 1))
	{
		ListIndex++;
		ListPath = FolderHistory[ListIndex];
		ListInit();
	}
}
template<class T>
void ArchiveForm::ErrorOutput(T &exception)
{
	wxMessageBox(exception.what(), "Error!", wxOK);
}
void ArchiveForm::Terminate(wxCloseEvent& event)
{
	SaveAndExit* question = new SaveAndExit(this);
	question->Show();
}
BEGIN_EVENT_TABLE(ArchiveForm,wxFrame)
EVT_LIST_ITEM_ACTIVATED(wxID_FILE1, ArchiveForm::ListObjectOpened)
EVT_LIST_ITEM_SELECTED(wxID_FILE1, ArchiveForm::SelectPath)
EVT_TEXT_ENTER(wxID_FILE4, ArchiveForm::SetList)
END_EVENT_TABLE()