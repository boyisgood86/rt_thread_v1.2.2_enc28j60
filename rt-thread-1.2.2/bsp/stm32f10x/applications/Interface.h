/*
*Description:�ӿ����ݽṹ
*<br/>Copyright (C),2014-xxxx
*<br/>This program is protected by copyright laws.
*<br/>Progarm Name: 
*<br/>Date:2014/8/19
*<br/>Last modified Date:2014/8/22
*@author minphone.linails  linails@foxmail.com
*@version 1.0
*/
#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include "MacroAndConst.h"

/**
  * @brief 	�������
  * @brief 	�ŵ������书�ʡ�panID
  */
typedef struct{
    uint8  Channel;         //�ŵ�
    uint8  Txpower;         //���书��
    uint16 panID;           //panID
}NetParamt_t;

/**
  * @brief 	�ڵ���Ϣ
  * @brief 	�ڵ����͡��ڵ�Ӧ������
  */
typedef struct {
    uint8  NodeType;        //NodeType
    uint8  NodeAppType;     //Node App Type
    uint8  Reserved[2];	    //����λ-����ʹ��
}NodeInfo_t;

/**
  * @brief ���ڵ���Ϣ
  * @brief ���ڵ����͡����ڵ�Ӧ�����͡����ڵ�̵�ַ
  */
typedef struct {
    uint8  pNodeType;		//���ڵ�����
    uint8  pNodeAppType;	//���ڵ�Ӧ������
    uint16 pNwkSrc;	        //���ڵ�Nwk
}pNodeInfo_t;

/**
  * @brief �󶨿��ƺ͹�����ز���
  * @brief �̵�ַ��clusterID
  */
typedef struct{
    uint16 NwkDest;
    uint16 ClusterID;
}BindCtrl_t;

/**
  * @brief �Ʋ����ͱ���
  */
typedef struct
{
    uint16 ClusterID;       //���Ƶ�����
    uint8  CtrlVal;         //���ƵƵ�ֵ
    uint8  CtrlMode;        //���ƵƵ�ģʽ : 0-˲����� ; 1-�𽥱���
}LState_t;

/**
  * @brief ���������ͱ���
  */
typedef struct{
    uint8  CtrlVal;         //���ƴ�����ֵ
    uint8  Reserved[3];     //����λ-����ʹ��
}Curtain_t; 

/**
  * @brief �ʵƲ����ͱ���
  * @brief �����ƽṹ�����չ
  */
typedef struct{
    uint8  Red;				//��ɫֵ
    uint8  Blue;			//��ɫֵ
    uint8  Green;			//��ɫֵ
    uint8  Lux;				//����ֵ
}ColorL_t;

/**
  * @brief ��Ҫ���ݵ���Ϣ�������ڵ���Ϣ�͵�ַ��Ϣ�ȣ�����Ϊ4���ֽ�
  */
typedef union{
    NetParamt_t NetParamt;  //�������
    NodeInfo_t  NodeInfo;   //�ڵ���Ϣ
    pNodeInfo_t pNodeInfo;  //���ڵ���Ϣ
    BindCtrl_t  BindCtrl;   //�󶨿��ƺ͹�����ز���
    LState_t    LState;     //�Ʋ����ͱ���
    Curtain_t   Curtain;    //���������ͱ���
    ColorL_t    ColorL;     //�ʵƲ����ͱ���
}UnionInfo_t;

/**
  * @brief �ڵ������Ϣ & mac��ַ��Ϣ
  */
typedef union{
    struct{
        UnionInfo_t info;
        uint8       Reserved[4];
    }U_info;
    uint8           mac[8];
}UnionInfo_mac;

/**
  * @brief �ڵ���Ϣ
  */
typedef struct{
  UnionInfo_mac Info_mac;     /*��Ҫ���ݵ���Ϣ�������ڵ���Ϣ�͵�ַ��Ϣ��*/
  uint16        NwkDest;  	  /*NwkDestĿ�ĵ�ַ*/
}SendNode_t;

/**
  * @brief ���ͺͽ��յ�������ݽṹ
  * @brief ����ʹ�õ�12�ֽ����ݽṹ
  */
typedef struct{
  uint8            CMD;       /*����Command    */
  uint8            ClusterID; /*ClusterID Flag */
  SendNode_t       SendNode;  /*�ڵ���Ϣ       */
}Send_Recv_CMDA_t;

//-----------------------------------------------------------------------------

/**
  * @brief Zigbee���������нڵ�Ľڵ�Ӧ�����Ͷ���
  */
#define NODE_APP_TYPE_LIGHT             0x01  //������
#define NODE_APP_TYPE_CLIGHT            0x02  //�ʵ�����
#define NODE_APP_TYPE_CURTAIN           0x03  //��������
#define NODE_APP_TYPE_GETEWAY           0x04  //��������


#endif
