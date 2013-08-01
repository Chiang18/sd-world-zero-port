#include "WpfInterface.h"
#include "Wpf.h"

#include <assert.h>

int main()
{
	
	CWpfInterface* pkWpfSys = new CWpf;
	assert(pkWpfSys);

	pkWpfSys->WpfOpen("E:\\SDNA\\World_Zero0925\\gdata\\world0\\misc.wpf", 0, EWOT_LISTDIR | EWOT_READONLY);
	pkWpfSys->ExportDir("", "E:\\project_game\\edata\\world1_1");
	pkWpfSys->WpfClose();

	pkWpfSys->WpfOpen("E:\\SDNA\\World_Zero0925\\gdata\\world0\\model.wpf", 0, EWOT_LISTDIR | EWOT_READONLY);
	pkWpfSys->ExportDir("", "E:\\project_game\\edata\\world1_1");
	pkWpfSys->WpfClose();

	pkWpfSys->WpfOpen("E:\\SDNA\\World_Zero0925\\gdata\\world0\\scene.wpf", 0, EWOT_LISTDIR | EWOT_READONLY);
	pkWpfSys->ExportDir("", "E:\\project_game\\edata\\world1_1");
	pkWpfSys->WpfClose();	

	pkWpfSys->WpfOpen("E:\\SDNA\\World_Zero0925\\gdata\\world0\\texture1.wpf", 0, EWOT_LISTDIR | EWOT_READONLY);
	pkWpfSys->ExportDir("", "E:\\project_game\\edata\\world1_1");
	pkWpfSys->WpfClose();

	pkWpfSys->WpfOpen("E:\\SDNA\\World_Zero0925\\gdata\\world0\\texture2.wpf", 0, EWOT_LISTDIR | EWOT_READONLY);
	pkWpfSys->ExportDir("", "E:\\project_game\\edata\\world1_1");
	pkWpfSys->WpfClose();
}