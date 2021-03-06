//
// PageProfileContact.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndSettingsPage.h"


class CContactProfilePage : public CSettingsPage
{
	DECLARE_DYNCREATE(CContactProfilePage)

public:
	CContactProfilePage();
	virtual ~CContactProfilePage();

	enum { IDD = IDD_PROFILE_CONTACT };

public:
	CString	m_sEmail;
	CString	m_sMSN;
	CString	m_sYahoo;
	CString	m_sAOL;
	CString	m_sICQ;
	CString m_sJabber;
	CString m_sTwitter;
	CString m_sFacebook;
	CString m_sPeerProjectOrg;

	void	AddAddress(LPCTSTR pszClass, LPCTSTR pszName, LPCTSTR pszAddress);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};
