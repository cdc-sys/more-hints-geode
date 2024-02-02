#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

bool hintAnimationFinished = true;
int deaths = 0;
int jumps = 0;

class HintNode {
public:
    void onFinishAnim(cocos2d::CCObject* sender) {
        //std::cout << "2" << std::endl;
        auto node = reinterpret_cast<CCMenuItemSpriteExtra*>(sender);
        node->removeFromParent();
    }
};
class $modify(PlayerObject){
	void playerDestroyed(bool p2){
		PlayerObject::playerDestroyed(p2);
		deaths++;
	}
};
class $modify(PlayLayer) {
	void incrementJumps(){
		PlayLayer::incrementJumps();
		jumps++;
	}
	bool init(GJGameLevel* p0,bool p1,bool p2){
		hintAnimationFinished = true;
		deaths = 0;
		jumps = 0;
		return PlayLayer::init(p0,p1,p2);
	}
	void updateProgressbar(){
		//geode::log::info("d: {} j: {}",deaths,jumps);
		if (deaths == 2&&jumps == 0){
		PlayLayer::showHint();
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
    if (!hintAnimationFinished) {
        return;
    }
    hintAnimationFinished = false;
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto levelID = this->m_level->m_levelID;
    const char* hint = "";
    float scale = 0.7f;
    float delay = 3.0f;
    struct HintData {
        const char* hint = "";
        float scale = 0.7f;
        float delay = 4.0f;
    };
	std::unordered_map<int, HintData> hints = {};
	if (Mod::get()->getSettingValue<bool>("more-hints")){
    hints = {
		{0,{"Jump!",0.4f,4.5f}},
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
        {12,{"Touching a Mini Portal will make your icon smaller",0.6f,4.0f}},
        {13,{"Touching a Breakable Block will make it disappear",0.6f,4.0f}},
        {14,{""}},
        {15,{"Your speed changes when you touch a Speed Portal",0.5f,4.0f}},
        {16,{"You get split into two when touching a Dual Portal",0.6f,4.0f}},
        {17,{"The wave goes up while you're holding"}},
        {18,{""}},
        {19,{"Click / Space to jump over the monster"}},
        {20,{""}},
        {21,{"Hold to dash!"}},
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
		auto custom_id = Mod::get()->getSettingValue<int64_t>("custom-hint-levelid");
		auto custom_text = Mod::get()->getSettingValue<std::string>("custom-hint-text");
		auto custom_text_s = Mod::get()->getSettingValue<double>("custom-hint-text-scale");
		auto custom_text_d = Mod::get()->getSettingValue<double>("custom-hint-text-delay");
		 hints = {
        {static_cast<int>(custom_id), {custom_text.c_str(),static_cast<float>(custom_text_s),static_cast<float>(custom_text_d)}
		}
		};
	}
	
    //std::cout << "hint: " << hints[levelID].hint << " scale: " << hints[levelID].scale << " delay: " << hints[levelID].delay << std::endl;
    auto text = CCLabelBMFont::create(hints[levelID].hint, "bigFont.fnt");
    text->setPosition({ winSize.width * 0.5f,winSize.height * 0.5f + 60.f });
    text->setScale(hints[levelID].scale);
    text->setOpacity(0);
    text->runAction(
        CCSequence::create(
        CCFadeIn::create(0.5),
        CCDelayTime::create(hints[levelID].delay),
        CCFadeOut::create(0.5),
        CCCallFuncO::create(text,callfuncO_selector(HintNode::onFinishAnim),text),
        nullptr
        )
    );
    text->setZOrder(999);
    this->addChild(text);
	} 
};
