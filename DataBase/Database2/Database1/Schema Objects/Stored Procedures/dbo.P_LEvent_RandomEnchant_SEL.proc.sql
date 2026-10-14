CREATE PROCEDURE [dbo].[P_LEvent_RandomEnchant_SEL]
AS
set nocount on
set transaction isolation level read uncommitted

declare @sdtNow smalldatetime
select @sdtNow = getdate()

select
    EventUID
,   CONVERT(nvarchar(19), BeginDate, 120)
,   CONVERT(nvarchar(19), EndDate, 120)
,   EnchantLevel
from dbo.LEvent_RandomEnchant with (nolock)
where
    EndDate >= @sdtNow
order by
    BeginDate


