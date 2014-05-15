#ifndef __APPMACROS_H__
#define __APPMACROS_H__

#include "cocos2d.h"

typedef struct tagResourc {
    cocos2d::CCSize size;
    char directory[100];
} Resource;

/*----- android用 -----*/
static Resource smallResource  = { cocos2d::CCSizeMake( 540,  960), "S"};
static Resource mediumResource = { cocos2d::CCSizeMake( 720, 1280), "M"};
static Resource largeResource  = { cocos2d::CCSizeMake(1080, 1920), "L"};

static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(720, 1280);

#define NUMBER_FONT_SIZE (cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().width / mediumResource.size.width * 48)

#endif // __APPMACROS_H__
