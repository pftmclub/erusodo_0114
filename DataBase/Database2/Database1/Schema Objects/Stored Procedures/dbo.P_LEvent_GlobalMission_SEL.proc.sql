CREATE PROCEDURE [dbo].[P_LEvent_GlobalMission_SEL]
AS
set nocount on
set transaction isolation level read uncommitted

declare @sdtNow smalldatetime
select @sdtNow = getdate()

select
    CONVERT(nvarchar(19), BeginDate, 120)
,   CONVERT(nvarchar(19), EndDate, 120)
,   MissionType
,   ClearCondition
,   ClearCount
,   NowCount
from dbo.LEvent_GlobalMission with (nolock)
where
    EndDate >= @sdtNow
order by
    BeginDate


