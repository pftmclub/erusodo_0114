CREATE TABLE [dbo].[LEvent_GlobalMission]
(
[BeginDate] [smalldatetime] NOT NULL,
[EndDate] [smalldatetime] NOT NULL,
[MissionType] [int] NOT NULL,
[ClearCondition] [int] NOT NULL,
[ClearCount] [int] NOT NULL,
[NowCount] [int] NOT NULL CONSTRAINT [DF_LEvent_GlobalMission_NowCount] DEFAULT (0),
CONSTRAINT [PK_LEvent_GlobalMission_BeginDate] PRIMARY KEY CLUSTERED ( [BeginDate] )
) ON [PRIMARY]


