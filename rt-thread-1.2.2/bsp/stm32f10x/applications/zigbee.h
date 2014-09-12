/*
*Description:Zigbee的数据发送和接收通道
*<br/>Copyright (C),2014-
*<br/>This program is protected by copyright laws.
*<br/>Progarm Name:
*<br/>Date:2014/9/1
*<br/>Last modified Date:2014/9/1
*@author minphone.linails  linails@foxmail.com
*@version 1.0
*/
#ifndef _ZIGBEE_H_
#define _ZIGBEE_H_
#include "MacroAndConst.h"
#include "Interface.h"

//*****节点信息查询*******************
//************************************
#define CMD_NODE_INFO_LOOKUP      0x01       //CMD

//------------------------------------

#define CID_NODE_TYPE_LOOKUP      0x01  //节点类型查询
#define CID_NODE_ALLNWK_LOOKUP    0x02  //所有节点地址信息查询
#define CID_NODE_NWK_LOOPUP       0x03  //节点nwk地址查询-根据mac地址
#define CID_NODE_MAC_LOOPUP       0x04  //节点mac地址查询-根据nwk地址
#define CID_NODE_PANID_LOOPUP     0x05  //节点PANID信息查询
#define CID_NODE_EXTPANID_LOOPUP  0x06  //节点extPANID信息查询
#define CID_NODE_CHANNEL_LOOPUP   0x07  //节点信道查询
#define CID_NODE_P_NWK_LOOPUP     0x08  //父节点nwk地址查询
#define CID_NODE_P_MAC_LOOPUP     0x09  //父节点mac地址查询
#define CID_NODE_APPTYPE_LOOKUP   0x0A  //节点应用类型查询

//*****网络操作***********************
//************************************
#define CMD_NETWORK_OPRT          0x02       //CMD

//------------------------------------

#define CID_NETWORK_FORMAT_REQ    0x01  //网络格式化，重启网络使用
#define CID_NETWORK_PERMIT_JOIN   0x02  //允许加入网络
#define CID_NETWORK_PERMIT_LEAVE  0x03  //允许离开网络
#define CID_NETWORK_REMOVE_CHILD  0x04  //移除子节点
#define CID_NETWORK_FOUND         0x05  //网络发现
#define CID_NETWORK_GETPARAMET    0x06  //获取网络参数

//*****灯控制操作*********************
//************************************
#define CMD_LIGHT_OPRT            0x03       //CMD

//------------------------------------

#define CID_LIGHT_TURN_ON         0x01  //灯开启
#define CID_LIGHT_TURN_OFF        0x02  //灯关闭

//*****窗帘控制操作*******************
//************************************
#define CMD_CURTAIN_OPRT          0x04       //CMD

//------------------------------------

#define CID_CURTAIN_OPEN          0x01  //窗帘打开
#define CID_CURTAIN_CLOSE         0x02  //窗帘关闭
#define CID_CURTAIN_STOP          0x03  //窗帘停止

//*******************************公共ClusterID*********************************
//Zigbee网络内使用的控制ClusterID
//-----------------------------------------------------------------------------

#define EXEAPP_CID_NODE_TYPE_LOOKUP      0x0101  //节点类型查询
#define EXEAPP_CID_NODE_ALLNWK_LOOKUP    0x0102  //所有节点地址信息查询
#define EXEAPP_CID_NODE_NWK_LOOKUP       0x0103  //节点nwk地址查询-根据mac地址
#define EXEAPP_CID_NODE_MAC_LOOKUP       0x0104  //节点mac地址查询-根据nwk地址
#define EXEAPP_CID_NODE_PANID_LOOKUP     0x0105  //节点PANID信息查询
#define EXEAPP_CID_NODE_EXTPANID_LOOKUP  0x0106  //节点extPANID信息查询
#define EXEAPP_CID_NODE_CHANNEL_LOOKUP   0x0107  //节点信道查询
#define EXEAPP_CID_NODE_P_NWK_LOOKUP     0x0108  //父节点nwk地址查询
#define EXEAPP_CID_NODE_P_MAC_LOOKUP     0x0109  //父节点mac地址查询
#define EXEAPP_CID_NODE_APPTYPE_LOOKUP   0x010A  //节点应用类型查询

//-----------------------------------------------------------------------------

#define EXEAPP_CID_NETWORK_FORMAT_REQ    0x0201  //网络格式化，重启网络使用
#define EXEAPP_CID_NETWORK_PERMIT_JOIN   0x0202  //允许加入网络
#define EXEAPP_CID_NETWORK_PERMIT_LEAVE  0x0203  //允许离开网络
#define EXEAPP_CID_NETWORK_REMOVE_CHILD  0x0204  //移除子节点
#define EXEAPP_CID_NETWORK_FOUND         0x0205  //网络发现
#define EXEAPP_CID_NETWORK_GETPARAMET    0x0206  //获取网络参数

//-----------------------------------------------------------------------------

#define EXEAPP_CID_LIGHT_TURN_ON         0x0301  //灯开启
#define EXEAPP_CID_LIGHT_TURN_OFF        0x0302  //灯关闭

//-----------------------------------------------------------------------------

#define EXEAPP_CID_CURTAIN_OPEN          0x0401  //窗帘打开
#define EXEAPP_CID_CURTAIN_CLOSE         0x0402  //窗帘关闭
#define EXEAPP_CID_CURTAIN_STOP          0x0403  //窗帘停止

//-----------------------------------------------------------------------------

/*Zigbee数据发送函数*/
void ZigbeeDataSend(Send_Recv_CMDA_t *data);

/*对接收的Zigbee数据进行处理*/
extern void Zigbee_srcdataHandler(const Send_Recv_CMDA_t *data);

#endif
