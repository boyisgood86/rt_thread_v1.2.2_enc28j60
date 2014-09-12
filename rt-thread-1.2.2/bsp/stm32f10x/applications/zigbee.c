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
#include "zigbee.h"
#include "MacroAndConst.h"
#include "Interface.h"

/*从接收的Zigbee数据中获取clusterID*/
static void GetClusterID(uint16 *clusterID,const Send_Recv_CMDA_t *data);

/*提取Abs_NodeTypeLookup信息*/
static void Abs_NodeTypeLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeAllNwkLookup信息*/
static void Abs_NodeAllNwkLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeNwkLookup信息*/
static void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeNwkLookup信息*/
static void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeMacLookup信息*/
static void Abs_NodeMacLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodePanIDLookup信息*/
static void Abs_NodePanIDLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeExtpanidLookup信息*/
static void Abs_NodeExtpanidLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeChannelLookup信息*/
static void Abs_NodeChannelLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodePNwkLookup信息*/
static void Abs_NodePNwkLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodePMacLookup信息*/
static void Abs_NodePMacLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NodeAppTypeLookup信息*/
static void Abs_NodeAppTypeLookup(const Send_Recv_CMDA_t *data);

/*提取Abs_NetworkFound信息*/
static void Abs_NetworkFound(const Send_Recv_CMDA_t *data);

/*提取Abs_NetworkGetparamet信息*/
static void Abs_NetworkGetparamet(const Send_Recv_CMDA_t *data);

/**
  *	@fun	ZigbeeDataSend
  * @brief  Zigbee数据发送函数
  * @param  data	:指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  */
void ZigbeeDataSend(Send_Recv_CMDA_t *data)
{
//	USART1_SendData((uint8*)data,sizeof(Send_Recv_CMDA_t));
}

/**
  *	@fun	GetClusterID
  * @brief  从接收的Zigbee数据中获取clusterID
  * @param  clusterID:指向clusterID变量的指针
  * @param  data	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void GetClusterID(uint16 *clusterID,const Send_Recv_CMDA_t *data)
{
	*clusterID=0;
	*clusterID|=data->CMD;
	*clusterID<<=8;
	*clusterID|=data->ClusterID;
}

/**
  *	@fun	Zigbee_srcdataHandler
  * @brief  对接收的Zigbee数据进行处理
  * @param  data	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Zigbee_srcdataHandler(const Send_Recv_CMDA_t *data)
{
	uint16	clusterID;
	void (*pfn)(const Send_Recv_CMDA_t *data)=(void*)0;
	GetClusterID(&clusterID,data);
	switch(clusterID){
		case EXEAPP_CID_NODE_TYPE_LOOKUP:
            pfn=Abs_NodeTypeLookup;
            break;
        case EXEAPP_CID_NODE_ALLNWK_LOOKUP:
            pfn=Abs_NodeAllNwkLookup;
            break;
        case EXEAPP_CID_NODE_NWK_LOOKUP:
            pfn=Abs_NodeNwkLookup;
            break;
        case EXEAPP_CID_NODE_MAC_LOOKUP:
            pfn=Abs_NodeMacLookup;
            break;
        case EXEAPP_CID_NODE_PANID_LOOKUP:
            pfn=Abs_NodePanIDLookup;
            break;
        case EXEAPP_CID_NODE_EXTPANID_LOOKUP:
            pfn=Abs_NodeExtpanidLookup;
            break;
        case EXEAPP_CID_NODE_CHANNEL_LOOKUP:
            pfn=Abs_NodeChannelLookup;
            break;
        case EXEAPP_CID_NODE_P_NWK_LOOKUP:
            pfn=Abs_NodePNwkLookup;
            break;
        case EXEAPP_CID_NODE_P_MAC_LOOKUP:
            pfn=Abs_NodePMacLookup;
            break;
        case EXEAPP_CID_NODE_APPTYPE_LOOKUP:
            pfn=Abs_NodeAppTypeLookup;
            break;
        case EXEAPP_CID_NETWORK_FOUND:
            pfn=Abs_NetworkFound;
            break;
        case EXEAPP_CID_NETWORK_GETPARAMET:
            pfn=Abs_NetworkGetparamet;
            break;
        default:
            pfn=(void*)0;
            break;
    }
	if(pfn)	pfn(data);
}

/**
  *	@fun	Abs_NodeTypeLookup
  * @brief  提取Abs_NodeTypeLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeTypeLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeAllNwkLookup
  * @brief  提取Abs_NodeAllNwkLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeAllNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeNwkLookup
  * @brief  提取Abs_NodeNwkLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeMacLookup
  * @brief  提取Abs_NodeMacLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeMacLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePanIDLookup
  * @brief  提取Abs_NodePanIDLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePanIDLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeExtpanidLookup
  * @brief  提取Abs_NodeExtpanidLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeExtpanidLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeChannelLookup
  * @brief  提取Abs_NodeChannelLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeChannelLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePNwkLookup
  * @brief  提取Abs_NodePNwkLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePMacLookup
  * @brief  提取Abs_NodePMacLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePMacLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeAppTypeLookup
  * @brief  提取Abs_NodeAppTypeLookup信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeAppTypeLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NetworkFound
  * @brief  提取Abs_NetworkFound信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NetworkFound(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NetworkGetparamet
  * @brief  提取Abs_NetworkGetparamet信息
  * @param  data 	 :指向Zigbee使用的通道数据结构变量的指针
  * @retval None
  * @data	2014/9/1
  */
void Abs_NetworkGetparamet(const Send_Recv_CMDA_t *data)
{

}


