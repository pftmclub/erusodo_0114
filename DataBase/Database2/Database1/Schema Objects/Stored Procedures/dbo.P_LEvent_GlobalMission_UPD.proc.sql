CREATE PROCEDURE [dbo].[P_LEvent_GlobalMission_UPD]
    @sdtGlobalMissionDay_ [smalldatetime],
    @iNowCount_ [int],
    @iOK [int] = 0
AS
set nocount on

if not exists(select * from dbo.LEvent_GlobalMission with (nolock) where BeginDate = @sdtGlobalMissionDay_)
    begin    select @iOK = -1    goto end_proc    end

update dbo.LEvent_GlobalMission
    set NowCount = @iNowCount_
    where BeginDate = @sdtGlobalMissionDay_
if @@error <> 0
    begin    select @iOK = -2    goto end_proc    end

end_proc:
select @iOK as OK


