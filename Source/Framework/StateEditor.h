#pragma once
#include "GameState.h"
#include "UIMenu.h"
#include "../Utility/DialogOpenFile.h"
#include "../Utility/DialogSaveFile.h"
#include "../Game/MapLoader.h"
#include <stack>
#include "UIEditorGrid.h"
#include "UIButtonText.h"
#include "UIButtonTexture.h"
#include "UIItemLister.h"
#include "UIButtonDisc.h"
#include "UIButtonRect.h"
#include "UINumUpDownInt.h"
#include "Window.h"
#include "UIEditorList.h"
#include "UIItemListerScroll.h"
#include "UIListPlanet.h"
#include "UIListEntity.h"
#include "UITeamMenu.h"
#include "../System/System.h"

class StateEditor : public GameState
{
public:
	StateEditor()
		:
		menu(StateEditor::MenuCallback),
		btnPX("+", Drawing::GetDraw().GetFont(Drawing::F_MEDIUM)),
		btnMX("-", Drawing::GetDraw().GetFont(Drawing::F_MEDIUM)),
		btnPY("+", Drawing::GetDraw().GetFont(Drawing::F_MEDIUM)),
		btnMY("-", Drawing::GetDraw().GetFont(Drawing::F_MEDIUM)),
		
		btnPOX("+", FONT_MED),
		btnPOY("+", FONT_MED),
		btnMOX("-", FONT_MED),
		btnMOY("-", FONT_MED),

		curMapName("-"),

		listRight(listPlan,listEnt)
	{
		cur = this;

		Event_Resize(Framework::DrawStart(), Framework::DrawWidth());

		menu.Register(*this);

		menu.AddSection("File");
		menu.AddElement("File", "New");
		menu.AddElement("File", "Open...");
		menu.AddElement("File", "Save");
		menu.AddElement("File", "Save As...");

		menu.AddSection("Edit");
		menu.AddElement("Edit", "Undo");
		menu.AddElement("Edit", "Redo");

		menu.AddSection("Tools");
		menu.AddElement("Tools", "Align Midpoint");
		menu.AddElement("Tools", "Align Lines");
		menu.AddElement("Tools", "Adjust Planets to Radius");
		menu.AddElement("Tools", "Mirror X");
		menu.AddElement("Tools", "Mirror Y");
		menu.AddElement("Tools", "Mirror XY");

		menu.AddSection("Teams");
		menu.AddElement("Teams", "Edit Teams");

		AddObject(&grid);
		AddObject(&btnPX);
		AddObject(&btnMX);
		AddObject(&btnPY);
		AddObject(&btnMY);

		AddObject(&btnPOX);
		AddObject(&btnMOX);
		AddObject(&btnPOY);
		AddObject(&btnMOY);

		listRight.Register(*this);
	}
	virtual ~StateEditor()
	{
		cur = nullptr;
	}
	virtual void Event_Resize(const PointF& ul, const PointF& dim) override
	{
		const float Scalar = Framework::GetScalar();

		menu.SetMetrics(dim);
		menu.SetOrigin(ul);

		btnPOY.AdjustToFont();
		btnPOX.AdjustToFont();
		btnMOY.AdjustToFont();
		btnMOX.AdjustToFont();

		grid.SetOrigin({ ul.x + btnPOX.GetMetrics().x, ul.y + menu.GetMetrics().y + btnPOY.GetMetrics().x });
		grid.SetMetrics({ dim.x - 420.0f, (dim.y - menu.GetMetrics().y) * 0.8f });

		// + - buttons
		float height = grid.GetMetrics().y / 2.0f;

		btnPX.AdjustToFont();
		btnPX.SetOrigin(grid.GetRect().TopRight());
		btnPX.SetHeight(height);

		btnPOX.SetOrigin(grid.GetRect().TopLeft() - PointF(btnPOX.GetMetrics().x, 0.0f));
		btnPOX.SetHeight(height);

		btnMX.AdjustToFont();
		btnMX.SetOrigin(grid.GetRect().TopRight() + PointF(0.0f, grid.GetMetrics().y / 2.0f));
		btnMX.SetHeight(height);

		btnMOX.SetOrigin(btnPOX.GetRect().BottomLeft());
		btnMOX.SetHeight(height);

		height = btnPX.GetMetrics().x;
		const float width = grid.GetMetrics().x / 2.0f;

		btnPY.AdjustToFont();
		btnPY.SetOrigin(grid.GetRect().BottomLeft());
		btnPY.SetMetrics({width, height});

		btnPOY.SetOrigin(grid.GetRect().TopLeft() - PointF(0.0f, height));
		btnPOY.SetMetrics({ width, height });

		btnMY.AdjustToFont();
		btnMY.SetOrigin(grid.GetRect().BottomLeft() + PointF(grid.GetMetrics().x / 2.0f, 0.0f));
		btnMY.SetMetrics({ width, height });

		btnMOY.SetOrigin(btnPOY.GetRect().TopRight());
		btnMOY.SetMetrics({ width, height });


		//Planet buttons etc.

		//panel setup
		panelRight.p1 = btnPX.GetRect().TopRight() - PointF(0.0f,height);
		panelRight.p2 = ul + dim;

		panelBottom.p1 = btnPY.GetRect().TopLeft() - PointF(height,0.0f);
		panelBottom.p2 = panelRight.p2 - PointF(panelRight.getWidth(), 0);

		panelTop.p1 = ul;
		panelTop.p2 = btnPX.GetRect().TopRight();


		listRight.SetOrigin(panelRight.TopLeft());
		listRight.SetMetrics({ panelRight.getWidth(), panelRight.getHeight() });

		
		//listRight.OrderItems();
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		//Draw Map
		grid.Draw(draw);
		
		// draw cursor
		if (listRight.GetState() == UIEditorList::state::floating)
		{
			if (listRight.GetSelection() == UIEditorList::selection::planet)
			{
				// draw Planet
				const auto p = listRight.GetPlanet();
				draw.DrawPlanet(Input::GetMouseXY(), Color::GetTeamColor(p.team), grid.GetScalar() * p.radius);
			}
			else if (listRight.GetSelection() == UIEditorList::selection::entity)
			{
				const auto p = listRight.GetSpawn();
				draw.DrawRect(RectF::FromPoint(Input::GetMouseXY(), 15.0f * grid.GetScalar()), Color::GetTeamColor(p.team));
			}
		}

		draw.DrawRect(panelRight, Color::Black());
		draw.DrawRect(panelBottom, Color::Black());
		draw.DrawRect(panelTop, Color::Black());

		btnPX.Draw(draw);
		btnMX.Draw(draw);

		btnPY.Draw(draw);
		btnMY.Draw(draw);

		btnPOX.Draw(draw);
		btnMOX.Draw(draw);

		btnPOY.Draw(draw);
		btnMOY.Draw(draw);

		listRight.Draw(draw);

		menu.Draw(draw);	
		
	}
	virtual void ExecuteCode(float dt) override
	{
		//Button press
		if (btnPX.Pushed() && !menuPush)
			grid.WidthPlus();

		if (btnMX.Pushed() && !menuPush)
			grid.WidthMinus();

		if (btnPY.Pushed() && !menuPush)
			grid.HeightPlus();

		if (btnMY.Pushed() && !menuPush)
			grid.HeightMinus();

		if (btnPOX.Pushed() && !menuPush)
		{
			grid.WidthOtherPlus();
			RefreshSelected();
		}

		if (btnMOX.Pushed() && !menuPush)
		{
			grid.WidthOtherMinus();
			RefreshSelected();
		}

		if (btnPOY.Pushed() && !menuPush)
		{
			grid.HeightOtherPlus();
			RefreshSelected();
		}

		if (btnMOY.Pushed() && !menuPush)
		{
			grid.HeightOtherMinus();
			RefreshSelected();
		}

		if (listRight.Changed())
		{
			if (listRight.GetState() == UIEditorList::state::fixed)
			{
				int id = listRight.GetID();
				if (listRight.GetSelection() == UIEditorList::selection::entity)
				{
					grid.ChangeSpawn(listRight.GetSpawn(), id);
				}
				else if (listRight.GetSelection() == UIEditorList::selection::planet)
				{
					grid.ChangePlanet(listRight.GetPlanet(), id);
				}
			}
		}

		if (menu.isClosed())
		{
			SetNextState(states::Extras);
		}

		if (bTeamChange && !isFinished())
		{
			bTeamChange = false;
			grid.ChangeTeams(Database::GetEditorTeams());
		}

		menuPush = false;
	}
	virtual void Event_MouseMove(const PointF& p)
	{
		listRight.SetGridMouse(grid.MouseToModel());
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& p) override
	{
		if (menu.isActive())
			return;

		switch (k)
		{
		case Input::Left:
			if (listRight.GetState() == UIEditorList::state::floating)
			{
				if (grid.GetRect().PointInside(p))
				{
					// place item
					if (listRight.GetSelection() == UIEditorList::selection::entity)
					{
						grid.AddSpawn(listRight.GetSpawn());
					}
					else if (listRight.GetSelection() == UIEditorList::selection::planet)
					{
						grid.AddPlanet(listRight.GetPlanet());
					}
				}
			}
			else
			{
				if (grid.GetRect().PointInside(p))
				{
					// fixed or none -> select at mouse position
					UIEditorGrid::type ty;
					int id = grid.GetSelected(&ty);
					if (id < 0)
						return;

					if (ty == UIEditorGrid::type::entity)
					{
						auto sp = grid.GetEntByID(id);
						if (sp)
						{
							listRight.SelectSpawn(*sp, id);
						}
					}
					else if (ty == UIEditorGrid::type::planet)
					{
						auto pl = grid.GetPlanByID(id);
						if (pl)
						{
							listRight.SelectPlanet(*pl, id);
						}
					}
				}
			}
			break;
		case Input::Right:
			if (listRight.GetState() == UIEditorList::state::none)
				grid.DeleteAtMouse();
			break;
		case Input::Middle:
			break;
		case Input::ScrollUp:
			break;
		case Input::ScrollDown:
			break;
		}
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		switch (secretKeyCode)
		{
		case 0:
		case 1:
			if (code == SDL_SCANCODE_UP)
				secretKeyCode++;
			else secretKeyCode = 0;
			break;
		case 2:
		case 3:
			if (code == SDL_SCANCODE_DOWN)
				secretKeyCode++;
			else secretKeyCode = 0;
			break;
		case 4:
		case 6:
			if (code == SDL_SCANCODE_LEFT)
				secretKeyCode++;
			else secretKeyCode = 0;
			break;
		case 5:
		case 7:
			if (code == SDL_SCANCODE_RIGHT)
				secretKeyCode++;
			else secretKeyCode = 0;
			break;
		case 8:
			if (code == SDL_SCANCODE_B)
				secretKeyCode++;
			else secretKeyCode = 0;
			break;
		case 9:
			if (code == SDL_SCANCODE_A)
				SetNextState(states::LevelpackEditor);
			else secretKeyCode = 0;
		}
	}
	void OpenTeamsDialog()
	{
		Database::SetEditorTeams(grid.GetCurTeams());
		SetNextState(states::EditorTeams);
		bTeamChange = true;
	}
private: //menu bar needs acces
	static StateEditor* cur;

	void RefreshSelected()
	{
		if (listRight.GetState() == UIEditorList::state::fixed)
		{
			if (listRight.GetSelection() == UIEditorList::selection::entity)
			{
				auto sp = grid.GetEntByID(listRight.GetID());
				
				if (sp)
				{
					listRight.SelectSpawn(*sp, listRight.GetID());
				}
				else
				{
					listRight.Deselect();
				}
			}
			else if (listRight.GetSelection() == UIEditorList::selection::planet)
			{
				auto pl = grid.GetPlanByID(listRight.GetID());

				if (pl)
				{
					listRight.SelectPlanet(*pl, listRight.GetID());
				}
				else
				{
					listRight.Deselect();
				}
			}
		}
	}
public:
	static void MenuCallback(const std::string& section, const std::string& name)
	{
		assert(cur);
		cur->menuPush = true;
		if (section == "File")
		{
			bool bSaveAs = false;
			if (name == "New")
			{
				cur->grid.Reset();
				cur->listRight.Deselect();
				cur->curMapName = "-";
			}
			else if (name == "Open...")
			{
				DialogOpenFile dialog("map");

				dialog.Show();

				if (dialog.IsSuccess())
				{
					auto te = dialog.GetName();

					MapLoader loader(te);
					if (loader.isValid())
					{
						//load map
						cur->listRight.Deselect();
						cur->grid.LoadMap(loader.GetPlanets(), loader.GetSpawns(), loader.GetWidth(), loader.GetHeight(),loader.GetTeams());
						cur->curMapName = te;
					}
					else
					{
						System::MessageBox("Error", "could not open map", System::IconError);
					}
				}
			} 
			else if (name == "Save")
			{
				if (cur->curMapName != "-")
				{

					//Check if it may be saved
					std::string err;
					bool bValid = cur->grid.MapIsValid(&err);
					if(bValid)
					{
						//save
						bValid = cur->grid.SaveMap(std::string(cur->curMapName.begin(), cur->curMapName.end()));
						if (!bValid)
							err = "could not save file";
					}

					if (!bValid)
					{
						System::MessageBox("Error", err.c_str(), System::IconError);
					}
					else
					{
						System::MessageBox("Info",std::string("Map Saved!\n")
							+ std::string(cur->curMapName.begin(), cur->curMapName.end()),
							System::IconError);
						Settings::UnlockAchievement(Settings::A_MapBuilder);
					}
				}
				else
				{
					bSaveAs = true;
				}
			}

			if (name == "Save As..." || bSaveAs)
			{
				//Check if it may be saved
				std::string err;
				bool bValid = cur->grid.MapIsValid(&err);

				if (bValid)
				{
					DialogSaveFile dialog("map");
					
					dialog.Show();

					if (dialog.IsSuccess())
					{
						//get results
						auto te = dialog.GetName();
						// check for ending
						if (te.length() > 4)
						{
							auto len = te.length();
							if (te[len - 1] != 'p' || te[len - 2] != 'a' ||
								te[len - 3] != 'm' || te[len - 4] != '.')
							{
								te += ".map";
							}
						}
						else
						{
							te += ".map";
						}

						//try saving
						std::string err;
						bool bValid = cur->grid.MapIsValid(&err);
						if (bValid)
						{
							//save
							bValid = cur->grid.SaveMap(te);
							if (!bValid)
								err = "could not save file";
							else
								cur->curMapName = te;
						}

						if (!bValid)
						{
							System::MessageBox("Error", err.c_str(), System::IconError);
						}
						else
						{
							Settings::UnlockAchievement(Settings::A_MapBuilder);
						}
					}
				}
				else
				{
					System::MessageBox("Error", err.c_str(), System::IconError);
				}
			}
			
		} else if (section == "Edit")
		{
			if (name == "Undo")
			{
				cur->grid.Undo();
			}
			else if (name == "Redo")
			{
				cur->grid.Redo();
			}
			cur->listRight.Deselect();
		}
		else if (section == "Tools")
		{
			if (name == "Align Lines")
			{
				cur->grid.AlignLine();
			}else if (name == "Align Midpoint")
			{
				cur->grid.AlignMid();
			}
			else if (name == "Adjust Planets to Radius")
			{
				cur->grid.AdjustPlans();
			}
			else if (name == "Mirror X")
			{
				cur->grid.MirrorX();
			}
			else if (name == "Mirror Y")
			{
				cur->grid.MirrorY();
			}
			else if (name == "Mirror XY")
			{
				cur->grid.MirrorX();
				cur->grid.MirrorY();
			}
			cur->RefreshSelected();
		}
		else if (section == "Teams")
		{
			if (name == "Edit Teams")
			{
				cur->OpenTeamsDialog();
			}
		}
	}
	
private:
	UIMenu menu;

	UIEditorGrid grid;

	UIButtonText btnPX,
		btnMX,
		btnPY,
		btnMY,

		btnPOX,
		btnMOX,
		btnPOY,
		btnMOY;

	RectF panelRight; //without + - button
	RectF panelBottom; //with + - button
	RectF panelTop;

	std::string curMapName;

	UIListPlanet listPlan;
	UIListEntity listEnt;

	UIEditorList listRight;

	bool menuPush = false;
	bool bTeamChange = false;

	int secretKeyCode = 0;
};