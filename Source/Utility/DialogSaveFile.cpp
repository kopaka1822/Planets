#include "DialogSaveFile.h"
#include "../filedialog/include/nfd.h"

DialogSaveFile::DialogSaveFile(std::string pattern)
	:
	patt(pattern)
{

}
DialogSaveFile::~DialogSaveFile()
{

}
void DialogSaveFile::Show()
{
	nfdchar_t *savePath = nullptr;
	nfdresult_t nres = NFD_SaveDialog(patt.c_str(), nullptr, &savePath);
	if (nres == NFD_OKAY)
	{
		bSucces = true;
		result = (char*)savePath;
		free(savePath);
		savePath = nullptr;
	}
}
bool DialogSaveFile::IsSuccess() const
{
	return bSucces;
}
std::string DialogSaveFile::GetName() const
{
	return result;
}