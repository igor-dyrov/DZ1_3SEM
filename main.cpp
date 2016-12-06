#ifndef WX_PRECOMP
#include "GUI.h"
#endif

class MyApp : public wxApp
{
public:
	bool OnInit()
	{
		//path a =  "F:\\2.arc";
		//ArchiveForm* GUI = new ArchiveForm(nullptr,a,a.string());
		Form* GUI = new Form;
		//PasswordInput * GUI = new PasswordInput();
		GUI->Show();

		return true;
	}
};

IMPLEMENT_APP(MyApp);

