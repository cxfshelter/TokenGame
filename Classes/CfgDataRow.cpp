//
//  CfgDataRow.cpp
//  TokenGame
//
//  Created by token on 15/3/29.
//
//

#include "CfgDataRow.h"


CfgDataRow::CfgDataRow(int cfgId)
{
    m_CfgId = cfgId;
    m_vmi = CfgData::getInstance()->GetCfgRow(cfgId);
}

const Value* CfgDataRow::GetValue(CfgField field)
{
    const Value* val;
    auto iter = m_vmi->find((int)field);
    if (iter == m_vmi->end())
    {
        //Id不存在
        CCLOGERROR("cfgId:%d not field:%d", m_CfgId, (int)field);
        val = &Value::Null;
    }
    else
    {
        val = &(iter->second);
    }
    
    return val;
}