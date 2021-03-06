#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../Game_Interface/game_content_interface.h"

//model
namespace Game_Model
{
	class RewardData
	{
	public:

	private:
		static RewardData* getInstance();
	};

};
typedef Game_Model::RewardData REWARD_MODEL;


//view

namespace Game_View
{
	class RewardLayer : public cocos2d::Layer
	{
	public:
		void									onCloseCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
		virtual void							onEnter() override;
		virtual void							onExit()  override;
		virtual bool							init()    override;
		CREATE_FUNC(RewardLayer);
	private:
		cocos2d::ui::Widget*					_rootWidget;
		cocos2d::ui::Button*					_closeButton;

	};

};
typedef Game_View::RewardLayer REWARD_VIEW;

namespace Game_Net
{
	static bool send_on_load(Game_Logic::Game_Interface& gm_interface);

}