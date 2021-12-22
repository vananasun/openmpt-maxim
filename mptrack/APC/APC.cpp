//#ifdef MPT_WITH_APC
//#include <APCAPI.h>
//#include "../../soundlib/plugins/PlugInterface.h"
//#include "../MPTrack.h"
//#include "../Mainfrm.h"
//#include "../Moddoc.h"
//#include "../CommandSet.h"
//#include "../View_pat.h"
//#include "./APC.h"
//#include "./APCSavedData.h"
//#include "./WinEvent.h"
//
//int g_animStep = 0;
//int g_animTimeout = 0;
//bool g_channelsOnBeforeSolo[MAX_CHANNELS];
//bool g_APCEnabled = false;
//
//
//
//
////
////static void jumpColumn(bool right)
////{
////	CView *pView = CMainFrame::GetMainFrame()->GetActiveView();
////	if(!strcmp("CViewPattern", pView->GetRuntimeClass()->m_lpszClassName))
////	{
////		reinterpret_cast<CViewPattern *>(pView)->MoveCursor(right?1:0);
////	}
////}
////
////
/////*
//// * Move left or right one channel, wrapping around is needed.
////**/
////static void jumpChannel(CModDoc *doc, bool right)
////{
////	CView *pView = CMainFrame::GetMainFrame()->GetActiveView();
////	if(!strcmp("CViewPattern", pView->GetRuntimeClass()->m_lpszClassName))
////	{
////		CViewPattern *pPat = reinterpret_cast<CViewPattern *>(pView);
////		if(right)
////		{
////			pPat->SetCurrentColumn((pPat->GetCurrentChannel() + 1) % doc->GetSoundFile().GetNumChannels(), pPat->GetCursor().GetColumnType());
////		} else
////		{
////			if(pPat->GetCurrentChannel() > 0)
////			{
////				pPat->SetCurrentColumn((pPat->GetCurrentChannel() - 1) % doc->GetSoundFile().GetNumChannels(), pPat->GetCursor().GetColumnType());
////			} else
////			{
////				pPat->SetCurrentColumn(doc->GetSoundFile().GetNumChannels() - 1, pPat->GetCursor().GetColumnType());
////			}
////		}
////	}
////}
////
////
////static void jumpUpSingleRow()
////{
////	CView *pView = CMainFrame::GetMainFrame()->GetActiveView();
////	if(!strcmp("CViewPattern", pView->GetRuntimeClass()->m_lpszClassName))
////	{
////		reinterpret_cast<CViewPattern *>(pView)->SetCurrentRow(
////			reinterpret_cast<CViewPattern *>(pView)->GetCurrentRow() - 1, true);
////	}
////}
////
////static void jumpDownSingleRow()
////{
////	CView *pView = CMainFrame::GetMainFrame()->GetActiveView();
////	if(!strcmp("CViewPattern", pView->GetRuntimeClass()->m_lpszClassName))
////	{
////		reinterpret_cast<CViewPattern *>(pView)->SetCurrentRow(
////			reinterpret_cast<CViewPattern *>(pView)->GetCurrentRow() + 1, true);
////	}
////}
////
////static void jumpUpQuarterPattern(CModDoc *doc)
////{
////	// Jump to last quarter of pattern but keep the offset within the current
////	// quarter to keep the beat in sync.
////	ROWINDEX nRowCount = doc->GetSoundFile().Patterns[doc->GetSoundFile().GetCurrentPattern()].GetNumRows();
////	ROWINDEX nNextRow = doc->GetSoundFile().m_PlayState.m_nRow - (nRowCount / 4) + 1;
////	if(static_cast<long>(nNextRow) < 0)
////	{
////		nNextRow += nRowCount; // wrap around pattern
////	}
////	doc->GetSoundFile().m_PlayState.m_nNextRow = nNextRow;
////}
////
////static void jumpDownQuarterPattern(CModDoc *doc)
////{
////	ROWINDEX nRowCount = doc->GetSoundFile().Patterns[doc->GetSoundFile().GetCurrentPattern()].GetNumRows();
////	ROWINDEX nNextRow = doc->GetSoundFile().m_PlayState.m_nRow + (nRowCount / 4) + 1;
////	if(nNextRow > nRowCount)
////	{  // wrap around
////		nNextRow -= nRowCount;
////	}
////	doc->GetSoundFile().m_PlayState.m_nNextRow = nNextRow;
////}
//
//
////void APC::errorCallback(std::string &msg, void *userData)
////{
////	Reporting::Error(msg, nullptr);
////	g_APCEnabled = false; // @TODO: not sure if this is appropriate...
////}
//
//
////void APC::Tick()
////{
////
////	while(theApp.m_apc40mkii->poll(&e))
////	{
////		switch(e.type)
////		{
////			case APC40MkII_EventType::PlayStop:
////				doc->OnPatternRestart(true);
////				theApp.m_apc40mkii->setPlay(true);
////				break;
////			case APC40MkII_EventType::Record:
////				APC::ToggleRecord();
////				//if(!e.value) break;  // don't trigger on release
////				//doc->OnPatternPlay();
////				//theApp.m_apc40mkii->setRecord(false);
////				break;
////			case APC40MkII_EventType::Session:
////				if(!e.value) break; // don't trigger on release
////				theApp.m_apc40mkii->setSession(false);
////				CMainFrame::GetActiveWindow()->SendMessage(WM_MOD_KEYCOMMAND, kcPatternRecord);
////				break;
////			case APC40MkII_EventType::Pan:
////				CMainFrame::GetActiveWindow()->SendMessage(WM_MOD_KEYCOMMAND, kcChangeLoopStatus);
////				break;
////			case APC40MkII_EventType::Sends:
////				CMainFrame::GetActiveWindow()->SendMessage(WM_MOD_KEYCOMMAND, kcToggleFollowSong);
////				break;
////
////			case APC40MkII_EventType::Nudge:
////				if(!e.value) break;  // don't trigger on release
////				CMainFrame::GetActiveWindow()->SendMessage(WM_MOD_KEYCOMMAND, (e.direction == 1) ? kcNextOrder : kcPrevOrder);
////				break;
////
////
////			case APC40MkII_EventType::SceneLaunch:
////				if(e.value >= 0 && e.value <= 3)
////				{
////					// jump into the section BUT keep the offset of the section so the beat matches
////					ROWINDEX numRows = doc->GetSoundFile().Patterns[doc->GetSoundFile().GetCurrentPattern()].GetNumRows();
////					float quarter = static_cast<float>(numRows) * 0.25f;
////					float rowOffset = static_cast<float>(doc->GetSoundFile().m_PlayState.m_nRow);
////					while(rowOffset > quarter)
////					{
////						rowOffset -= quarter;
////					}
////					ROWINDEX nextRow = static_cast<ROWINDEX>(e.num * quarter + rowOffset) + 1;
////					doc->GetSoundFile().m_PlayState.m_nNextRow = nextRow;
////				}
////				break;
////
////

////
////
////	}
////
////
////	if (g_animTimeout++ % 5 == 1)
////		theApp.m_apc40mkii->displayPattern(static_cast<unsigned char>(g_animStep++));
////
////}
//
//
//#endif // MPT_WITH_APC