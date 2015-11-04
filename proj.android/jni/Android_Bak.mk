LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp\
                   ../../Classes/AchievementScene.cpp\
                   ../../Classes/AchievementUI.cpp\
                   ./../Classes/AppDelegate.cpp\
                   ../../Classes/BasicExcel.cpp\
                   ./../Classes/BattlePubScene.cpp\
                   ../../Classes/BattlePubUI.cpp\
                   ../../Classes/BinaryUtils.cpp\
                   ../../Classes/CfgData.cpp\
                   ../../Classes/ExcelToCfgData.cpp\
                   ../../Classes/ExplainUI.cpp\
                   ../../Classes/GameUtils.cpp\
                   ../../Classes/HelloWorldScene.cpp\
                   ../../Classes/LoadScene.cpp\
                   ../../Classes/MainScene.cpp\
                   ../../Classes/MainUI.cpp\
                   ../../Classes/MakeBuildScene.cpp\
                   ../../Classes/MakeBuildUI.cpp\
                   ../../Classes/MakeItemScene.cpp\
                   ../../Classes/MakeItemUI.cpp\
                   ../../Classes/MsgListUI.cpp\
                   ../../Classes/MultiLanguage.cpp\
                   ../../Classes/ProductionScene.cpp\
                   ../../Classes/ProductionUI.cpp\
                   ../../Classes/ShopScene.cpp\
                   ../../Classes/ShopUI.cpp\
                   ../../Classes/TalentScene.cpp\
                   ../../Classes/TalentUI.cpp\
                   ../../Classes/TemplateScene.cpp\
                   ../../Classes/TemplateUI.cpp\
                   ../../Classes/TraingHeroScene.cpp\
                   ../../Classes/TraingHeroUI.cpp\
                   ../../Classes/UserData.cpp\
                   ../../Classes/UserEventCallBack.cpp\
                   ../../Classes/UserEventContainer.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
