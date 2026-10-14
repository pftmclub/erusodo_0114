CREATE PROCEDURE [dbo].[P_LExchangeLimitInfo_GET]
AS
set nocount on
set transaction isolation level read uncommitted

declare @sdtNow smalldatetime
select @sdtNow = getdate()

select
    LimitUID
,   TargetID
,   LimitQuantity
,   CONVERT(nvarchar(19), BeginDate, 120)
,   CONVERT(nvarchar(19), EndDate, 120)
from dbo.LExchangeLimitInfo with (nolock)
where
    EndDate >= @sdtNow
order by
    BeginDate


