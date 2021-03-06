#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////////////
//数据定义

//连接类型
#define CLIENT_KIND_FALSH			1									//网页类型
#define CLIENT_KIND_MOBILE			2									//手机类型
#define CLIENT_KIND_COMPUTER		3									//电脑类型

//签到信息长度
#define SIGNIN_REWARD_ARRAY_LENGTH  7									//签到数组信息长度


//绑定参数
struct tagBindParameter
{
	//网络参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//连接信息
	WORD							wModuleID;							//模块标识
	BYTE							cbClientKind;						//连接类型
	DWORD							dwPlazaVersion;						//大厅版本

	//抽奖信息
	bool							bIsLtryInfoSend;					//是否需要发送抽奖信息

	DWORD							dwUserID;
	TCHAR							szNickName[LEN_NICKNAME];
};

struct tagWealthBag
{
	DWORD dwUserID;
	DWORD dwSocketID;
	int nScore;
	TCHAR szNickName[LEN_NICKNAME];
};

struct tagMBVerifInfo
{
	TCHAR szMobilePhone[LEN_MOBILE_PHONE];
	TCHAR szVerificationCode[LEN_VERIFICATION_CODE];
	time_t m_dwVerifCodeSendTime;
};

struct tagMapMBVerifInfo
{
	TCHAR szVerificationCode[LEN_VERIFICATION_CODE];
	time_t m_dwVerifCodeSendTime;
};

struct tagCompRankInfo
{
	int nRank;
	int nUserId;
	int nCount;
};

struct tagSuperRankInfo
{
	int nRank;
	TCHAR szNickName[32];
	LONGLONG lCount;
};

struct tagLoginQueueInfo
{
	DWORD							dwSocketId;
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//////////////////////////////////////////////////////////////////////////////////
//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bNeekCorrespond;					//协调标志
	bool                            m_bShowServerStatus;                //显示服务器状态

	//变量定义
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:
	CServerListManager				m_ServerListManager;				//列表管理
	std::vector<tagWealthBag>		m_vWealthBag;
	time_t							m_dwWealthBagStartTime;
	CMD_GPO_WealthBagResult			m_WealthBagResult;
	bool							m_bWealthBagRunning;
public:
	std::map<std::string,tagMapMBVerifInfo> m_mMBVerifInfo;
	std::map<std::string,int> m_mRegisteredMbNum;

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPSocketService *				m_pITCPSocketService;				//协调服务
public:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎

	//视频配置
protected:
	WORD							m_wAVServerPort;					//视频端口
	DWORD							m_dwAVServerAddr;					//视频地址

	//短信验证码
public:
	char *							m_SMSVerifyAccount;
	char *							m_SMSVerifyAccountToken;
	char *							m_SMSVerifyAppID;
	char *							m_SMSVerifyRestIP;//app.cloopen.com,不需要写https：//
	char *							m_SMSVerifyTemplateID;
	int								m_SMSVerifyPort;
	//比赛排名
public:
	std::vector<tagCompRankInfo>	m_CompRankInfo;
	WORD	m_nSendMaxNum;
	//超级海盗排名
public:
	std::vector<tagSuperRankInfo>	m_SuperRankInfo;
	//登录排队
public:
	std::vector<tagLoginQueueInfo>	m_LoginQueueInfo;

	//签到信息
tagSignInDayInfo m_NormalUserRewardConfig[7];
tagSignInDayInfo m_VIPUserRewardConfig[7];

	// fake server info
	WORD							m_wSendFakeInfoInterval;
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//远程服务
	bool OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录处理
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务处理
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//远程处理
	bool OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//道具处理
	bool OnTCPNetworkMainItemService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//技能处理
	bool OnTCPNetworkMainSkillService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//登录处理
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网站消息
	bool OnTCPNetworkMainWebSite(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//I D 登录
	bool OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游客登录
	bool OnTCPNetworkSubPCGuestReg(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//手机号注册
	bool OnTCPNetworkSubPCMBNumRegister(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//I D 登录
	bool OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//游戏事件
protected:
	//登录成功
	bool OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonValidatePassPostID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户头像
	bool OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行信息
	bool OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行成功
	bool OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//赠送信息
	bool OnDBPCUserInsureTransferInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行失败
	bool OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//密码验证
	bool OnDBPCUserCheckPassword(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作成功
	bool OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//手机事件
protected:
	//登录成功
	bool OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游客登录成功
	bool OnDBPCGuestLogon(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//列表事件
protected:
	//游戏种类
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏类型
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏节点
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏定制
	bool OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏列表
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

protected:
	//签到查询成功
	bool OnDBPCSignInQuerySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//签到查询失败
	bool OnDBPCSignInQueryFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户签到成功
	bool OnDBPCUserSignInSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户签到失败
	bool OnDBPCUserSignInFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//抽奖查询结果
	bool OnDBPCLotteryQueryResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户抽奖结果
	bool OnDBPCUserLotteryResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//邮件ID列表
	bool OnDBMailIDList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//邮件信息
	bool OnDBMailInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//邮件内容
	bool OnDBMailContent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//邮件物品
	bool OnDBMailItem(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//充值到账
	bool OnDBChargeValue(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//VIP信息
	bool OnDBVIPInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//VIP每日赠送金币
	bool OnDBVIPResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//当前分数
	bool OnDBCurrentScore(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录奖励
	bool OnDBLogonGift(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//首充成功
	bool OnDBCharge1stRequest(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//领取首充礼包奖励
	bool OnDBCharge1stGet(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//版本检测
	bool CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer=true);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//生成随机密码
	VOID GenRandCode(TCHAR szRandCode[]);

	//发送列表
protected:
	//发送类型
	VOID SendGameTypeInfo(DWORD dwSocketID);
	//发送种类
	VOID SendGameKindInfo(DWORD dwSocketID);
	//发送节点
	VOID SendGameNodeInfo(DWORD dwSocketID, WORD wKindID);
	//发送定制
	VOID SendGamePageInfo(DWORD dwSocketID, WORD wKindID);
	//发送房间
	VOID SendGameServerInfo(DWORD dwSocketID, WORD wKindID);
	//发送房间
	VOID SendGameServerInfoEx(DWORD dwSocketID, WORD wKindID);

	//手机列表
protected:
	//发送类型
	VOID SendMobileKindInfo(DWORD dwSocketID, WORD wModuleID);
	//发送房间
	VOID SendMobileServerInfo(DWORD dwSocketID, WORD wModuleID);

protected:
	//接受验证码请求
	bool OnTCPNetworkSubPCMBCODEVERIFY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//处理手机号验证结果
	bool OnDBPCVerifyMBNumRes(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//手机注册结果
	bool OnDBMBNumRegisterSuccessRes(DWORD dwContextID, VOID * pData, WORD wDataSize);

protected:
	char * ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn); 
// 生成六位随机数
	VOID GenRandNumCode(TCHAR szRandCode[]);

	//手机号绑定
protected:
	//接受验证码请求
	bool OnTCPNetworkSubPCMoBileCode(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//处理手机号绑定验证码验证
	bool OnTCPNetworkSubPCCodeVerification(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询手机号是否绑定过
	bool OnTCPNetworkSubPCMobileCheckExit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求广播速度
protected:
	bool OnTCPNetworkSubPCBroadCastSpeed(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求某一物品数量
protected:
	bool OnTCPNetworkSubPCItemCount(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取奖励
	bool OnTCPNetworkSubMatchReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//喇叭广播
protected:
	bool OnTCPNetworkSubPCBroadLaBa(VOID * pData, WORD wDataSize, DWORD dwSocketID);
protected:
	bool OnDBGetCompRankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
protected:
	bool OnDBBroadCastResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
protected:
	bool OnCancelLoginQueueing(DWORD dwSocketID);
protected:
	bool OnRequestDataBaseLogin();
protected:
	bool OnTCPNetworkSubPCChangePhoneFare(VOID * pData, WORD wDataSize, DWORD dwSocketID);
protected:
	bool OnDBChangePhoneFareResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
protected:
	bool OnDBGetSuperRankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询绑定手机奖励和新手引导信息
	bool OnTCPNetworkGetBindRewardNewGuid(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//设置超级海盗新手引导状态
	bool OnTCPNetworkSetSuperNewGuid(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//初始化签到奖励信息
	bool ReadSignInRewardConfig();
	bool OnDBGetNewMailRemindInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool OnDBGetMatchResultPrize(DWORD dwContextID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif