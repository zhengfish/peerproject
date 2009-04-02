//
// DownloadWithTransfers.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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

#if !defined(AFX_DOWNLOADWITHTRANSFERS_H__B2CEC922_899C_412E_93B6_953073B992DB__INCLUDED_)
#define AFX_DOWNLOADWITHTRANSFERS_H__B2CEC922_899C_412E_93B6_953073B992DB__INCLUDED_

#pragma once

#include "DownloadWithSources.h"

class CConnection;
class CEDClient;
class CDownloadTransfer;

class CDownloadWithTransfers : public CDownloadWithSources
{
// Construction
protected:
	CDownloadWithTransfers();
	virtual ~CDownloadWithTransfers();
	
// Attributes
private:
	CDownloadTransfer*	m_pTransferFirst;
	CDownloadTransfer*	m_pTransferLast;
	int					m_nTransferCount;
	DWORD				m_tTransferStart;

// Operations
public:
	bool		HasActiveTransfers() const;
	DWORD		GetTransferCount() const;
	DWORD		GetTransferCount(int nState, IN_ADDR* const pAddress = NULL) const;
	QWORD		GetAmountDownloadedFrom(IN_ADDR* const pAddress) const;
	void		CloseTransfers();
	DWORD		GetAverageSpeed() const;
	DWORD		GetMeasuredSpeed() const;
	BOOL		OnAcceptPush(const Hashes::Guid& oClientID, CConnection* pConnection);
	BOOL		OnDonkeyCallback(CEDClient* pClient, CDownloadSource* pExcept = NULL);
	BOOL		StartNewTransfer(DWORD tNow = 0);
	BOOL		CanStartTransfers(DWORD tNow);
protected:
	BOOL		StartTransfersIfNeeded(DWORD tNow = 0);
private:
	void		AddTransfer(CDownloadTransfer* pTransfer);
	void		RemoveTransfer(CDownloadTransfer* pTransfer);

// Inlines
public:
	inline bool			ValidTransfer(IN_ADDR* const pAddress, CDownloadTransfer* const pTransfer) const;
	CDownloadTransfer*	GetFirstTransfer() const { return m_pTransferFirst; }

	friend class CDownloadTransfer; // AddTransfer && RemoveTransfer
};

#endif // !defined(AFX_DOWNLOADWITHTRANSFERS_H__B2CEC922_899C_412E_93B6_953073B992DB__INCLUDED_)