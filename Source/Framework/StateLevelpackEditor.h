#pragma once
#include "GameState.h"
#include "UILevelPackList.h"
#include "UIButtonText.h"
#include "../Utility/DialogSaveFile.h"
#include "../Utility/FileReader.h"
#include "../Utility/FileWriter.h"

class StateLevelPackEditor : public GameState
{
public:
	StateLevelPackEditor()
		:
		btnBack("Back", FONT_MED),
		lblTitle("Levelpack Editor",FONT_BIG),
		btnCreate("Create Pack",FONT_SMALL),
		boxStatus("",FONT_SMALL),
		btnLoadConfig("Load Config",FONT_SMALL),
		btnSaveConfig("Save Config",FONT_SMALL)
	{
		AddObject(&btnBack);
		AddObject(&btnCreate);
		AddObject(&btnLoadConfig);
		AddObject(&btnSaveConfig);

		list.registerMe(*this);
		list.setMetrics({ 900, 540 });
		list.centerX(150.0f);
		// now the trick lol
		list.setMetrics({ 700, 540 });

		list.OrderItems();

		btnBack.setOrigin({ 10, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });
		lblTitle.centerX(50.0f);
		
		float xsta = 920.0f;
		btnCreate.setOrigin(PointF(xsta, 150.0f));
		btnSaveConfig.setOrigin(PointF(xsta, 220.0f));
		btnLoadConfig.setOrigin(PointF(xsta, 290.0f));
		boxStatus.setOrigin(PointF(xsta, 360.0f));
		//boxStatus.setWidth(300.0f);
		boxStatus.AdjustToFont(Framework::STD_DRAW_X - xsta);
	}
	virtual ~StateLevelPackEditor()
	{

	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
			SetNextState(states::Editor);

		if (btnCreate.Pushed())
			CreatePack();

		if (btnSaveConfig.Pushed())
			SaveConfig();

		if (btnLoadConfig.Pushed())
			LoadConfig();
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		list.draw(draw);
		btnCreate.draw(draw);
		btnSaveConfig.draw(draw);
		btnLoadConfig.draw(draw);
		boxStatus.draw(draw);
		btnBack.draw(draw);
	}
private:
	void CreatePack()
	{
		DialogSaveFile dia("pack");
		dia.Show();

		if (dia.IsSuccess())
		{
			std::string path = dia.GetName();
			path = tool::fileAddExtension(path, "pack");

			// try to create levelpack
			try
			{
				Levelpack::Save(path, list.GetMapVector(), list.GetMapTimes());
			}
			catch (const GameError& e)
			{
				SetWarning(e.what());
				return;
			}
			SetInfo("saved succesfully");
			return;
		}
		SetWarning("canceled");
		return;
	}
	void SaveConfig()
	{
		DialogSaveFile dia("pconf");
		dia.Show();
		if (dia.IsSuccess())
		{
			std::string path = dia.GetName();
			path = tool::fileAddExtension(path, "pconf");

			FileWriter w(path);

			if (w.isOpen())
			{
				// Header
				w.writeChar('C');
				w.writeChar('F');
				w.writeChar('G');

				w.writeInt(1); // Version == 1

				// data
				for (const auto& f : list.GetMapTimes())
				{
					w.writeFloat(f);
				}
				for (const auto& s : list.GetMapVector())
				{
					w.writeString(s);
				}

				w.writeChar('E');
				w.writeChar('N');
				w.writeChar('D');

				SetInfo("saved");
			}
			else
			{
				SetWarning("could not create file");
			}
		}
	}
	void LoadConfig()
	{
		DialogOpenFile dia("pconf");
		dia.Show();
		if (dia.IsSuccess())
		{
			FileReader r(dia.GetName());

			if (r.isOpen())
			{
				try
				{
					char c1 = r.readChar();
					char c2 = r.readChar();
					char c3 = r.readChar();

					if (c1 != 'C' || c2 != 'F' || c3 != 'G')
						throw GameError("invalid header");

					int version = r.readInt();
					if (version != 1)
						throw GameError("invalid version");

					// read time
					std::vector< float > times;
					for (int i = 0; i < 12; i++)
						times.push_back(r.readFloat());

					std::vector< std::string > names;
					for (int i = 0; i < 12; i++)
						names.push_back(r.readString());

					// check ending
					c1 = r.readChar();
					c2 = r.readChar();
					c3 = r.readChar();

					if (c1 != 'E' || c2 != 'N' || c3 != 'D')
						throw GameError("invalid file end");


					// load into list
					list.LoadConfig(names, times);
					
					SetInfo("loaded");
				}
				catch (const std::out_of_range&)
				{
					SetWarning("out of range exception cought while reading config");
				}
				catch (const GameError& e)
				{
					SetWarning(e.what());
				}
			}
			else
			{
				SetWarning("could not open file");
			}
		}
	}
	void SetWarning(const std::string& w)
	{
		boxStatus.SetColor(Color::Red());
		boxStatus.SetText(w);
	}
	void SetInfo(const std::string& s)
	{
		boxStatus.SetColor(Color::White());
		boxStatus.SetText(s);
	}
private:
	UIButtonText btnBack;
	UIButtonText btnCreate;
	UIButtonText btnLoadConfig;
	UIButtonText btnSaveConfig;
	UILabel lblTitle;

	UITextBox boxStatus;
	UILevelPackList list;
};