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
	virtual void setMetrics(const PointF& m) override
	{
		UIObject::setMetrics(m);
		btnOkay.AdjustToFont();
		btnCancel.AdjustToFont();

		list.setMetrics(PointF(m_dim.x - 2 * (border + padding), m_dim.y - 2 * border - 3 * padding - btnOkay.getMetrics().y));
	}
	// second!
	virtual void setOrigin(const PointF& p) override
	{
		UIObject::setOrigin(p);
		list.setOrigin(m_pos + PointF(border + padding, border + padding));
		btnOkay.setOrigin(list.getRect().BottomLeft() + PointF(0.0f, padding));
		btnCancel.setOrigin(btnOkay.getRect().TopRight() + PointF(padding, 0.0f));

		list.OrderItems();
	}
	virtual void draw(Drawing& draw) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		list.draw(draw);
		btnOkay.draw(draw);
		btnCancel.draw(draw);
	}
	virtual ~UITeamMenu()
	{

	}
	virtual void registerMe(GameState& gs) override
	{
		UIObject::registerMe(gs);
		btnOkay.registerMe(gs);
		btnCancel.registerMe(gs);
		list.registerMe(gs);
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
			if (!getRect().PointInside(pos))
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
