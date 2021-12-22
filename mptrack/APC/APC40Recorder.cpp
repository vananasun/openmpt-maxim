#ifdef MPT_WITH_APC
#include <APCAPI.h>
#include "../MPTrack.h"
#include "../Mainfrm.h"
#include "../Moddoc.h"
#include "../View_pat.h"
#include "../View_ins.h"
#include "./APC40.h"
//#include "./WinEvent.h"
#include "./WinSharedMem.h"


enum RecordState
{
	UNINITIALIZED = 0,
	NOT_RECORDING = 1,
	REQUEST_START_RECORDING = 2,
	REQUEST_STOP_RECORDING = 3,
	DONE_RECORDING = 4,
};



/**
 *  \brief There is an OpenMPT utility function out there, but this one ensures a C drive path.
 */
static wchar_t *GetTempAudioFilePath()
{
	static wchar_t path[MAX_PATH];
	if(S_OK == (SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
	{
		PathAppendW(path, _TEXT("\\Temp\\$OpenMPT rec2sample.wav"));
	}
	return path;
}



void APC40Recorder::toggle()
{
	if(!establishConnectionWithVST()) return;
	if(!m_connected) return;

	if (NOT_RECORDING == m_sharedMem->state)
	{
		m_sharedMem->state = REQUEST_START_RECORDING;
		theApp.m_apc40->m_api->setRecord(true);
	} else if (REQUEST_START_RECORDING == m_sharedMem->state)
	{
		m_sharedMem->state = REQUEST_STOP_RECORDING;
		theApp.m_apc40->m_api->setRecord(false);
	}

}


void APC40Recorder::recordHandler()
{
	if (!m_connected) return;

	// Handle reinitialization request
	if (m_sharedMem->resetOpenMPT)
	{
		resetRecordFunctionality();
		return;
	}

	if (m_sharedMem->state == DONE_RECORDING)
	{
		if (!loadRecordedSample())
		{
			Reporting::Warning("Unable to sample the recording!");
		}
		m_sharedMem->state = NOT_RECORDING;
	}
}



void APC40Recorder::resetRecordFunctionality()
{
	m_sharedMem->state = NOT_RECORDING;
	m_sharedMem->resetOpenMPT = false;
	theApp.m_apc40->m_api->setRecord(false);
}


bool APC40Recorder::establishConnectionWithVST()
{
	if (!m_sharedMem)
	{
		m_sharedMem = reinterpret_cast<MptApcMem*>(SharedMem_Open("OPENMPT_APC40MKII", 32));
		if (!m_sharedMem)
		{
			if (!m_errored) {
				Reporting::Warning("Could not establish connection with OpenMPT Sample Recorder VST.");
				m_errored = true;
			}
			return false;
		}

		resetRecordFunctionality();
		m_connected = true;

	}
	return true;
}



bool APC40Recorder::loadRecordedSample()
{
	// @TODO: test this

	// Read the recording into an instrument
	InputFile f(mpt::PathString::FromCString(GetTempAudioFilePath()));
	if(!f.IsValid()) return false;
	FileReader fileReader = GetFileReader(f);
	CModDoc *pDoc = CMainFrame::GetMainFrame()->GetActiveDoc();
	INSTRUMENTINDEX nInstr = pDoc->GetSoundFile().GetNextFreeInstrument();
	if(INSTRUMENTINDEX_INVALID == nInstr) return false;
	if(!pDoc->GetSoundFile().AllocateInstrument(nInstr)) return false;
	if(!pDoc->GetSoundFile().ReadSampleAsInstrument(nInstr, fileReader, false)) return false;
	strcpy(pDoc->GetSoundFile().Instruments[nInstr]->name.buf, "MPTRecordSample output");

	// Select the instrument
	pDoc->UpdateAllViews(InstrumentHint().Info().Names());
	pDoc->UpdateAllViews(SampleHint().Data());
	pDoc->ViewInstrument(nInstr);

	return true;

	/*POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CViewPattern *pView = dynamic_cast<CViewPattern *>(pDoc->GetNextView(pos));
		if(pView != nullptr && pView->GetDocument() == pDoc)
		{
			pView->SendCtrlMessage(CTRLMSG_SETCURRENTINSTRUMENT, nInstr);
		}
	}*/

	// Select the instrument in the pattern editor
	/*POSITION pos = doc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CViewPattern *pView = dynamic_cast<CViewPattern *>(doc->GetNextView(pos));
		if(pView != nullptr && pView->GetDocument() == doc)
		{
			pView->SendCtrlMessage(CTRLMSG_PAT_SETINSTRUMENT, nInstr);
		}
	}*/
	
	// Switch view to instrument editor with the newly recorded instrument
	//doc->ActivateWindow();
	//doc->ViewSample(nSample);
	//doc->ViewInstrument(nInstr);

	//pos = doc->GetFirstViewPosition();
	//while(pos != NULL)
	//{
	//	CViewInstrument *pView = dynamic_cast<CViewInstrument *>(doc->GetNextView(pos));
	//	if(pView != nullptr && pView->GetDocument() == doc)
	//	{
	//		pView->GetParentFrame()->SetActiveView(pView, 1);
	//		//pView->SendMessage(VIEWMSG_SETACTIVE)
	//		pView->SendCtrlMessage(CTRLMSG_SETCURRENTINSTRUMENT, nInstr);
	//	}
	//}
}



#endif