CREATE TABLE [dbo].[LEvent_RandomEnchant]
(
[EventUID] [int] NOT NULL IDENTITY(1, 1),
[BeginDate] [smalldatetime] NOT NULL,
[EndDate] [smalldatetime] NOT NULL,
[EnchantLevel] [tinyint] NOT NULL,
CONSTRAINT [PK_LEvent_RandomEnchant_EventUID] PRIMARY KEY CLUSTERED ( [EventUID] )
) ON [PRIMARY]


