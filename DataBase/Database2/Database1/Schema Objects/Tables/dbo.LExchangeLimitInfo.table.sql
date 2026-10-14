CREATE TABLE [dbo].[LExchangeLimitInfo]
(
[LimitUID] [int] NOT NULL IDENTITY(1, 1),
[TargetID] [int] NOT NULL,
[LimitQuantity] [int] NOT NULL,
[BeginDate] [smalldatetime] NOT NULL,
[EndDate] [smalldatetime] NOT NULL,
CONSTRAINT [PK_LExchangeLimitInfo_LimitUID] PRIMARY KEY CLUSTERED ( [LimitUID] )
) ON [PRIMARY]


