/*�û��Զ��������*/
#ifndef _USER_DEFINE_H_
#define _USER_DEFINE_H_

//*****�ڵ���Ϣ��ѯ*******************
//************************************
#define CMD_NODE_INFO_LOOKUP      0x01       //CMD

//------------------------------------

#define CID_NODE_TYPE_LOOKUP      0x01  //�ڵ����Ͳ�ѯ
#define CID_NODE_ALLNWK_LOOKUP    0x02  //���нڵ��ַ��Ϣ��ѯ
#define CID_NODE_NWK_LOOPUP       0x03  //�ڵ�nwk��ַ��ѯ-����mac��ַ
#define CID_NODE_MAC_LOOPUP       0x04  //�ڵ�mac��ַ��ѯ-����nwk��ַ
#define CID_NODE_PANID_LOOPUP     0x05  //�ڵ�PANID��Ϣ��ѯ
#define CID_NODE_EXTPANID_LOOPUP  0x06  //�ڵ�extPANID��Ϣ��ѯ
#define CID_NODE_CHANNEL_LOOPUP   0x07  //�ڵ��ŵ���ѯ
#define CID_NODE_P_NWK_LOOPUP     0x08  //���ڵ�nwk��ַ��ѯ
#define CID_NODE_P_MAC_LOOPUP     0x09  //���ڵ�mac��ַ��ѯ
#define CID_NODE_APPTYPE_LOOKUP   0x0A  //�ڵ�Ӧ�����Ͳ�ѯ

//*****�������***********************
//************************************
#define CMD_NETWORK_OPRT          0x02       //CMD

//------------------------------------

#define CID_NETWORK_FORMAT_REQ    0x01  //�����ʽ������������ʹ��
#define CID_NETWORK_PERMIT_JOIN   0x02  //�����������
#define CID_NETWORK_PERMIT_LEAVE  0x03  //�����뿪����
#define CID_NETWORK_REMOVE_CHILD  0x04  //�Ƴ��ӽڵ�
#define CID_NETWORK_FOUND         0x05  //���緢��
#define CID_NETWORK_GETPARAMET    0x06  //��ȡ�������

//*****�ƿ��Ʋ���*********************
//************************************
#define CMD_LIGHT_OPRT            0x03       //CMD

//------------------------------------

#define CID_LIGHT_TURN_ON         0x01  //�ƿ���
#define CID_LIGHT_TURN_OFF        0x02  //�ƹر�

//*****�������Ʋ���*******************
//************************************
#define CMD_CURTAIN_OPRT          0x04       //CMD

//------------------------------------

#define CID_CURTAIN_OPEN          0x01  //������
#define CID_CURTAIN_CLOSE         0x02  //�����ر�
#define CID_CURTAIN_STOP          0x03  //����ֹͣ

//*****ϵͳ�㴦���ܼ�***************
//************************************
#define CMD_SYSTEM_FUNC           0x0F       //CMD

//------------------------------------

#define CID_HEARTBEAT             0x01  //���������¼�ID
#define CID_VERSION_LOOKUP        0x02  //�汾�鿴
#define CID_HEARTBEAT_RSP         0x03  //����Ӧ��

//*******************************����ClusterID*********************************
//Zigbee������ʹ�õĿ���ClusterID
//-----------------------------------------------------------------------------

#define EXEAPP_CID_NODE_TYPE_LOOKUP      0x0101  //�ڵ����Ͳ�ѯ
#define EXEAPP_CID_NODE_ALLNWK_LOOKUP    0x0102  //���нڵ��ַ��Ϣ��ѯ
#define EXEAPP_CID_NODE_NWK_LOOPUP       0x0103  //�ڵ�nwk��ַ��ѯ-����mac��ַ
#define EXEAPP_CID_NODE_MAC_LOOPUP       0x0104  //�ڵ�mac��ַ��ѯ-����nwk��ַ
#define EXEAPP_CID_NODE_PANID_LOOPUP     0x0105  //�ڵ�PANID��Ϣ��ѯ
#define EXEAPP_CID_NODE_EXTPANID_LOOPUP  0x0106  //�ڵ�extPANID��Ϣ��ѯ
#define EXEAPP_CID_NODE_CHANNEL_LOOPUP   0x0107  //�ڵ��ŵ���ѯ
#define EXEAPP_CID_NODE_P_NWK_LOOPUP     0x0108  //���ڵ�nwk��ַ��ѯ
#define EXEAPP_CID_NODE_P_MAC_LOOPUP     0x0109  //���ڵ�mac��ַ��ѯ
#define EXEAPP_CID_NODE_APPTYPE_LOOKUP   0x010A  //�ڵ�Ӧ�����Ͳ�ѯ

//-----------------------------------------------------------------------------

#define EXEAPP_CID_NETWORK_FORMAT_REQ    0x0201  //�����ʽ������������ʹ��
#define EXEAPP_CID_NETWORK_PERMIT_JOIN   0x0202  //�����������
#define EXEAPP_CID_NETWORK_PERMIT_LEAVE  0x0203  //�����뿪����
#define EXEAPP_CID_NETWORK_REMOVE_CHILD  0x0204  //�Ƴ��ӽڵ�
#define EXEAPP_CID_NETWORK_FOUND         0x0205  //���緢��
#define EXEAPP_CID_NETWORK_GETPARAMET    0x0206  //��ȡ�������

//-----------------------------------------------------------------------------

#define EXEAPP_CID_LIGHT_TURN_ON         0x0301  //�ƿ���
#define EXEAPP_CID_LIGHT_TURN_OFF        0x0302  //�ƹر�

//-----------------------------------------------------------------------------

#define EXEAPP_CID_CURTAIN_OPEN          0x0401  //������
#define EXEAPP_CID_CURTAIN_CLOSE         0x0402  //�����ر�
#define EXEAPP_CID_CURTAIN_STOP          0x0403  //����ֹͣ

//-----------------------------------------------------------------------------

#define EXEAPP_CID_HEARTBEAT             0x0F01  //���������¼�ID
#define EXEAPP_CID_VERSION_LOOKUP        0x0F02  //�汾�鿴
#define EXEAPP_CID_HEARTBEAT_RSP         0x0F03  //����Ӧ��

#endif
