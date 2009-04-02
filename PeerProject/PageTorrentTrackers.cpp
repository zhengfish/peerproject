//
// PageTorrentTrackers.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2006.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"

#include "BENode.h"
#include "DlgDownloadSheet.h"
#include "PageTorrentTrackers.h"
#include "CoolInterface.h"
#include "Network.h"
#include "Skin.h"
#include "Transfers.h"
#include "Downloads.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTorrentTrackersPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CTorrentTrackersPage, CPropertyPageAdv)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TORRENT_REFRESH, &CTorrentTrackersPage::OnTorrentRefresh)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_TORRENT_TRACKER, &CTorrentTrackersPage::OnEnChangeTorrentTracker)
	ON_NOTIFY(NM_CLICK, IDC_TORRENT_TRACKERS, &CTorrentTrackersPage::OnNMClickTorrentTrackers)
	ON_CBN_SELCHANGE(IDC_TORRENT_TRACKERMODE, &CTorrentTrackersPage::OnCbnSelchangeTorrentTrackermode)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTorrentTrackersPage property page

CTorrentTrackersPage::CTorrentTrackersPage() : 
	CPropertyPageAdv( CTorrentTrackersPage::IDD ),
	m_pDownload( NULL )
{
}

CTorrentTrackersPage::~CTorrentTrackersPage()
{
}

void CTorrentTrackersPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageAdv::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TORRENT_TRACKER, m_sTracker);
	DDX_Control(pDX, IDC_TORRENT_COMPLETED, m_wndComplete);
	DDX_Control(pDX, IDC_TORRENT_INCOMPLETE, m_wndIncomplete);
	DDX_Control(pDX, IDC_TORRENT_REFRESH, m_wndRefresh);
	DDX_Control(pDX, IDC_TORRENT_TRACKERS, m_wndTrackers);
	DDX_Control(pDX, IDC_TORRENT_TRACKERMODE, m_wndTrackerMode);
}

/////////////////////////////////////////////////////////////////////////////
// CTorrentTrackersPage message handlers

BOOL CTorrentTrackersPage::OnInitDialog()
{
	if ( ! CPropertyPageAdv::OnInitDialog() )
		return FALSE;
	
	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	m_pDownload = ((CDownloadSheet*)GetParent())->m_pDownload;
	if ( ! Downloads.Check( m_pDownload ) || ! m_pDownload->IsTorrent() )
		return FALSE;

	CBTInfo& oInfo = m_pDownload->m_pTorrent;

	m_sTracker = oInfo.GetTrackerAddress();
	int nCount = oInfo.GetTrackerCount();
	int nTrackerMode = oInfo.GetTrackerMode();

	// Remove invalid modes
	if ( nCount < 2 )
	{
		ASSERT( nTrackerMode!= CBTInfo::tMultiFound );
		m_wndTrackerMode.DeleteString( CBTInfo::tMultiFound );
		ASSERT( nTrackerMode != CBTInfo::tMultiFinding );
		m_wndTrackerMode.DeleteString( CBTInfo::tMultiFinding );
	}
	m_wndTrackerMode.SetCurSel( nTrackerMode );

	CRect rc;
	m_wndTrackers.GetClientRect( &rc );
	rc.right -= GetSystemMetrics( SM_CXVSCROLL );
	CoolInterface.SetImageListTo( m_wndTrackers, LVSIL_SMALL );
	m_wndTrackers.InsertColumn( 0, _T("Tracker"), LVCFMT_LEFT, rc.right - 80, -1 );
	m_wndTrackers.InsertColumn( 1, _T("Status"), LVCFMT_RIGHT, 80, 0 );
	m_wndTrackers.InsertColumn( 2, _T("Type"), LVCFMT_LEFT, 0, 0 );
	Skin.Translate( _T("CTorrentTrackerList"), m_wndTrackers.GetHeaderCtrl() );

	int nTracker = 0;
	for ( nTracker = 0 ; nTracker < nCount; nTracker++ )
	{
		LV_ITEM pItem = {};
		pItem.mask		= LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		pItem.iItem		= m_wndTrackers.GetItemCount();
		pItem.lParam	= (LPARAM)nTracker;

		if ( oInfo.GetTrackerIndex() == nTracker )
			pItem.iImage = CoolInterface.ImageForID( ID_MEDIA_SELECT );
		else
			pItem.iImage = CoolInterface.ImageForID( ID_DOWNLOADS_URI );

		pItem.pszText	= (LPTSTR)(LPCTSTR)oInfo.GetTrackerAddress( nTracker );
		pItem.iItem		= m_wndTrackers.InsertItem( &pItem );

		// Display status
		CString sStatus;
		switch ( oInfo.GetTrackerStatus( nTracker ) )
		{
		case TRI_UNKNOWN:
			LoadString( sStatus, IDS_STATUS_UNKNOWN );
			break;
		case TRI_FALSE:
			LoadString( sStatus, IDS_STATUS_TRACKERDOWN );
			break;
		case TRI_TRUE:
			LoadString( sStatus, IDS_STATUS_ACTIVE );
			break;
		}
		m_wndTrackers.SetItemText( pItem.iItem, 1, sStatus );

		// Display type
		CString sType;
		if ( oInfo.IsMultiTracker() )
			sType.Format( _T("Tier %i"), oInfo.GetTrackerTier( nTracker ) );
		else
			sType = _T("Announce");
		m_wndTrackers.SetItemText( pItem.iItem, 2, sType );
	}

	GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( nTrackerMode != CBTInfo::tNull );

	UpdateData( FALSE );

	if ( Network.IsConnected() )
		PostMessage( WM_COMMAND, MAKELONG( IDC_TORRENT_REFRESH, BN_CLICKED ),
			(LPARAM)m_wndRefresh.GetSafeHwnd() );

	return TRUE;
}

void CTorrentTrackersPage::OnDestroy() 
{
	if ( IsThreadAlive() ) 
	{
		m_pRequest.Cancel();
		CloseThread();
	}
	
	CPropertyPageAdv::OnDestroy();
}

void CTorrentTrackersPage::OnTorrentRefresh() 
{
	if ( m_wndRefresh.IsWindowEnabled() == FALSE ) return;
	
	if ( IsThreadAlive() ) 
	{
		m_pRequest.Cancel();
		CloseThread();
	}
	
	m_wndRefresh.EnableWindow( FALSE );
	BeginThread( "PageTorrentTrackers" );
}

void CTorrentTrackersPage::OnTimer(UINT_PTR nIDEvent) 
{
	if ( nIDEvent == 1 )
	{
		// Re-enable the refresh button
		m_wndRefresh.EnableWindow( TRUE );
		KillTimer( 1 );
	}
	else
	{
		// Close the scrape thread
		CloseThread();

		// Re-enable the refresh button
		SetTimer( 1, 4000, NULL );

		if ( nIDEvent == 3 )
		{
			// Update the display
			CString str;
			str.Format( _T("%i"), m_nComplete );
			m_wndComplete.SetWindowText( str );
			str.Format( _T("%i"), m_nIncomplete );
			m_wndIncomplete.SetWindowText( str );
		}
	}
}

BOOL CTorrentTrackersPage::OnApply()
{
	if ( ! UpdateData() )
		return FALSE;

	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return FALSE;

	if ( ! Downloads.Check( m_pDownload ) || ! m_pDownload->IsTorrent() )
		return FALSE;

	CBTInfo& oInfo = m_pDownload->m_pTorrent;

	int nTrackerMode = m_wndTrackerMode.GetCurSel();
	bool bAddressChanged = oInfo.GetTrackerAddress() != m_sTracker;
	bool bModeChanged = oInfo.GetTrackerMode() != nTrackerMode;
	if ( bAddressChanged || bModeChanged )
	{
		oLock.Unlock();

		GetDlgItem( IDC_TORRENT_TRACKER )->SetFocus();

		// Display warning
		CString strMessage;
		LoadString( strMessage, IDS_BT_TRACK_CHANGE );
		if ( AfxMessageBox( strMessage, MB_ICONQUESTION|MB_YESNO ) != IDYES )
			return FALSE;

		if ( ! oLock.Lock( 250 ) )
			return FALSE;

		if ( bAddressChanged )
			oInfo.SetTracker( m_sTracker );
		if ( bModeChanged )
			oInfo.SetTrackerMode( nTrackerMode );
	}

	return CPropertyPageAdv::OnApply();
}

void CTorrentTrackersPage::OnRun()
{
	CSingleLock oLock( &Transfers.m_pSection );
	if ( oLock.Lock( 250 ) )
	{
		if ( Downloads.Check( m_pDownload ) && m_pDownload->IsTorrent() )
		{
			m_pRequest.Clear();

			CString strURL = m_sTracker;
			if ( strURL.Replace( _T("/announce"), _T("/scrape") ) == 1 )
			{
				// Fetch scrape only for the given info hash
				strURL = strURL.TrimRight( _T('&') ) +
					( ( strURL.Find( _T('?') ) != -1 ) ? _T('&') : _T('?') ) +
					_T("info_hash=") +
						CBTTrackerRequest::Escape( m_pDownload->m_pTorrent.m_oBTH ) +
					_T("&peer_id=") +
						CBTTrackerRequest::Escape( m_pDownload->m_pPeerID );

				oLock.Unlock();

				m_pRequest.SetURL( strURL );
				m_pRequest.AddHeader( _T("Accept-Encoding"), _T("deflate, gzip") );
				m_pRequest.EnableCookie( false );
				m_pRequest.SetUserAgent( Settings.SmartAgent() );

				theApp.Message( MSG_DEBUG | MSG_FACILITY_OUTGOING,
					_T("[BT] Sending BitTorrent tracker scrape: %s"), strURL );

				if ( m_pRequest.Execute( FALSE ) && m_pRequest.InflateResponse() )
				{
					CBuffer* pResponse = m_pRequest.GetResponseBuffer();

					if ( pResponse != NULL && pResponse->m_pBuffer != NULL )
					{
						if ( CBENode* pNode = CBENode::Decode( pResponse ) )
						{
							theApp.Message( MSG_DEBUG | MSG_FACILITY_INCOMING,
								_T("[BT] Recieved BitTorrent tracker response: %s"), pNode->Encode() );

							if ( OnTree( pNode ) )
							{
								delete pNode;
								PostMessage( WM_TIMER, 3 );
								return;
							}
							
							delete pNode;
						}
					}
				}
			}
		}
	}
	m_pRequest.Clear();
	PostMessage( WM_TIMER, 2 );
}

BOOL CTorrentTrackersPage::OnTree(CBENode* pNode)
{
	if ( ! pNode->IsType( CBENode::beDict ) ) return FALSE;
	
	CBENode* pFiles = pNode->GetNode( "files" );
	if ( ! pFiles->IsType( CBENode::beDict ) ) return FALSE;

	LPBYTE nKey;
	{
		CSingleLock oLock( &Transfers.m_pSection );
		if ( ! oLock.Lock( 250 ) )
			return FALSE;

		if ( ! Downloads.Check( m_pDownload ) || ! m_pDownload->IsTorrent() )
			return FALSE;

		nKey = &m_pDownload->m_pTorrent.m_oBTH[ 0 ];
	}

    CBENode* pFile = pFiles->GetNode( nKey, Hashes::BtHash::byteCount );
	if ( ! pFile->IsType( CBENode::beDict ) ) return FALSE;	
	
	m_nComplete		= 0;
	m_nIncomplete	= 0;
	
	if ( CBENode* pComplete = pFile->GetNode( "complete" ) )
	{
		if ( ! pComplete->IsType( CBENode::beInt ) ) return FALSE;
		// Since we read QWORDs, make sure we won't get negative values;
		// Some buggy trackers send very huge numbers, so let's leave them as
		// the max int.	
		m_nComplete = (int)(pComplete->GetInt() & ~0xFFFF0000);
	}
	
	if ( CBENode* pIncomplete = pFile->GetNode( "incomplete" ) )
	{
		if ( ! pIncomplete->IsType( CBENode::beInt ) ) return FALSE;	
		m_nIncomplete = (int)(pIncomplete->GetInt() & ~0xFFFF0000);
	}
	
	return TRUE;
}

void CTorrentTrackersPage::OnEnChangeTorrentTracker()
{
	if ( ! UpdateData() )
		return;
 
	CSingleLock oLock( &Transfers.m_pSection );
	if ( ! oLock.Lock( 250 ) )
		return;
 
	if ( ! Downloads.Check( m_pDownload ) || m_pDownload->IsMoving() )
		return;

	CBTInfo& oInfo = m_pDownload->m_pTorrent;

	if ( m_sTracker != oInfo.GetTrackerAddress() )
		m_wndTrackerMode.SetCurSel( CBTInfo::tSingle );
	else if ( oInfo.GetTrackerMode() != CBTInfo::tNull )
		m_wndTrackerMode.SetCurSel( oInfo.GetTrackerMode() );
}

void CTorrentTrackersPage::OnNMClickTorrentTrackers(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast< LPNMITEMACTIVATE >( pNMHDR );
	*pResult = 0;

	if ( ! UpdateData() )
		return;

	if ( pNMItemActivate->iItem != -1 )
	{
		m_sTracker = m_wndTrackers.GetItemText( pNMItemActivate->iItem, 0 );
		m_wndTrackerMode.SetCurSel( CBTInfo::tSingle );

		GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( TRUE );

		UpdateData( FALSE );
	}
}

void CTorrentTrackersPage::OnCbnSelchangeTorrentTrackermode()
{
	int nMode = m_wndTrackerMode.GetCurSel();

	GetDlgItem( IDC_TORRENT_TRACKER )->EnableWindow( nMode != CBTInfo::tNull );
}