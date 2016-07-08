#pragma once
#include "UIObject.h"
#include "UIItemListerScroll.h"
#include "UILabel.h"
#include "UIButtonText.h"
#include "UINumUpDownInt.h"

class UITeamMenu : public UIObject
{
public:
	UITeamMenu(const std::vector< byte >& teams)
		:
		btnOkay("Okay",FONT_SMALL),
		btnCancel("Cancel",FONT_SMALL),
		list(false)
	{
		lbls.push_back(std::unique_ptr<UILabel>(new UILabel("Player:", FONT_SMALL)));
		lbls.push_back(std::unique_ptr<UILabel>(new UILabel("Team:", FONT_SMALL)));
		list.AddItem(lbls[0].get(), lbls[1].get());
		for (size_t i = 0; i < teams.size(); i++)
		{
			auto lbl = std::unique_ptr<UILabel>(new UILabel(std::string("Player ") + std::to_string(i + 1) + std::string(":"), FONT_SMALL));
			auto num = std::unique_ptr<UINumUpDownInt>(new UINumUpDownInt(teams[i], 1, 25, 1, FONT_SMALL));

			lbl->AdjustToFont();
			lbl->SetColor(Color::GetTeamColor(i + 1));
			num->AdjustToFont(150.0f);

			lbls.push_back(std::move(lbl));
			numTeams.push_back(std::move(num));

			list.AddItem(lbls[i + 2].get(), numTeams[i].get());
		}
	}
	// call this first!!
	virtual void SetMetrics(const PointF& m) override
	{
		UIObject::SetMetrics(m);
		btnOkay.AdjustToFont();
		btnCancel.AdjustToFont();

		list.SetMetrics(PointF(dim.x - 2 * (border + padding), dim.y - 2 * border - 3 * padding - btnOkay.GetMetrics().y));
	}
	// second!
	virtual void SetOrigin(const PointF& p) override
	{
		UIObject::SetOrigin(p);
		list.SetOrigin(pos + PointF(border + padding, border + padding));
		btnOkay.SetOrigin(list.GetRect().BottomLeft() + PointF(0.0f, padding));
		btnCancel.SetOrigin(btnOkay.GetRect().TopRight() + PointF(padding, 0.0f));

		list.OrderItems();
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

		list.Draw(draw);
		btnOkay.Draw(draw);
		btnCancel.Draw(draw);
	}
	virtual ~UITeamMenu()
	{

	}
	virtual void Register(GameState& gs) override
	{
		UIObject::Register(gs);
		btnOkay.Register(gs);
		btnCancel.Register(gs);
		list.Register(gs);
	}
	bool OkayPressed()
	{
		return btnOkay.Pushed();
	}
	bool Cancel()
	{
		return btnCancel.Pushed() || bCancel;
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos)
	{
		if (k == Input::Left)
		{
			if (!GetRect().PointInside(pos))
				bCancel = true;
		}
	}
	std::vector< byte >	GetTeams() const
	{
		std::vector< byte > t;
		for (const auto& n : numTeams)
		{
			t.push_back(n->GetValue());
		}
		return t;
	}
private:
	UIButtonText btnOkay;
	UIButtonText btnCancel;
	UIItemListerScroll list;
	bool bCancel = false;

	std::vector< std::unique_ptr< UILabel >> lbls;
	std::vector< std::unique_ptr< UINumUpDownInt >> numTeams;

	static const int border = 6;
	static const int padding = 4;
};
