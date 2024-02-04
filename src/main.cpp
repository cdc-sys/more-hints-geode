#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;
namespace hints{
bool hintAnimationFinished = true;
bool _jumpsRequirement;
int deaths = 0;
int jumps = 0;
struct HintData {
        std::string hint = "";
        float scale = 0.7f;
        float delay = 4.0f;
        
    };
    std::unordered_map<int, hints::HintData> getHints(){
        std::unordered_map<int, hints::HintData> hints = {};
	if (Mod::get()->getSettingValue<bool>("more-hints")){
    hints = {
		{0,{"Jump!",0.8f,4.5f}},
        {1, {"Click / Space to jump over the spikes"}},
        {2, {"Touching a Jump Pad will launch you in the air", 0.6f,4.0f}},
        {3, {"Click while touching a ring to jump mid air"}},
        {4,{"Touching a yellow portal will flip your gravity upside down",0.52f,4.0f}},
        {5,{"Click / Space to jump over the spikes"}},
        {6,{"Click / Space to jump!"}},
        {7,{"Click / Space to jump!"}},
        {8,{"Hold Left Click / Space to make progress in the level",0.55f,4.0f}},
        {9,{"Click / Space to switch gravity in the Ball gamemode",0.55f,4.0f}},
        {10,{"Click / Space to switch gravity in the Ball gamemode",0.55f,4.0f}},
        {11,{"Touching a Mini Portal will make your icon smaller",0.6f,4.0f}},
        {12,{"Click / Space in the Bird gamemode to flap upwards",0.6f,4.0f}},
        {13,{"Touching a Breakable Block will make it disappear",0.6f,4.0f}},
        {14,{""}},
        {15,{"Your speed changes when you touch a Speed Portal",0.5f,4.0f}},
        {16,{"You get split into two when touching a Dual Portal",0.6f,4.0f}},
        {17,{"The wave goes up while you're holding"}},
        {18,{""}},
        {19,{"Long Press in the Robot gamemode to jump higher!",0.5f}},
        {20,{""}},
        {21,{"Click / Space in the Spider gamemode to teleport to the nearest ceiling",0.4f}},
        {22,{"I know Geometry Dash is supposed to be unfair but this isn't sightreadable at all.",0.35f,4.5f}},
        {3001,{"This level is absolutely filled with bugs..."}}
    };
	}
	if (Mod::get()->getSettingValue<bool>("robtop-hints")){
		hints = {
        {1, {"Click / Space to jump over the spikes"}},
        {3, {"Click while touching a ring to jump mid air"}}
    };
	}
	if (Mod::get()->getSettingValue<bool>("custom-hint")){
		int64_t custom_id = Mod::get()->getSettingValue<int64_t>("custom-hint-levelid");
		std::string custom_text = Mod::get()->getSettingValue<std::string>("custom-hint-text");
		double custom_text_s = Mod::get()->getSettingValue<double>("custom-hint-text-scale");
		double custom_text_d = Mod::get()->getSettingValue<double>("custom-hint-text-delay");
		hints[custom_id] = hints::HintData{custom_text, static_cast<float>(custom_text_s), static_cast<float>(custom_text_d)};
	}
    return hints;
    }
}



class $modify(PlayerObject){
	void playerDestroyed(bool p2){
		PlayerObject::playerDestroyed(p2);
		hints::deaths++;
	}
};
class $modify(PauseLayer2,PauseLayer){
    void showHint(cocos2d::CCObject*sender){
        GameManager* gameManager = GameManager::sharedState();
        PlayLayer* playLayer = gameManager->getPlayLayer();
        hints::HintData hint = hints::getHints()[playLayer->m_level->m_levelID];
        FLAlertLayer::create("Hint",hint.hint,"OK")->show();
    }
	void customSetup(){
		PauseLayer::customSetup();
        GameManager* gameManager = GameManager::sharedState();
        PlayLayer* playLayer = gameManager->getPlayLayer();
        hints::HintData hint = hints::getHints()[playLayer->m_level->m_levelID];
        CCSprite* btnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(btnSprite,this,menu_selector(PauseLayer2::showHint));
        CCMenu* menu = CCMenu::create();
        if (hint.hint == ""){
            btn->setColor({ 132, 132, 132 });
            btn->setEnabled(false);
        }
        btn->setPosition({-248, 125});
        menu->addChild(btn);
        this->addChild(menu);
	}
};
class $modify(PlayLayer2,PlayLayer) {
    void onFinishHintAnim(cocos2d::CCObject* sender) {
        //std::cout << "2" << std::endl;
        auto node = static_cast<CCNode*>(sender);
        node->removeFromParent();
    }
	void incrementJumps(){
		PlayLayer::incrementJumps();
		hints::jumps++;
	}
	bool init(GJGameLevel* p0,bool p1,bool p2){
		hints::hintAnimationFinished = true;
		hints::deaths = 0;
		hints::jumps = 0;
        hints::_jumpsRequirement = Mod::get()->getSettingValue<bool>("require-no-jumps");
		return PlayLayer::init(p0,p1,p2);
	}
	void updateProgressbar(){
		//geode::log::info("d: {} j: {}",deaths,jumps);
        if (hints::_jumpsRequirement){
		    if (hints::deaths == 2&&hints::jumps == 0){
		        PlayLayer::showHint();
		    }
        }
        else{
            if (hints::deaths == 2){
                PlayLayer::showHint();
            }
        }
		PlayLayer::updateProgressbar();
		
	}
	/*void onQuit(){
		PlayLayer::onQuit();
		deaths = 0;
		jumps = 0;
	}*/
	void showHint() {
		// Modified decomp of PlayLayer::showHint
    //std::cout << this->m_level->m_levelID << std::endl;
    //std::cout << self->m_level->m_sLevelName;
    if (!hints::hintAnimationFinished) {
        return;
    }
    
    hints::hintAnimationFinished = false;
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto levelID = this->m_level->m_levelID;

    std::unordered_map<int, hints::HintData> hints = hints::getHints();

	if (hints[levelID].hint == ""){
       return;   
    }
    //std::cout << "hint: " << hints[levelID].hint << " scale: " << hints[levelID].scale << " delay: " << hints[levelID].delay << std::endl;
    auto text = CCLabelBMFont::create(hints[levelID].hint.c_str(), "bigFont.fnt");
    if (hints[levelID].hint != ""){
    //auto hintlayer = FLAlertLayer::create("Hint",hints[levelID].hint,"OK");
    }
    // OG Pos: +60
    if (Mod::get()->getSettingValue<bool>("new-layout")){
    text->setPosition({ winSize.width * 0.5f,winSize.height * 0.5f + 80.f });
    auto text2 = CCLabelBMFont::create("Hint!", "goldFont.fnt");
    auto background = CCScale9Sprite::create("square02_001.png");
    text2->setPosition({ winSize.width * 0.5f,winSize.height * 0.5f + 120.f });
    background->setPosition({ winSize.width * 0.5f,winSize.height * 0.5f + 100.f });
    if ((text->getScaledContentSize().width*hints[levelID].scale) + 25 > 100){
    background->setContentSize({ (text->getScaledContentSize().width*hints[levelID].scale) + 25,85});

    }
    else{
        background->setContentSize({ 100,85});
    }
    background->setOpacity(0);
    text2->setScale(1.2f);
    text2->setOpacity(0);
    text2->runAction(
        CCSequence::create(
        CCFadeIn::create(0.5),
        CCDelayTime::create(hints[levelID].delay),
        CCFadeOut::create(0.5),
        CCCallFuncO::create(text,callfuncO_selector(PlayLayer2::onFinishHintAnim),text),
        nullptr
        )
    );
    background->runAction(
        CCSequence::create(
        CCFadeTo::create(0.5,160),
        CCDelayTime::create(hints[levelID].delay),
        CCFadeTo::create(0.5,0),
        CCCallFuncO::create(text,callfuncO_selector(PlayLayer2::onFinishHintAnim),text),
        nullptr
        )
    );
    text->setZOrder(999);
    text2->setZOrder(999);
    background->setZOrder(998);
    this->addChild(text2);
    this->addChild(background);
    }
    else{
        text->setPosition({ winSize.width * 0.5f,winSize.height * 0.5f + 60.f });
        text->setZOrder(999);
    }
    text->setScale(hints[levelID].scale);
    text->setOpacity(0);
    text->runAction(
        CCSequence::create(
        CCFadeIn::create(0.5),
        CCDelayTime::create(hints[levelID].delay),
        CCFadeOut::create(0.5),
        CCCallFuncO::create(text,callfuncO_selector(PlayLayer2::onFinishHintAnim),text),
        nullptr
        )
    );
    this->addChild(text);
    //this->addChild(hintlayer);
	} 
};
