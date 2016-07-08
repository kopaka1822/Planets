#pragma once
#include "UIItemListerScroll.h"
#include <memory>
#include "UILabel.h"
#include "UINumUpDownInt.h"
#include "UIButtonFile.h"

class UILevelPackList : public UIItemListerScroll
{
	class Element
	{
	public:
		Element()
		{}
		Element(Element&& o)
		{
			Swap(o);
		}
		Element& operator=(Element&& o)
		{
			Swap(o);
		}
	public:
		std::unique_ptr< UILabel > pHeadline;
		std::unique_ptr< UILabel > pLblFile;
		std::unique_ptr< UILabel > pLblTime;
		std::unique_ptr< UINumUpDownInt > pNumTime;
		std::unique_ptr< UIButtonFile > pBtnFile;

	private:
		void Swap(Element& o)
		{
			std::swap(pHeadline, o.pHeadline);
			std::swap(pLblFile, o.pLblFile);
			std::swap(pLblTime, o.pLblTime);
			std::swap(pNumTime, o.pNumTime);
			std::swap(pBtnFile, o.pBtnFile);
		}
	};
public:
	UILevelPackList()
		:
		UIItemListerScroll(false)
	{
		for (int i = 0; i < 12; i++)
			AddElement(i + 1);

		// create list
		for (auto& e : elms)
		{
			this->AddItem(e.pHeadline.get(), nullptr);
			this->AddItem(e.pLblFile.get(), e.pBtnFile.get());
			this->AddItem(e.pLblTime.get(), e.pNumTime.get());
		}
	}
	virtual ~UILevelPackList()
	{

	}
	void LoadConfig(const std::vector< std::string >& names, const std::vector< float >& times)
	{
		assert(names.size() == 12);
		assert(times.size() == 12);
		assert(elms.size() == 12);

		for (int i = 0; i < 12; i++)
		{
			elms[i].pBtnFile->SetPath(names[i]);
			elms[i].pNumTime->SetValue((int)times[i]);
		}
	}
	std::vector< std::string > GetMapVector() const
	{
		std::vector< std::string > res;
		for (const auto& e : elms)
		{
			res.push_back(e.pBtnFile->GetPath());
		}
		return res;
	}
	std::vector< float > GetMapTimes() const
	{
		std::vector< float > res;
		for (const auto& e : elms)
		{
			res.push_back((float)e.pNumTime->GetValue());
		}
		return res;
	}
private:
	void AddElement(int index)
	{
		std::string name = "Level ";
		if (index < 10)
			name += "0";
		name += std::to_string(index);

		Element e;
		e.pHeadline = std::unique_ptr< UILabel >(new UILabel(name, FONT_MED));
		e.pLblFile = std::unique_ptr< UILabel >(new UILabel("File:", FONT_SMALL));
		e.pLblTime = std::unique_ptr< UILabel >(new UILabel("Time in s:", FONT_SMALL));
		e.pNumTime = std::unique_ptr< UINumUpDownInt >(
			new UINumUpDownInt(120, 1, 3600, 10, FONT_SMALL));
		e.pBtnFile = std::unique_ptr< UIButtonFile >(
			new UIButtonFile(FONT_SMALL));

		e.pNumTime->AdjustToFont(300.0f);
		e.pBtnFile->AdjustToFont(300.0f);

		elms.push_back(std::move(e));
	}

private:
	// buttons
	std::vector< Element > elms;
};