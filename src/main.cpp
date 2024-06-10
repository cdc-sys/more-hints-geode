#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <matjson/stl_serialize.hpp>

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
    std::unordered_map<int,hints::HintData> customHints = {};
    std::unordered_map<int,hints::HintData> defaultHints = {
		//{0,{"Jump!",0.8f,4.5f}},
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
        {3001,{"This level is absolutely filled with bugs..."}}};

    std::unordered_map<int, hints::HintData> getHints(){
        std::unordered_map<int, hints::HintData> hints = {};
	if (Mod::get()->getSettingValue<bool>("more-hints")){
        hints = hints::defaultHints;
	}
	if (Mod::get()->getSettingValue<bool>("robtop-hints")){
		hints = {
        {1, {"Click / Space to jump over the spikes"}},
        {3, {"Click while touching a ring to jump mid air"}}
    };
	}
	if (Mod::get()->getSettingValue<bool>("custom-hint")){
        hints = hints::customHints;
		/*int64_t custom_id = Mod::get()->getSettingValue<int64_t>("custom-hint-levelid");
		std::string custom_text = Mod::get()->getSettingValue<std::string>("custom-hint-text");
		double custom_text_s = Mod::get()->getSettingValue<double>("custom-hint-text-scale");
		double custom_text_d = Mod::get()->getSettingValue<double>("custom-hint-text-delay");
		hints[custom_id] = hints::HintData{custom_text, static_cast<float>(custom_text_s), static_cast<float>(custom_text_d)};*/
	}
    return hints;
    }
}

template <>
    struct ::matjson::Serialize<hints::HintData> {
        static hints::HintData from_json(const matjson::Value& value) {
            //geode::log::info("{}",value.dump());
            return hints::HintData {
                .hint = value["hint"].as_string(),
                .scale = std::stof(value["scale"].as_string()),
                .delay = std::stof(value["delay"].as_string()),
            };
        }
        static matjson::Value to_json(const hints::HintData& hint) {
            return matjson::Object {
                { "hint", hint.hint },
                { "scale", std::to_string(hint.scale) },
                { "delay", std::to_string(hint.delay) }
                };
    }
        // You can also implement this method:
        // > static bool is_json(const matjson::Value& value);
        // It is only used if you do value.is<User>();
};

namespace hints {
    void saveCustomHints(){
        std::unordered_map<std::string, hints::HintData> customHintsJSONCompat = {};
        for (auto& [key, value]: hints::customHints) {  customHintsJSONCompat[std::to_string(key)] = value; }
        Mod::get()->setSavedValue("custom_hints",customHintsJSONCompat);
    }
    void getSavedCustomHints(){
        std::unordered_map<std::string, hints::HintData> customHintsJSONCompat = Mod::get()->getSavedValue<std::unordered_map<std::string, hints::HintData>>("custom_hints");
        for (auto& [key, value]: customHintsJSONCompat) {  hints::customHints[std::stoi(key)] = value; }
    }
}

class $modify(PlayerObject){
	void playerDestroyed(bool p2){
		PlayerObject::playerDestroyed(p2);
		hints::deaths++;
	}
};
class CreateHintPopup : public geode::Popup<> {
protected:
    CCNode* nodeToRemove;
    InputNode* hintTextInputNode;
    InputNode* hintScaleInputNode;
    InputNode* hintDelayInputNode;
    bool editHintMode;
    float clip(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }
    void onCreate(cocos2d::CCObject*sender){
        PlayLayer* playLayer = PlayLayer::get();
        int levelID = playLayer->m_level->m_levelID;
        std::string hintText = hintTextInputNode->getString();
        std::string scaleText = hintScaleInputNode->getString();
        std::string delayText = hintDelayInputNode->getString();
        if (hintText == ""){
            hintText = "Jump!";
        }
        if (scaleText == ""){
            scaleText = "0";
        }
        if (delayText == ""){
            delayText = "0";
        }
        float scaleFloat = this->clip(std::stof(scaleText),0,2);
        float delayFloat = this->clip(std::stof(delayText),2,6);
        Mod::get()->setSettingValue<bool>("custom-hint",true);/*
        Mod::get()->setSettingValue<int64_t>("custom-hint-levelid",levelID);
        Mod::get()->setSettingValue<std::string>("custom-hint-text",hintText);
        Mod::get()->setSettingValue<double>("custom-hint-text-scale",scaleFloat);
        Mod::get()->setSettingValue<double>("custom-hint-text-delay",delayFloat);*/
        hints::customHints[levelID] = hints::HintData{hintText, scaleFloat, delayFloat};
        if (editHintMode){
            FLAlertLayer::create("Success!","Hint edited!","OK")->show();
        }
        else{
            FLAlertLayer::create("Success!","Hint created!","OK")->show();
            this->nodeToRemove->removeFromParent();
        }
        this->onClose(sender);
        hints::saveCustomHints();
    }
    virtual void onEnterTransitionDidFinish(){
        if (!Mod::get()->getSettingValue<bool>("disable-creation-disclaimer2")){
            //FLAlertLayer::create("Disclaimer!","You can <cr>ONLY</c> have <cr>ONE</c> <cy>custom</c> hint at a time for the time being.\nEvery time you <cj>create</c> a new hint your <co>old</c> one gets <cr>overwritten</c>.","OK")->show();
            //FLAlertLayer::create("Disclaimer!","You can <cr>ONLY</c> have <cj>TEMPORARY</c> <cy>custom</c> hints for the time being.\nYour custom hints are <cr>NOT</c> saved.","OK")->show();
            FLAlertLayer::create("Disclaimer!","When you create a <cy>custom</c> hint the <co>Use Custom Hints</c> setting is <cg>enabled</c>.\n<co>Use Custom Hints</c> is <cr>NOT</c> compatible with <cr>ANY OTHER USE HINTS SETTING</c>, <cr>disable</c> it in the <cy>Mod Settings</c> to get the default <cb>Hints+</c> hints back.","OK")->show();
            Mod::get()->setSettingValue<bool>("disable-creation-disclaimer2",true);
        }
    }
    bool setup() override {
        PlayLayer* playLayer = PlayLayer::get();
        int levelID = playLayer->m_level->m_levelID;
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        if (editHintMode){
            this->setTitle("Edit Hint");
        }
        else{
            this->setTitle("Create Hint");
        }
        ButtonSprite* createBtnSprite = ButtonSprite::create("Create","goldFont.fnt","GJ_button_01.png");
        if (editHintMode){
            createBtnSprite = ButtonSprite::create("Edit","goldFont.fnt","GJ_button_01.png");
        }
        CCMenuItemSpriteExtra* createBtn = CCMenuItemSpriteExtra::create(createBtnSprite,this,menu_selector(CreateHintPopup::onCreate));
        CCMenu* menu = CCMenu::create();
        hintTextInputNode = InputNode::create(250.f,"Jump!","chatFont.fnt");
        hintScaleInputNode = InputNode::create(250.f/4,"0.7","chatFont.fnt");
        hintDelayInputNode = InputNode::create(250.f/4,"4.0","chatFont.fnt");
        if (editHintMode){
            auto hints = hints::getHints();
            hintTextInputNode->setString(hints[levelID].hint);
            hintScaleInputNode->setString(std::to_string(hints[levelID].scale));
            hintDelayInputNode->setString(std::to_string(hints[levelID].delay));
        }
        CCLabelBMFont* hintTextLabel = CCLabelBMFont::create("Hint Text:","bigFont.fnt");
        CCLabelBMFont* hintScaleLabel = CCLabelBMFont::create("Hint Scale:","bigFont.fnt");
        CCLabelBMFont* hintDelayLabel = CCLabelBMFont::create("Hint Delay:","bigFont.fnt");


        hintTextInputNode->getInput()->m_allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-=?.,/1234567890 ";
        hintScaleInputNode->getInput()->m_allowedChars = "1234567890.";
        hintDelayInputNode->getInput()->m_allowedChars = "1234567890.";
        hintTextInputNode->setPosition({winSize.width / 2,winSize.height / 2 + 60});
        hintTextLabel->setPosition({winSize.width / 2,winSize.height / 2 + 86});
        hintTextLabel->setScale(0.4f);
        hintScaleInputNode->setPosition({winSize.width / 2,winSize.height / 2});
        hintScaleLabel->setPosition({winSize.width / 2,winSize.height / 2 + 26});
        hintScaleLabel->setScale(0.4f);
        hintDelayInputNode->setPosition({winSize.width / 2,winSize.height / 2 + -61});
        hintDelayLabel->setPosition({winSize.width / 2,winSize.height / 2 + -35});
        hintDelayLabel->setScale(0.4f);
        createBtn->setPosition({0,-110});

        this->m_mainLayer->addChild(hintTextInputNode);
        this->m_mainLayer->addChild(hintScaleInputNode);
        this->m_mainLayer->addChild(hintDelayInputNode);
        this->m_mainLayer->addChild(hintTextLabel);
        this->m_mainLayer->addChild(hintScaleLabel);
        this->m_mainLayer->addChild(hintDelayLabel);
        menu->addChild(createBtn);
        this->m_mainLayer->addChild(menu);
        //this->runAction(CCCallFuncO::create(this,callfuncO_selector(CreateHintPopup::openDisclaimer),this));
        return true;
    }

public:
    static CreateHintPopup* create(cocos2d::CCObject* sender,bool edit) {
        auto ret = new CreateHintPopup();
        ret->editHintMode = edit;
        ret->nodeToRemove = static_cast<CCNode*>(sender);
        if (ret && ret->init(300.f, 270.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
class $modify(PauseLayer2,PauseLayer){
    bool editHint = false;
    void showHint(cocos2d::CCObject*sender){
        GameManager* gameManager = GameManager::sharedState();
        PlayLayer* playLayer = gameManager->getPlayLayer();
        hints::HintData hint = hints::getHints()[playLayer->m_level->m_levelID];
        FLAlertLayer::create("Hint",hint.hint,"OK")->show();
    }
    void showCreateHint(cocos2d::CCObject*sender){
        CreateHintPopup* popup = CreateHintPopup::create(sender,editHint);
        popup->show();
    }
    void deleteHint(cocos2d::CCObject*sender){
        GameManager* gameManager = GameManager::sharedState();
        PlayLayer* playLayer = gameManager->getPlayLayer();
        int levelID = playLayer->m_level->m_levelID;
        hints::customHints[levelID] = {};
        FLAlertLayer::create("Success!","Hint deleted.","OK")->show();
        static_cast<CCNode*>(sender)->removeFromParent();
        editHint = false;
    }
	void customSetup(){
		PauseLayer::customSetup();
        CCDirector* director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        GameManager* gameManager = GameManager::sharedState();
        PlayLayer* playLayer = gameManager->getPlayLayer();
        hints::HintData hint = hints::getHints()[playLayer->m_level->m_levelID];
        CCSprite* btnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(btnSprite,this,menu_selector(PauseLayer2::showHint));
        CCMenu* menu = CCMenu::create();
        CCMenu* leftMenu = static_cast<CCMenu*>(this->getChildByID("left-button-menu"));
        leftMenu->addChild(btn);
        bool usingCustomHints = Mod::get()->getSettingValue<bool>("custom-hint");
        if (hint.hint == ""){
            btn->setColor({ 132, 132, 132 });
            btn->setEnabled(false);
            CCSprite* createBtnSprite = CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png");
            CCMenuItemSpriteExtra* createBtn = CCMenuItemSpriteExtra::create(createBtnSprite,this,menu_selector(PauseLayer2::showCreateHint));
            //OLD POS: createBtn->setPosition({-218, 125});
            //if (Loader::get()->isModLoaded("creo.small-gd-mods")){createBtn->setPosition({104,winSize.height-34});}
            //else{createBtn->setPosition({64,winSize.height-34});}
            //menu->addChild(createBtn);
            leftMenu->addChild(createBtn);
            editHint = false;
        }
        if (usingCustomHints&& hint.hint != ""){
            CCSprite* createBtnSprite = CCSprite::createWithSpriteFrameName("GJ_editObjBtn3_001.png");
            createBtnSprite->setScale(0.6f);
            CCMenuItemSpriteExtra* createBtn = CCMenuItemSpriteExtra::create(createBtnSprite,this,menu_selector(PauseLayer2::showCreateHint));
            leftMenu->addChild(createBtn);
            editHint = true;
            CCSprite* deleteBtnSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
            deleteBtnSprite->setScale(0.5f);
            CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteBtnSprite,this,menu_selector(PauseLayer2::deleteHint));
            leftMenu->addChild(deleteBtn);
        }
        //OLD POS: btn->setPosition({-248, 125});
       // if (Loader::get()->isModLoaded("creo.small-gd-mods")){btn->setPosition({74,winSize.height-34});}
        //winSize.height-34
        //else{btn->setPosition({bgPos.x+14,0});}
        //menu->setPosition({-bg->getContentSize().width/2,-bg->getContentSize().height/2});
        //menu->addChild(btn);
        //this->addChild(menu);
        //this->addChildAtPosition(menu,Anchor::TopLeft,{34,34});
        //leftMenu->alignItemsVerticallyWithPadding(30.f);
        leftMenu->updateLayout();
	}
};

class $modify(PlayerObject){
    void incrementJumps(){
		PlayerObject::incrementJumps();
		hints::jumps++;
	}
};
class $modify(PlayLayer2,PlayLayer) {
    void onFinishHintAnim(cocos2d::CCObject* sender) {
        //std::cout << "2" << std::endl;
        auto node = static_cast<CCNode*>(sender);
        node->removeFromParent();
    }
	bool init(GJGameLevel* p0,bool p1,bool p2){
		hints::hintAnimationFinished = true;
		hints::deaths = 0;
		hints::jumps = 0;
        hints::_jumpsRequirement = Mod::get()->getSettingValue<bool>("require-no-jumps");
        hints::getSavedCustomHints();
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
        CCCallFuncO::create(text2,callfuncO_selector(PlayLayer2::onFinishHintAnim),text2),
        nullptr
        )
    );
    background->runAction(
        CCSequence::create(
        CCFadeTo::create(0.5,160),
        CCDelayTime::create(hints[levelID].delay),
        CCFadeTo::create(0.5,0),
        CCCallFuncO::create(background,callfuncO_selector(PlayLayer2::onFinishHintAnim),background),
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
