// Ported forward from KNCSDK/Include/dbg/logfile.cpp for the VC10 server port.
//
// KNCSDK/lib_2010/KncDebug71MD.lib was built from an older snapshot of dbg::logfile that
// predates the LogStat feature declared in logfile.hpp (see the "2013. 02. 19" fenced block
// there), so these four members are missing from the prebuilt lib and must be compiled
// directly into each server instead. KNCSDK/Include/dbg/logfile.cpp (the VC7.1 snapshot)
// still has the original implementation this was ported from; std::hash_value there is
// stdext::hash_value here since VC10 moved that extension out of namespace std.
#include "logfile.hpp"
#include <sstream>
#include <xhash>

namespace dbg
{
	int logfile::GetLogFileSize()
	{
		m_file.seekp( 0, std::ios::end );
		return m_file.tellp();
	}

	logfile& logfile::LogStat( const wchar_t* pStrFunctionName, const wchar_t* pStrFileName, const int iLineNum )
	{
		if( m_sLevel > ms_sLevel ) return *this;

		std::wstringstream wstrStream;
		wstrStream << iLineNum;

		std::wstring wstrHashKey;
		wstrHashKey += pStrFunctionName;
		wstrHashKey += pStrFileName;
		wstrHashKey += wstrStream.str();
		const unsigned int uiHashKey = stdext::hash_value( wstrHashKey );

		{
			KLocker lock( m_csLogStat );

			std::map< unsigned int, KLogStat >::iterator mit;
			mit = m_mapLogStat.find( uiHashKey );
			if( mit == m_mapLogStat.end() )
			{
				KLogStat kLogStat;
				kLogStat.m_wstrFunctionName = pStrFunctionName;
				kLogStat.m_wstrFileName = pStrFileName;
				kLogStat.m_wstrLineNum = wstrStream.str();
				kLogStat.m_iCount = 1;
				m_mapLogStat.insert( std::make_pair( uiHashKey, kLogStat ) );
			}
			else
			{
				++mit->second.m_iCount;
			}
		}

		return *this;
	}

	void logfile::GetLogStat( std::map< unsigned int, KLogStat >& mapLogStat ) const
	{
		mapLogStat.clear();

		{
			KLocker lock( m_csLogStat );

			mapLogStat = m_mapLogStat;
		}
	}

	void logfile::ClearLogStat()
	{
		KLocker lock( m_csLogStat );

		m_mapLogStat.clear();
	}
}
