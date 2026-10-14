CREATE PROCEDURE [dbo].[P_LEvent_RandomEnchant_UPD]
    @iEventUID_ [int],
    @iUnitUID_ [int],
    @sdtRegDate_ [smalldatetime],
    @iOK [int] = 0
AS
set nocount on

if not exists(select * from dbo.LEvent_RandomEnchant with (nolock) where EventUID = @iEventUID_)
    begin    select @iOK = -1    goto end_proc    end

if exists(select * from dbo.LEvent_RandomEnchant_Claim with (nolock) where EventUID = @iEventUID_ and UnitUID = @iUnitUID_)
    begin    select @iOK = -2    goto end_proc    end

insert into dbo.LEvent_RandomEnchant_Claim
        (
            EventUID
,           UnitUID
,           RegDate
        )
    select  @iEventUID_
,           @iUnitUID_
,           @sdtRegDate_
if @@error <> 0
    begin    select @iOK = -3    goto end_proc    end

end_proc:
select @iOK as OK


