#include "Pet_Client.h"


#include "ui/CocosGUI.h"
#include "json/document.h"

#include "../Network_Common/NetManager.h"
#include "../Network_Common/message.h"

#include "../Player_Account/Account_Data_Mgr.h"
#include "../Character_System/Character_Mgr.h"
#include "../Character_System/Player.h"
#include "ClientLogic/Utils/BaseUtils.h"
#include "UI/City_Scene.h"

using namespace cocostudio;

void Game_Model::PetData::load()
{
	_petConfigMap.clear();
	_petAttrMap.clear();
    m_map_get_pet.clear();

	rapidjson::Document doc;
	rapidjson::Document doc1;
    rapidjson::Document doc2;
	auto str = FileUtils::getInstance()->getStringFromFile("game_data/guard_beast_system/guard_beast_data.json");
	auto str1 = FileUtils::getInstance()->getStringFromFile("game_data/guard_beast_system/guard_beast_att.json");
    auto str2 = FileUtils::getInstance()->getStringFromFile("game_data/guard_beast_system/get_guard_beast.json");
	
	doc.Parse<0>(str.c_str());
	if (doc.IsArray())
	{
		for (int i = 0; i < doc.Size(); i++)
		{
			PetConfig* config = new PetConfig;

			const rapidjson::Value& v = doc[i];
			int id = cocostudio::DICTOOL->getIntValue_json(doc[i], "id");
			int star = cocostudio::DICTOOL->getIntValue_json(doc[i], "star");
			int quality = cocostudio::DICTOOL->getIntValue_json(doc[i], "Quality");
			int consume = cocostudio::DICTOOL->getIntValue_json(doc[i], "expend_gold");
			std::string name = cocostudio::DICTOOL->getStringValue_json(doc[i], "name");
			int attr = cocostudio::DICTOOL->getIntValue_json(doc[i], "att_id");
            std::string armName = cocostudio::DICTOOL->getStringValue_json(doc[i], "armature");
            std::string iconName = cocostudio::DICTOOL->getStringValue_json(doc[i], "icon");
            int quality_up_id = DICTOOL->getIntValue_json(doc[i], "Quality_up_id");
            int star_up_id = DICTOOL->getIntValue_json(doc[i], "star_up_id");
            int variation_id = DICTOOL->getIntValue_json(doc[i], "variation_id");
			config->attrId = attr;
			config->id = id;
			config->star = star;
			config->quality = quality;
			config->consume = consume;
			config->name = name;
            config->armName = armName;
            config->iconName = iconName;
            config->quality_up_id = quality_up_id;
            config->star_up_id = star_up_id;
            config->variation_id = variation_id;

			_petConfigMap.insert( std::make_pair(id, config) );
            
            ArmatureDataManager::getInstance()->addArmatureFileInfo("armature/"+armName+".ExportJson");
		}

	}

	doc1.Parse<0>(str1.c_str());
	if (doc1.IsArray())
	{
		for (int i = 0; i < doc1.Size(); i++)
		{
			PetAttr* attr = new PetAttr;

			const rapidjson::Value& v = doc1[i];
			int id = cocostudio::DICTOOL->getIntValue_json(doc1[i], "id");
			int health = cocostudio::DICTOOL->getIntValue_json(doc1[i], "health");
			int magic = cocostudio::DICTOOL->getIntValue_json(doc1[i], "magic");
			int attack = cocostudio::DICTOOL->getIntValue_json(doc1[i], "attack");
			int defense = cocostudio::DICTOOL->getIntValue_json(doc1[i], "defense"); 
			int hit = cocostudio::DICTOOL->getIntValue_json(doc1[i], "hit");
			int dodge = cocostudio::DICTOOL->getIntValue_json(doc1[i], "dodge");
			int crit = cocostudio::DICTOOL->getIntValue_json(doc1[i], "crit");
			int tenacity = cocostudio::DICTOOL->getIntValue_json(doc1[i], "tenacity");
			attr->id = id;
			attr->health = health;
			attr->magic = magic;
			attr->attack = attack;
			attr->defense = defense;
			attr->hit = hit;
			attr->dodge = dodge;
			attr->crit = crit;
			attr->tenacity = tenacity;

			_petAttrMap.insert(std::make_pair(id, attr));

		}

	}
    doc2.Parse<0>(str2.c_str());
    if (doc2.IsArray())
    {
        for (int i = 0; i < doc2.Size(); i++)
        {
            GET_PET_CFG cfg;
            const rapidjson::Value& v = doc2[i];
            cfg.petId = DICTOOL->getIntValue_json(v, "id");
            cfg.levelLimit = DICTOOL->getIntValue_json(v, "level_limit");
            m_map_get_pet[cfg.petId] = cfg;
        }
        
    }
}

void PET_MODEL::eventChangeData()
{
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_REFRESH, nullptr);
}

void PET_VIEW::refresh()
{
    waitLayer->setVisible(false);
    _upgradeButton->setTouchEnabled(true);
	_upgradeButton->setBright(true); 

	loadScrollView();

	int petId = PET_MODEL::getInstance()->getSelectedPetId();
	if (petId > 0)
	{
		auto pet = PET_MODEL::getInstance()->_petConfigMap[petId];
		_petName->setString(pet->name);
		Value consume(pet->consume);
		_petConsume->setString(consume.asString());

		int attrId = pet->attrId;
		auto attr = PET_MODEL::getInstance()->_petAttrMap[attrId];

		float petPower = 0;
		int health = attr->health;
		int magic = attr->magic;
		int attack = attr->attack;
		int defense = attr->defense;
		int hit = attr->hit /*- HIT_SHOW_OFFSET*/;
		int dodge = attr->dodge;
		int crit = attr->crit /*- CRIT_SHOW_OFFSET*/;
		int tenacity = attr->tenacity;
		petPower = (0.1f*health) + (0.08f*magic) + (1.2f*attack) + (1.5f*defense) + (0.6f*hit) + (0.6f*dodge) + (0.6f*crit) + (0.6f*tenacity);
		Value petPowerV(petPower);
		_petPower->setString( petPowerV.asString() );

		int role_id = Account_Data_Mgr::instance()->get_current_role_id();
		auto player = CHARACTER_MGR::instance()->get_character(role_id);
		auto mePower = player->get_fighting_capacity();
		Value mePowerV(mePower);
		_mePower->setString( mePowerV.asString() );

        int q = 0;
        q = MIN(pet->quality, QUALITY_NUM);
        q = MAX(0, q);
        _quality->setString(qualityStr[q]);
        _quality->setVisible(true);
        
        for (int i = 0; i < STAR_NUM; i++) {
            _stars[i]->setVisible(false);
        }
        int s = pet->star;
        for (int i = 0; i<s; i++) {
            if (i>=STAR_NUM) {
                break;
            }
            _stars[i]->setVisible(true);
        }
        
        model->init(pet->armName);
        model->getAnimation()->playWithIndex(0);
        model->setVisible(true);
        
        auto att=PET_MODEL::getInstance()->_petAttrMap[pet->attrId];
        hpTxt->setString(Convert2String(att->health));
        mpTxt->setString(Convert2String(att->magic));
        atkTxt->setString(Convert2String(att->attack));
        defTxt->setString(Convert2String(att->defense));
        critTxt->setString(Convert2String(att->crit));
        tenacTxt->setString(Convert2String(att->tenacity));
        hitRateTxt->setString(Convert2String(att->hit));
        dodgeTxt->setString(Convert2String(att->dodge));

        detailButton->setTouchEnabled(true);
        detailButton->setBright(true);
        
        if (PET_MODEL::getInstance()->getCurrentPetId() == petId) {
            _upgradeButton->setTouchEnabled(false);
            _upgradeButton->setBright(false);
            _goButton->setTouchEnabled(false);
            _goButton->setBright(false);
            _goButton->setVisible(false);
            fightFlag->setVisible(true);
        }else{
            bool canUpgrade = true;
            int role_id = Account_Data_Mgr::instance()->get_current_role_id();
            auto player = dynamic_cast<Game_Data::Player*>(CHARACTER_MGR::instance()->get_character(role_id));
            if(player->get_gold() < pet->consume)
            {
                canUpgrade = false;
            }
            
            if ((pet->variation_id == pet->id) && (pet->star_up_id == pet->id) && (pet->quality_up_id == pet->id)) {
                canUpgrade = false;
            }
            _upgradeButton->setTouchEnabled(canUpgrade);
            _upgradeButton->setBright(canUpgrade);
            _goButton->setTouchEnabled(true);
            _goButton->setBright(true);
            _goButton->setVisible(true);
            fightFlag->setVisible(false);
        }
	}

}

void PET_VIEW::loadScrollView()
{

	if (_rootWidget == nullptr)return;
	_petsScrollView = dynamic_cast<cocos2d::ui::ScrollView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Scr_Pets"));
	_petsScrollView->removeAllChildren();

	auto cell = dynamic_cast<cocos2d::ui::Layout*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Pnl_Cell"));
	auto pets = PET_MODEL::getInstance()->getPets();
	int current_pet_id = PET_MODEL::getInstance()->getCurrentPetId();
    int sel_pet_id = PET_MODEL::getInstance()->getSelectedPetId();

	std::vector<Game_Model::Pet>::iterator it = pets->begin();
	int index = 0;
	for (; it != pets->end();it++)
	{
		auto cellClone = cell->clone();
		auto ready = (cocos2d::ui::ImageView*)( cellClone->getChildByName("Img_Ready") );
		ready->setVisible(false);
        if ((*it).id == current_pet_id)
        {
            ready->setVisible(true);
        }

		cellClone->setPosition(Vec2(index*110,0) );
        cellClone->addTouchEventListener( CC_CALLBACK_2(PET_VIEW::onClickCellCallback, this) );
		_petsScrollView->addChild(cellClone, index, (*it).id);
        
        auto pet = PET_MODEL::getInstance()->_petConfigMap[(*it).id];
        auto icon = (cocos2d::ui::ImageView*)(cellClone->getChildByName("Img_Pet"));
        icon->loadTexture("img/headIcon/"+pet->iconName +".png");
        
        cellClone->getChildByName("Img_Cursor")->setVisible(false);
        if ((*it).id == sel_pet_id) {
            cellClone->getChildByName("Img_Cursor")->setVisible(true);
        }else{
            
        }
        
		index++;

	}

}

bool PET_VIEW::init()
{
	if( !Layer::init() )
        return false;

	Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_REFRESH, CC_CALLBACK_0(PET_VIEW::refresh, this));


	_rootWidget = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui/pet.json");
	if( _rootWidget == nullptr )
        return false;
	addChild(_rootWidget);

	_closeButton  = dynamic_cast<cocos2d::ui::Button*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Btn_Close") );
	_closeButton->setVisible(true);
    _closeButton->setTouchEnabled(true);
	_closeButton->addTouchEventListener( CC_CALLBACK_2(PET_VIEW::onCloseCallback, this) );
	
	_upgradeButton = dynamic_cast<cocos2d::ui::Button*>( cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Btn_Upgrade") );
	_upgradeButton->setVisible(true);/*_upgradeButton->setTouchEnabled(true);*/
	_upgradeButton->addTouchEventListener( CC_CALLBACK_2(PET_VIEW::onUpgradeCallback, this) );

    //出战按钮
	_goButton = dynamic_cast<cocos2d::ui::Button*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Button_Go"));
	_goButton->setVisible(true); _goButton->setTouchEnabled(true);
	_goButton->addTouchEventListener( CC_CALLBACK_2(PET_VIEW::onGoCallback, this) );

    detailButton = dynamic_cast<cocos2d::ui::Button*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Button_114"));
    detailButton->setTouchEnabled(true);
    showDetail = false;
    detailButton->addTouchEventListener(CC_CALLBACK_2(PET_VIEW::onDetail, this));
    hpTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "shenmingshuxing shuzhi"));
    mpTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "mofashuxing shuzhi"));
    atkTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "gongjishuxing shuzhi"));
    defTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "fanyushuxing shuzhi_5"));
    critTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "baihushuxing shuzhi"));
    tenacTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "renxingshuxing shuzhi"));
    hitRateTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "mingzhongshuxing shuzhi"));
    dodgeTxt = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "shangbishuxing shuzhi"));
    
    detailBg = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "shuxingjiacheng"));
    
    //守护兽加战斗力
	_petPower = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Label_95_0"));
    //主角原本战斗力
	_mePower = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Label_95_0_4_5"));

    _quality = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Txt_Quality_0_0"));
    
    auto panel = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Image_80"));
    _stars[0] = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Image_85_0"));
    _stars[0]->setVisible(false);
    for (int i = 1; i < STAR_NUM; i++) {
        _stars[i] = dynamic_cast<cocos2d::ui::ImageView*>(_stars[0]->clone());
        panel->addChild(_stars[i]);
        _stars[i]->setPositionX(_stars[0]->getPositionX() + i * 60);
        _stars[i]->setPositionY(_stars[0]->getPositionY());
        _stars[i]->setVisible(false);
    }
    
    fightFlag = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Image_101"));
	_petName = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Label_81"));
	_petConsume = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Label_120_1"));
    int role_id = Account_Data_Mgr::instance()->get_current_role_id();
    auto player = CHARACTER_MGR::instance()->get_character(role_id);
    auto mePower = player->get_fighting_capacity();
    Value mePowerV(mePower);
    _mePower->setString(mePowerV.asString());
    
    auto showPanel = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Image_71"));
    ArmatureDataManager::getInstance()->addArmatureFileInfo("armature/pet_horse_3.ExportJson");
    model= Armature::create("pet_horse_3");
//    model->getAnimation()->playWithIndex(0);
    model->setPositionX(showPanel->getContentSize().width/2 );
    model->setPositionY(showPanel->getContentSize().height/4+40);
    model->setScale(1.5);
    model->setVisible(false);
    
    detailButton->setTouchEnabled(false);
    detailButton->setBright(false);
    
    showPanel->addChild(model);
    
    
	int petId = PET_MODEL::getInstance()->getCurrentPetId();
	if (petId > 0)
	{
        emptyPet = false;
		auto pet = PET_MODEL::getInstance()->_petConfigMap[petId];
		_petName->setString(pet->name);
		Value consume(pet->consume);
		_petConsume->setString(consume.asString());


		int attrId = pet->attrId;
		auto attr = PET_MODEL::getInstance()->_petAttrMap[attrId];

		float petPower = 0;
		int health = attr->health;
		int magic = attr->magic;
		int attack = attr->attack;
		int defense = attr->defense;
		int hit = attr->hit/* - HIT_SHOW_OFFSET*/;
		int dodge = attr->dodge;
		int crit = attr->crit/* - CRIT_SHOW_OFFSET*/;
		int tenacity = attr->tenacity;
		petPower = (0.1f*health) + (0.08f*magic) + (1.2f*attack) + (1.5f*defense) + (0.6f*hit) + (0.6f*dodge) + (0.6f*crit) + (0.6f*tenacity);
		Value petPowerV(petPower);
		_petPower->setString(petPowerV.asString());


        int q = 0;
        q = MIN(pet->quality, QUALITY_NUM);
        q = MAX(0, q);
        _quality->setString(qualityStr[q]);
        _quality->setVisible(true);

        int s = pet->star;
        for (int i = 0; i<s; i++) {
            if (i>=STAR_NUM) {
                break;
            }
            _stars[i]->setVisible(true);
        }
        
        model->init(pet->armName);
//        model= Armature::create(pet->armName);
        model->getAnimation()->playWithIndex(0);
//        model->setPositionX(showPanel->getPositionX() + showPanel->getContentSize().width/2 );
//        model->setPositionY(showPanel->getPositionY() + showPanel->getContentSize().height/5);
        model->setVisible(true);
        

        auto att=PET_MODEL::getInstance()->_petAttrMap[pet->attrId];
        hpTxt->setString(Convert2String(att->health));
        mpTxt->setString(Convert2String(att->magic));
        atkTxt->setString(Convert2String(att->attack));
        defTxt->setString(Convert2String(att->defense));
        critTxt->setString(Convert2String(att->crit));
        tenacTxt->setString(Convert2String(att->tenacity));
        hitRateTxt->setString(Convert2String(att->hit));
        dodgeTxt->setString(Convert2String(att->dodge));

        detailButton->setTouchEnabled(true);
        detailButton->setBright(true);
        
        PET_MODEL::getInstance()->setSelectedPetId(petId);
        _upgradeButton->setTouchEnabled(false);
        _upgradeButton->setBright(false);
        _goButton->setTouchEnabled(false);
        _goButton->setBright(false);
        _goButton->setVisible(false);
        fightFlag->setVisible(true);
//    }else if() {
    }else{
        emptyPet = true;
        auto label = dynamic_cast<cocos2d::ui::Text*>(cocos2d::ui::Helper::seekWidgetByName(_rootWidget, "Label_120_1_2"));
        label->setString("角色升到2级可获得守护兽");
        model->setVisible(false);
        _petName->setVisible(false);
        _petConsume->setString("0");
        _petPower->setString("0");
        _quality->setVisible(false);
        _upgradeButton->setVisible(false);
        _goButton->setVisible(false);
        detailButton->setTouchEnabled(false);
        detailButton->setBright(false);
        fightFlag->setVisible(false);
    }

	loadScrollView();
    
    
    waitLayer = LayerColor::create(Color4B(166,166,166,100));
    auto to = Sequence::createWithTwoActions(ProgressTo::create(2, 100), ProgressTo::create(0,0));
    auto pt = ProgressTimer::create(Sprite::create("img/battle/skillLinkBg.png"));
    pt->setType(ProgressTimer::Type::RADIAL);
    pt->setPosition(Vec2(YNBaseUtil::WinSize.width/2, YNBaseUtil::WinSize.height/2));
    pt->runAction(RepeatForever::create(to));
    waitLayer->setSwallowsTouches(true);
    waitLayer->addChild(pt);
    waitLayer->setVisible(false);
    addChild(waitLayer);

    return true;

}

void PET_VIEW::onEnter()
{
	Layer::onEnter();
}

void PET_VIEW::onExit()
{
	Layer::onExit();
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_REFRESH);
}

void PET_VIEW::onUpgradeCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
        return;

    int petId = PET_MODEL::getInstance()->getSelectedPetId();
    auto pet = PET_MODEL::getInstance()->_petConfigMap[petId];
    
    int role_id = Account_Data_Mgr::instance()->get_current_role_id();
    auto player = dynamic_cast<Game_Data::Player*>(CHARACTER_MGR::instance()->get_character(role_id));
    if(player->get_gold() < pet->consume)
    {
        return;
    }
    
    if ((pet->variation_id == pet->id) && (pet->star_up_id == pet->id) && (pet->quality_up_id == pet->id)) {
        return;
    }
    _upgradeButton->setTouchEnabled(false);
    _upgradeButton->setBright(false);
    std::vector<uint64> para;
    Game_Logic::Game_Content_Interface::instance()->exec_interface(EVENT_PET_SEND_UPGRADE, para);
    waitLayer->setVisible(true);
}

void PET_VIEW::onGoCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
        return;

	std::vector<uint64> para;
    Game_Logic::Game_Content_Interface::instance()->exec_interface(EVENT_PET_SEND_CHANGE, para);

    waitLayer->setVisible(true);
}

void PET_VIEW::onDetail(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
        return;
    showDetail = !showDetail;
    
    detailBg->setVisible(showDetail);
    if (!showDetail) {
        detailButton->setTitleText("详细");
    }else{
        detailButton->setTitleText("返回");
    }
}

void PET_VIEW::onCloseCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
        return;

    int petId = PET_MODEL::getInstance()->getCurrentPetId();
    if (petId > 0) {
        auto pet = PET_MODEL::getInstance()->_petConfigMap[petId];
        
        auto scene = (UI::City_Scene *)Director::getInstance()->getRunningScene();
        
        scene->changeGuardianArmature(pet->armName,emptyPet);
    }
    
	auto spawnAction = cocos2d::Spawn::createWithTwoActions(cocos2d::DelayTime::create(0.1), cocos2d::CallFunc::create(CC_CALLBACK_0(PET_VIEW::removeFromParent, this)));
	this->runAction(spawnAction);
}

void PET_VIEW::onClickCellCallback(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type!=cocos2d::ui::Widget::TouchEventType::ENDED)
        return;
    
	auto cell = (cocos2d::ui::Layout*)pSender;
	int selected_pet_id = cell->getTag();
    if (selected_pet_id == PET_MODEL::getInstance()->getSelectedPetId()) {
        return;
    }
    
    if (PET_MODEL::getInstance()->getSelectedPetId() != -1) {
        auto oldPet = _petsScrollView->getChildByTag(PET_MODEL::getInstance()->getSelectedPetId());
        oldPet->getChildByName("Img_Cursor")->setVisible(false);
    }
    
    
    auto pet = PET_MODEL::getInstance()->_petConfigMap[selected_pet_id];
	PET_MODEL::getInstance()->setSelectedPetId(cell->getTag());
	if (PET_MODEL::getInstance()->getCurrentPetId() == selected_pet_id)
	{
        _upgradeButton->setTouchEnabled(false);
        _upgradeButton->setBright(false);
        _upgradeButton->setVisible(true);
        _goButton->setTouchEnabled(false);
        _goButton->setVisible(false);
//        _goButton->setBright(false);
//        _goButton->setVisible(true);
        fightFlag->setVisible(true);
	} else {
        bool canUpgrade = true;
        int role_id = Account_Data_Mgr::instance()->get_current_role_id();
        auto player = dynamic_cast<Game_Data::Player*>(CHARACTER_MGR::instance()->get_character(role_id));
        if(player->get_gold() < pet->consume)
        {
            canUpgrade = false;
        }
        
        if ((pet->variation_id == pet->id) && (pet->star_up_id == pet->id) && (pet->quality_up_id == pet->id)) {
            canUpgrade = false;
        }
        _upgradeButton->setTouchEnabled(canUpgrade);
        _upgradeButton->setBright(canUpgrade);
        _upgradeButton->setVisible(true);
        _goButton->setTouchEnabled(true);
        _goButton->setBright(true);
        _goButton->setVisible(true);
        fightFlag->setVisible(false);
	}
    
	auto cursor = (cocos2d::ui::ImageView*)( cell->getChildByName("Img_Cursor") );
	cursor->setVisible(true);
    
    _petName->setString(pet->name);
    Value consume(pet->consume);
    _petConsume->setString(consume.asString());
    
    int attrId = pet->attrId;
    auto attr = PET_MODEL::getInstance()->_petAttrMap[attrId];
    
    float petPower = 0;
    int health = attr->health;
    int magic = attr->magic;
    int attack = attr->attack;
    int defense = attr->defense;
    int hit = attr->hit/* - HIT_SHOW_OFFSET*/;
    int dodge = attr->dodge;
    int crit = attr->crit/* - CRIT_SHOW_OFFSET*/;
    int tenacity = attr->tenacity;
    petPower = (0.1f*health) + (0.08f*magic) + (1.2f*attack) + (1.5f*defense) + (0.6f*hit) + (0.6f*dodge) + (0.6f*crit) + (0.6f*tenacity);
    Value petPowerV(petPower);
    _petPower->setString(petPowerV.asString());
    
    int q = 0;
    q = MIN(pet->quality, QUALITY_NUM);
    q = MAX(0, q);
    _quality->setString(qualityStr[q]);
    _quality->setVisible(true);
    
    for (int i = 0; i < STAR_NUM; i++) {
        _stars[i]->setVisible(false);
    }
    int s = pet->star;
    for (int i = 0; i<s; i++) {
        if (i>=STAR_NUM) {
            break;
        }
        _stars[i]->setVisible(true);
    }
    
    ArmatureDataManager::getInstance()->addArmatureFileInfo("armature/"+pet->armName+".ExportJson");
    model->init(pet->armName);
    model->getAnimation()->playWithIndex(0);
    model->setVisible(true);
    
    auto att=PET_MODEL::getInstance()->_petAttrMap[pet->attrId];
    hpTxt->setString(Convert2String(att->health));
    mpTxt->setString(Convert2String(att->magic));
    atkTxt->setString(Convert2String(att->attack));
    defTxt->setString(Convert2String(att->defense));
    critTxt->setString(Convert2String(att->crit));
    tenacTxt->setString(Convert2String(att->tenacity));
    hitRateTxt->setString(Convert2String(att->hit));
    dodgeTxt->setString(Convert2String(att->dodge));
    
    detailButton->setTouchEnabled(true);
    detailButton->setBright(true);
}

bool PET_NET::on_load(Game_Logic::Game_Interface& gm_interface)
{
//	char body[256] = { 0 };
//	message_stream body_ms((char*)gm_interface.get_buff(), gm_interface.get_buff_size());
//	body_ms.set_rd_ptr(sizeof(int));

//	int			player_id;
	int			pet_id;
	uint64		pet_guid;
//	body_ms >> player_id;
//	body_ms >> pet_id;
//	body_ms >> pet_guid;
    vector<uint64> para = gm_interface.get_para();
    pet_id = pet_guid = (int)para[0];

	int role_id = Account_Data_Mgr::instance()->get_current_role_id();
	auto player = CHARACTER_MGR::instance()->get_character(role_id);
	player->add_pet(pet_guid,pet_id);
	auto pets = PET_MODEL::getInstance()->getPets();
	Game_Model::Pet pet;
	pet.guid = pet_guid;
	pet.id = pet_id;
	pets->push_back(pet);
	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_CHANGE_DATA, nullptr);

	return true;
}

bool PET_NET::send_upgrade(Game_Logic::Game_Interface& gm_interface)
{
	int player_id = Account_Data_Mgr::instance()->get_current_role_id();

	int pet_id = PET_MODEL::getInstance()->getSelectedPetId();
	auto pets = PET_MODEL::getInstance()->getPets();
	std::vector<Game_Model::Pet>::iterator it = pets->begin();
	uint64 guid;
	for (; it != pets->end();it++)
	{
		if ((*it).id == pet_id)
		{
			guid = (*it).guid;
            break;
		}
	}
//	char body[256] = { 0 };
//	message_stream body_ms( body, sizeof(body) );
//	body_ms << C2S_GUARD_BEAST_LV_UP;
//	body_ms << player_id;
//	body_ms << guid;
//
//	CNetManager::GetMe()->send_msg(body_ms);
    Game_Logic::Game_Interface temp;
    vector<uint64> para;
    para.push_back(pet_id);
    auto pet = PET_MODEL::getInstance()->_petConfigMap[pet_id];
    para.push_back(pet->quality_up_id);
    temp.set_para(para);
    on_upgrade(temp);
	return true;

}

bool PET_NET::on_upgrade(Game_Logic::Game_Interface& gm_interface)
{
//	char body[256] = { 0 };
//	message_stream body_ms((char*)gm_interface.get_buff(), gm_interface.get_buff_size());
//	body_ms.set_rd_ptr(sizeof(int));

//	int player_id;
	int old_pet_id;
	int new_pet_id;
//	int exp = 0;
//	body_ms >> player_id;
//	body_ms >> old_pet_id;
//	body_ms >> new_pet_id;
//	body_ms >> exp;
    vector<uint64> para = gm_interface.get_para();
    old_pet_id = (int)para[0];
    new_pet_id = (int)para[1];

	if ( !new_pet_id || (old_pet_id == new_pet_id) )
        return false;

	int role_id = Account_Data_Mgr::instance()->get_current_role_id();
	auto player = CHARACTER_MGR::instance()->get_character(role_id);
	player->change_one_pet_of_list_by_id(old_pet_id, new_pet_id);

	auto pets = PET_MODEL::getInstance()->getPets();

	std::vector<Game_Model::Pet>::iterator it = pets->begin();
	for (; it != pets->end();it++)
	{
		if( (*it).id == old_pet_id )
        {
            (*it).id = new_pet_id;
            break;
        }
	}

    if (PET_MODEL::getInstance()->getSelectedPetId() == old_pet_id) {
        PET_MODEL::getInstance()->setSelectedPetId(new_pet_id);
    }

	Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_CHANGE_DATA, nullptr);
	return true;

}

bool PET_NET::send_change_state(Game_Logic::Game_Interface& gm_interface)
{
	int player_id = Account_Data_Mgr::instance()->get_current_role_id();

	int pet_id = PET_MODEL::getInstance()->getSelectedPetId();
	auto pets = PET_MODEL::getInstance()->getPets();
	std::vector<Game_Model::Pet>::iterator it = pets->begin();
	uint64 guid;
	for (; it != pets->end(); it++)
	{
		if ((*it).id == pet_id)
		{
			guid = (*it).guid;
		}
	}
//	char body[256] = { 0 };
//	message_stream body_ms(body, sizeof(body));
//	body_ms << C2S_GUARD_BEAST_CHANGE_STATE;
//	body_ms << player_id;
//	body_ms << guid;
//
//	CNetManager::GetMe()->send_msg(body_ms);
//
//	//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_CHANGE_DATA, nullptr);

    on_change_state(gm_interface);
	return true;
}

bool PET_NET::on_change_state(Game_Logic::Game_Interface& gm_interface)
{
//	char body[256] = { 0 };
//	message_stream body_ms((char*)gm_interface.get_buff(), gm_interface.get_buff_size());
//	body_ms.set_rd_ptr(sizeof(int));

	int			player_id;
	int			result;

//	body_ms >> player_id;
//	body_ms >> result;

	if (result)
	{
		int selected_id = PET_MODEL::getInstance()->getSelectedPetId();
		PET_MODEL::getInstance()->setCurrentPetId(selected_id);
        int player_id = Account_Data_Mgr::instance()->get_current_role_id();
        Game_Data::Player* player = dynamic_cast<Game_Data::Player*>(CHARACTER_MGR::instance()->get_character(player_id));
        player->set_cur_pet_id(selected_id);
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_CHANGE_DATA, nullptr);
	}
	else
	{

		return false;
	}

	return true;

}

int PET_MODEL::get_create_pet_id(int player_lv)
{
    int result = 0;
    auto _iter = m_map_get_pet.begin();
    for (; _iter != m_map_get_pet.end(); _iter++) {
        if (_iter->second.levelLimit <= player_lv) {
            result = _iter->second.petId;
        }else{
            break;
        }
    }
    return result;
    
}
