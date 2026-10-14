CREATE PROCEDURE [dbo].[P_LExchangeLimitItem_GET]
    @iTargetID_ [int]
AS
set nocount on
set transaction isolation level read uncommitted

select
    DestItemID
from dbo.LExchangeLimitItem with (nolock)
where
    TargetID = @iTargetID_


