CREATE TABLE [dbo].[LEvent_RandomEnchant_Claim]
(
[EventUID] [int] NOT NULL,
[UnitUID] [int] NOT NULL,
[RegDate] [smalldatetime] NOT NULL,
CONSTRAINT [PK_LEvent_RandomEnchant_Claim_EventUID_UnitUID] PRIMARY KEY CLUSTERED ( [EventUID], [UnitUID] )
) ON [PRIMARY]


