#pragma once
#include <string>

class DialogSaveFile
{
public:
	// "png,jpg;pdf"
	DialogSaveFile(std::string pattern);
	~DialogSaveFile();
	void Show();
	bool IsSuccess() const;
	std::string GetName() const;
private:
	bool bSucces = false;
	std::string result;
	std::string patt;
};