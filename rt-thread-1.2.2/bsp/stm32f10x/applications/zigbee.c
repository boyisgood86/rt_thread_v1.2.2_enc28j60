/*
*Description:Zigbee�����ݷ��ͺͽ���ͨ��
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

/*�ӽ��յ�Zigbee�����л�ȡclusterID*/
static void GetClusterID(uint16 *clusterID,const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeTypeLookup��Ϣ*/
static void Abs_NodeTypeLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeAllNwkLookup��Ϣ*/
static void Abs_NodeAllNwkLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeNwkLookup��Ϣ*/
static void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeNwkLookup��Ϣ*/
static void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeMacLookup��Ϣ*/
static void Abs_NodeMacLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodePanIDLookup��Ϣ*/
static void Abs_NodePanIDLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeExtpanidLookup��Ϣ*/
static void Abs_NodeExtpanidLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeChannelLookup��Ϣ*/
static void Abs_NodeChannelLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodePNwkLookup��Ϣ*/
static void Abs_NodePNwkLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodePMacLookup��Ϣ*/
static void Abs_NodePMacLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NodeAppTypeLookup��Ϣ*/
static void Abs_NodeAppTypeLookup(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NetworkFound��Ϣ*/
static void Abs_NetworkFound(const Send_Recv_CMDA_t *data);

/*��ȡAbs_NetworkGetparamet��Ϣ*/
static void Abs_NetworkGetparamet(const Send_Recv_CMDA_t *data);

/**
  *	@fun	ZigbeeDataSend
  * @brief  Zigbee���ݷ��ͺ���
  * @param  data	:ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  */
void ZigbeeDataSend(Send_Recv_CMDA_t *data)
{
//	USART1_SendData((uint8*)data,sizeof(Send_Recv_CMDA_t));
}

/**
  *	@fun	GetClusterID
  * @brief  �ӽ��յ�Zigbee�����л�ȡclusterID
  * @param  clusterID:ָ��clusterID������ָ��
  * @param  data	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
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
  * @brief  �Խ��յ�Zigbee���ݽ��д���
  * @param  data	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
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
  * @brief  ��ȡAbs_NodeTypeLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeTypeLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeAllNwkLookup
  * @brief  ��ȡAbs_NodeAllNwkLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeAllNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeNwkLookup
  * @brief  ��ȡAbs_NodeNwkLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeMacLookup
  * @brief  ��ȡAbs_NodeMacLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeMacLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePanIDLookup
  * @brief  ��ȡAbs_NodePanIDLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePanIDLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeExtpanidLookup
  * @brief  ��ȡAbs_NodeExtpanidLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeExtpanidLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeChannelLookup
  * @brief  ��ȡAbs_NodeChannelLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeChannelLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePNwkLookup
  * @brief  ��ȡAbs_NodePNwkLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePNwkLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodePMacLookup
  * @brief  ��ȡAbs_NodePMacLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodePMacLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NodeAppTypeLookup
  * @brief  ��ȡAbs_NodeAppTypeLookup��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NodeAppTypeLookup(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NetworkFound
  * @brief  ��ȡAbs_NetworkFound��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NetworkFound(const Send_Recv_CMDA_t *data)
{

}

/**
  *	@fun	Abs_NetworkGetparamet
  * @brief  ��ȡAbs_NetworkGetparamet��Ϣ
  * @param  data 	 :ָ��Zigbeeʹ�õ�ͨ�����ݽṹ������ָ��
  * @retval None
  * @data	2014/9/1
  */
void Abs_NetworkGetparamet(const Send_Recv_CMDA_t *data)
{

}


