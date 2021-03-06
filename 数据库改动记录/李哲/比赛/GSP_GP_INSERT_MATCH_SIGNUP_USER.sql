USE [qptreasuredb]
GO
/****** Object:  StoredProcedure [dbo].[GSP_GP_INSERT_MATCH_SIGNUP_USER]    Script Date: 06/04/2016 11:59:38 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
ALTER PROCEDURE [dbo].[GSP_GP_INSERT_MATCH_SIGNUP_USER]
	@dwUserID INT,
	@MatchNum INT,
	@MatchDay VARCHAR(10),
	--@MatchType INT,
	@GroupID INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

	DECLARE @Enrollmentfee BIGINT 
	set @Enrollmentfee = 0
	DECLARE @UserScore BIGINT 
	set @UserScore = 0
	DECLARE @NickName NVARCHAR(31)
	declare @MatchStatus INT
	Set @MatchStatus = 1
	declare @ReturnValue INT
	set @ReturnValue = 0
	declare @MatchType INT 
	set @MatchType = 0
	
	-- init 
	select @Enrollmentfee = ApplyCost FROM qptreasuredb.dbo.MachConfig WHERE ID = @MatchNum
	select @UserScore = Score FROM qptreasuredb.dbo.GameScoreInfo WHERE UserID = @dwUserID
	select @MatchType = MachType FROM qptreasuredb.dbo.MachConfig WHERE ID = @MatchNum
	
	if exists (select * from MachApply where UserID = @dwUserID And MachNum = @MatchNum)
	begin
		set @MatchStatus = 1
		set @strErrorDescribe = '已经报名过一次, 此次报名失败'
		set @ReturnValue = 1
	end
	else if (@Enrollmentfee > @UserScore)
	begin
		set @MatchStatus = 0
		set @strErrorDescribe = '金币数量不足, 需要报名费' + CONVERT(varchar(20), @Enrollmentfee) + ', 请充值'
		set @ReturnValue = 2
	end
	else
	begin
		set @MatchStatus = 1
		set @strErrorDescribe = '报名成功'
		--insert into MachApply (ApplyTime,UserID,MachNum,MachDay,GroupID) VALUES (GETDATE(),@dwUserID,@MatchNum,@MatchDay,@GroupID)
		insert into MachApply (ApplyTime,UserID,MachNum,MachType,MachDay,GroupID) VALUES (GETDATE(),@dwUserID,@MatchNum,@MatchType,@MatchDay,@GroupID)
		update qptreasuredb.dbo.GameScoreInfo SET Score = Score - @Enrollmentfee WHERE UserID = @dwUserID
		
		set @ReturnValue = 0
	end
	
	select @NickName = Nickname from qpaccountsdb.dbo.AccountsInfo where UserID = @dwUserID

	SELECT @dwUserID AS UserID, @MatchNum AS MatchID, @MatchStatus AS MatchStatus,@NickName AS NickName 
	
	return @ReturnValue
END
