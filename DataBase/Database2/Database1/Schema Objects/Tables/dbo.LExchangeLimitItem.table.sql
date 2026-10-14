CREATE TABLE [dbo].[LExchangeLimitItem]
(
[TargetID] [int] NOT NULL,
[DestItemID] [int] NOT NULL,
CONSTRAINT [PK_LExchangeLimitItem_TargetID_DestItemID] PRIMARY KEY CLUSTERED ( [TargetID], [DestItemID] )
) ON [PRIMARY]


