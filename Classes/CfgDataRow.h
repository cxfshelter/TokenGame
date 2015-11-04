//
//  CfgDataRow.h
//  TokenGame
//
//  Created by token on 15/3/29.
//
//

#ifndef __TokenGame__CfgDataRow__
#define __TokenGame__CfgDataRow__


#include "cocos2d.h"
#include "CfgData.h"

//USING_NS_CC;

class CfgDataRow
{

private:
    int m_CfgId;
    const ValueMapIntKey* m_vmi;
    CfgDataRow(const CfgDataRow& other){};
    CfgDataRow& operator = (const CfgDataRow& other){return *this;};
public:
    
    CfgDataRow(int cfgId);
    
    const Value* GetValue(CfgField field);
    
};

#endif /* defined(__TokenGame__CfgDataRow__) */
